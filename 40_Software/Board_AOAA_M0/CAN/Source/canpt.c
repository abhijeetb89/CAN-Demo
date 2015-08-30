/*****************************************************************************
 *
 *   Copyright(C) 2011, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * Embedded Artists AB assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. Embedded Artists AB
 * reserves the right to make changes in the software without
 * notification. Embedded Artists AB also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "mcu_regs.h"
#include "type.h"
#include "stdio.h"
#include <string.h>
#include "timer32.h"
#include "gpio.h"
#include "i2c.h"

#include "rgb.h"
#include "btn.h"
#include "light.h"
#include "lm75a.h"
#include "board.h"

#include "rom_drivers.h"
#include "can.h"
#include "canpt.h"

#include "cr_section_macros.h"
__BSS(RESERVED) char CAN_driver_memory[184];    /* reserve 184 bytes for CAN driver */

/******************************************************************************
 * Forward declarations
 *****************************************************************************/

// Callback function prototypes
static void CAN_rx(uint8_t msg_obj_num);
static void CAN_tx(uint8_t msg_obj_num);
static void CAN_error(uint32_t error_info);

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

#define SUB_MAX_NUM (20)

typedef struct {
  uint8_t reqId;
  uint8_t periphId;
  uint8_t act;
  uint32_t value;
  uint32_t last;
} sub_t;

#define NUM_RX_MSGS (15)

/******************************************************************************
 * Local variables
 *****************************************************************************/

static uint8_t tmpLedMask = 0;


// CAN on-chip drivers
static ROM **rom = (ROM **)0x1fff1ff8;

// message objects
static CAN_MSG_OBJ msg_obj;
static CAN_MSG_OBJ tx_obj;

// CAN Callback Functions
static CAN_CALLBACKS callbacks = {
    CAN_rx,
    CAN_tx,
    CAN_error,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};


static uint8_t subNumRegistered = 0;
static sub_t subscribers[SUB_MAX_NUM];

//static uint8_t publishSent = 0;

static CAN_MSG_OBJ rxMsgs[NUM_RX_MSGS];
static uint8_t rxMsgIn = 0;
static uint8_t rxMsgOut = 0;


/******************************************************************************
 * Local functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Put a CAN message in the queue
 *
 * Params:
 *   [in] d: message to put in the queue
 *
 *****************************************************************************/
static void q_put(CAN_MSG_OBJ* d)
{
  CAN_MSG_OBJ* m;

  if (rxMsgOut == (rxMsgIn + 1) % NUM_RX_MSGS) {
    return;
  }

  m = &rxMsgs[rxMsgIn];
  rxMsgIn = (rxMsgIn + 1) % NUM_RX_MSGS;

  m->msgobj = d->msgobj;
  m->mode_id = d->mode_id;
  m->mask = d->mask;
  m->dlc = d->dlc;
  memcpy(m->data, d->data, 8);
}

/******************************************************************************
 *
 * Description:
 *    Get a CAN message from the queue
 *
 *****************************************************************************/
static CAN_MSG_OBJ* q_get(void)
{
  CAN_MSG_OBJ* m;

  // empty
  if (rxMsgIn == rxMsgOut) {
    return NULL;
  }

  m = &rxMsgs[rxMsgOut];
  rxMsgOut = (rxMsgOut + 1) % NUM_RX_MSGS;

  return m;
}

/******************************************************************************
 *
 * Description:
 *    Check if the queue is empty
 *
 *****************************************************************************/
static uint8_t q_isEmpty(void)
{
  return (rxMsgIn == rxMsgOut);
}

/******************************************************************************
 *
 * Description:
 *    Set RGB LED request
 *
 * Params:
 *   [in] buf: Received data that will be used to turn on/off RGB leds
 *   [in] len: length of buffer
 *
 *****************************************************************************/
static void setRgb(uint8_t* buf, uint8_t len)
{
  uint8_t onMask = 0;
  uint8_t offMask = 0;
  if (len < 2) {
    return;
  }

  if (buf[1] > 0) {
    onMask = buf[0];
  }
  else {
    offMask = buf[0];
  }

  rgb_setLeds(onMask, offMask);
}

/******************************************************************************
 *
 * Description:
 *    Turn LED13 on/off
 *
 * Params:
 *   [in] buf: Received data that will be used to turn on/off LED
 *   [in] len: length of buffer
 *
 *****************************************************************************/
static void setLed(uint8_t* buf, uint8_t len)
{
  if (len < 1) {
    return;
  }

  led_set(buf[0] == 1);
}

/******************************************************************************
 *
 * Description:
 *    Send message to requesting node
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node
 *   [in] dataLen: length of data (not including data at index 0)
 *
 *****************************************************************************/
static void sendMessage(uint8_t reqId, uint8_t dataLen)
{

  tx_obj.mode_id = reqId;
  tx_obj.msgobj = 3;
  tx_obj.mask = 0x7FF;

  tx_obj.data[0] = CANPT_NODE_UNIQUE_ID;
  tx_obj.dlc = dataLen + 1;

  (*rom)->pCAND->can_transmit(&tx_obj);
}

/******************************************************************************
 *
 * Description:
 *    Send a response to a subscription request
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node
 *   [in] subId: subscription ID
 *
 *****************************************************************************/
static void sendSubResponse(uint8_t reqId, uint8_t subId)
{
  tx_obj.data[1] = (CANPT_MSG_SUB|CANPT_MSG_SUB_RESP);
  tx_obj.data[2] = subId;

  sendMessage(reqId, 2);
}

/******************************************************************************
 *
 * Description:
 *    Send a publish message. Usually as a response to a discovery message.
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node. 0 if the publish should be
 *               broadcasted.
 *
 *****************************************************************************/
static void sendPublish(uint8_t reqId)
{
  if (reqId == 0) {
    // if reqId = 0 -> broadcast message
    reqId = CANPT_MSG_CMN_ID_DISC;
  }

  tx_obj.data[1] = (CANPT_MSG_PUBLISH);
  tx_obj.data[2] = (CANPT_MSG_DEV_TEMP | (CANPT_MSG_DEV_LIGHT >> 4));
  tx_obj.data[3] = (CANPT_MSG_DEV_BTN | (CANPT_MSG_DEV_RGB >> 4));
  tx_obj.data[4] = CANPT_MSG_DEV_LED;

  sendMessage(reqId, 4);
}

/******************************************************************************
 *
 * Description:
 *    Send a response to a poll request.
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node.
 *
 *****************************************************************************/
static void sendPollResponse(uint8_t reqId)
{
  tx_obj.data[1] = (CANPT_MSG_POLL|CANPT_MSG_POLL_RESP);
  sendMessage(reqId, 1);
}

/******************************************************************************
 *
 * Description:
 *    Send measured temperature to CAN node with specified ID
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node
 *
 *****************************************************************************/
static void sendTemperature(uint8_t reqId)
{
  int32_t v = lm75a_readTemp();

  tx_obj.data[1] = (CANPT_MSG_VAL|CANPT_MSG_DEV_TEMP);
  tx_obj.data[2] = ((v >> 8) & 0xff);
  tx_obj.data[3] = (v & 0xff);

  sendMessage(reqId, 3);
}

/******************************************************************************
 *
 * Description:
 *    Send light sensor value to CAN node with specified ID
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node
 *
 *****************************************************************************/
static void sendLight(uint8_t reqId)
{
  uint16_t v = light_read();

  tx_obj.data[1] = (CANPT_MSG_VAL|CANPT_MSG_DEV_LIGHT);
  tx_obj.data[2] = ((v >> 8) & 0xff);
  tx_obj.data[3] = (v & 0xff);

  sendMessage(reqId, 3);
}

/******************************************************************************
 *
 * Description:
 *    Send state of SW5 button to CAN node with specified ID
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node
 *
 *****************************************************************************/
static void sendBtn(uint8_t reqId)
{
  uint8_t v = btn_get();

  tx_obj.data[1] = (CANPT_MSG_VAL|CANPT_MSG_DEV_BTN);
  tx_obj.data[2] = v;

  sendMessage(reqId, 2);
}

/******************************************************************************
 *
 * Description:
 *    Get value from a peripheral and send to the requesting node
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node
 *   [in] periphId: peripheral to read value from
 *
 *****************************************************************************/
static void getValue(uint8_t reqId, uint8_t peripId)
{
  switch(peripId) {
  case CANPT_MSG_DEV_TEMP:
    sendTemperature(reqId);
    break;
  case CANPT_MSG_DEV_LIGHT:
    sendLight(reqId);
    break;
  case CANPT_MSG_DEV_BTN:
    sendBtn(reqId);
    break;
  }
}

/******************************************************************************
 *
 * Description:
 *    Write a value to a peripheral
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node
 *   [in] periphId: peripheral to set
 *   [in] buf: buffer containing the value to set
 *   [in] len: length of buffer
 *
 *****************************************************************************/
static void setValue(uint8_t reqId, uint8_t peripId, uint8_t* buf, int16_t len)
{

  if (len < 1) {
    return;
  }

  switch(peripId) {
  case CANPT_MSG_DEV_RGB:
    setRgb(buf, len);
    break;
  case CANPT_MSG_DEV_LED:
    setLed(buf, len);
    break;

  }
}

/******************************************************************************
 *
 * Description:
 *    Initialize subscription functionality
 *
 *****************************************************************************/
static void subInit(void)
{
  int i = 0;
  subNumRegistered = 0;

  for (i = 0; i < SUB_MAX_NUM; i++) {
    subscribers[i].reqId = 0;
  }

}

/******************************************************************************
 *
 * Description:
 *    Check if a subscription exists
 *
 * Return:
 *   The subscription ID of the existing subscription or 0 if a subscription
 *   doesn't exist.
 *
 *****************************************************************************/
static uint8_t subscriptionExists(uint8_t reqId, uint8_t act,
    uint8_t pId, uint32_t value)
{
  int i = 0;
  uint8_t subId = 0;

  for (i = 0; i < SUB_MAX_NUM; i++) {
    if (subscribers[i].reqId == reqId
        && subscribers[i].periphId == pId
        && subscribers[i].act == act
        && subscribers[i].value == value) {

      subId = (i+1);
      break;
    }
  }

  return subId;
}

/******************************************************************************
 *
 * Description:
 *    Start a subscription
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node
 *   [in] periphId: peripheral to set
 *   [in] buf: buffer containing the value to set
 *   [in] len: length of buffer
 *
 *****************************************************************************/
static void subscribe(uint8_t reqId, uint8_t act, uint8_t* buf, int16_t len)
{
  uint8_t pId = 0;
  int32_t val = 0;
  uint32_t cval = 0;
  int i = 0;
  uint8_t subId = 0;

  if (len < 2) {
    return;
  }

  switch (buf[0]) {
  case CANPT_MSG_DEV_TEMP:
    val = (int16_t)(buf[1] << 8 | buf[2]);
    pId = CANPT_MSG_DEV_TEMP;
    cval = lm75a_readTemp();
    break;
  case CANPT_MSG_DEV_LIGHT:
    val = (uint16_t)(buf[1] << 8 | buf[2]);
    pId = CANPT_MSG_DEV_LIGHT;
    cval = light_read();
    break;
  case CANPT_MSG_DEV_BTN:
    val = buf[1];
    if (val == 0 || val == 1) {
      pId = CANPT_MSG_DEV_BTN;
      cval = btn_get();
    }
    break;
  }

  // not a valid subscription
  if (pId == 0) {
    return;
  }

  // if the subscription already exists respond with that ID
  subId = subscriptionExists(reqId, act, pId, val);
  if (subId != 0) {
    sendSubResponse(reqId, subId);
    return;
  }

  // get free subscription entry
  for (i = 0; i < SUB_MAX_NUM; i++) {
    if (subscribers[i].reqId == 0) {
      break;
    }
  }

  if (i < SUB_MAX_NUM) {
    subscribers[i].reqId = reqId;
    subscribers[i].periphId = pId;
    subscribers[i].act = act;
    subscribers[i].value = val;
    subscribers[i].last = cval;

    subNumRegistered++;

    sendSubResponse(reqId, (i+1));
  }

}

/******************************************************************************
 *
 * Description:
 *    Cancel a subscription
 *
 * Params:
 *   [in] reqId: CAN ID of requesting node
 *   [in] subscription ID
 *
 *****************************************************************************/
static void unsubscribe(uint8_t reqId, uint8_t subId)
{
  if (subId == 0) {
    return;
  }

  subId--;

  if (subId >= SUB_MAX_NUM) {
    return;
  }

  if (subscribers[subId].reqId == reqId) {
    subscribers[subId].reqId = 0;
    subNumRegistered--;
  }
}

/******************************************************************************
 *
 * Description:
 *    Process a received common message
 *
 *****************************************************************************/
static void processCmnMessage(CAN_MSG_OBJ* msg)
{
  uint16_t id = (msg->mode_id & ~CANPT_MSG_CMN_MASK);

  switch (id) {
  case CANPT_MSG_CMN_ID_DISC:
    // ignore any broadcasted publish messages
    if (msg->dlc == 1) {
      sendPublish(msg->data[0]);
    }
    break;
  }

}

/******************************************************************************
 *
 * Description:
 *    Process a received message
 *
 *****************************************************************************/
static void processMessage(CAN_MSG_OBJ* msg) {
  uint8_t reqId = 0;
  uint8_t len = 0;

  len = msg->dlc;

  if (len < 2) {
    return;
  }

  reqId = msg->data[0];

  switch ((msg->data[1] & CANPT_MSG_TYPE_MASK)) {
  case CANPT_MSG_GET:
    getValue(reqId, (msg->data[1] & CANPT_MSG_DEV_MASK));
    break;
  case CANPT_MSG_SET:
    setValue(reqId, (msg->data[1] & CANPT_MSG_DEV_MASK),
        &msg->data[2], len-2);
    break;
  case CANPT_MSG_SUB:
    subscribe(reqId, (msg->data[1] & CANPT_MSG_DEV_MASK), &msg->data[2], len-2);
    break;
  case CANPT_MSG_UNSUB:
    unsubscribe(reqId, msg->data[2]);
    break;
  case CANPT_MSG_POLL:
    sendPollResponse(reqId);
    break;
  }


}

/******************************************************************************
 *
 * Description:
 *    CAN RX callback.
 *
 *    Function is executed by the Callback handler after a CAN message has
 *    been received.
 *
 *****************************************************************************/
static void CAN_rx(uint8_t msg_obj_num){

  /* Determine which CAN message has been received */
  msg_obj.msgobj = msg_obj_num;

  /* Now load up the msg_obj structure with the CAN message */
  (*rom)->pCAND->can_receive(&msg_obj);

  q_put(&msg_obj);

  return;
}

/******************************************************************************
 *
 * Description:
 *    CAN TX callback.
 *
 *    Function is executed by the Callback handler after a CAN message has
 *    been transmitted.
 *
 *****************************************************************************/
static void CAN_tx(uint8_t msg_obj_num){
  return;
}

/******************************************************************************
 *
 * Description:
 *    CAN error callback.
 *
 *    Function is executed by the Callback handler after an error has occurred
 *    on the CAN bus.
 *
 *****************************************************************************/
static void CAN_error(uint32_t error_info){

  tmpLedMask |= RGB_RED;

  rgb_setLeds(tmpLedMask, 0);
  return;
}

/******************************************************************************
 * Public functions
 *****************************************************************************/


/******************************************************************************
 *
 * Description:
 *    Initialize CAN communication and the EA specific protocol used
 *    in the AOA demos.
 *
 *****************************************************************************/
void canpt_init(void)
{
  uint32_t initTable[2];

  // configure bit rate to 125k
  initTable[0] = 0x05; // 48 Mhz/(5+1) -> 8 MHz
  initTable[1] = BITRATE125K8MHZ;

  // Initialize the CAN controller
  (*rom)->pCAND->init_can(&initTable[0], 1);

  // Configure the CAN callback functions
  (*rom)->pCAND->config_calb(&callbacks);


  // Configure message object 1 to receive all common messages
  msg_obj.msgobj = 1;
  msg_obj.mode_id = 0x000;
  msg_obj.mask    = CANPT_MSG_CMN_MASK;
  (*rom)->pCAND->config_rxmsgobj(&msg_obj);

  // Configure message object 2 to receive messages directed to this node
  msg_obj.msgobj = 2;
  msg_obj.mode_id = CANPT_NODE_UNIQUE_ID;
  msg_obj.mask    = 0x7FF;
  (*rom)->pCAND->config_rxmsgobj(&msg_obj);

  subInit();

  /* Enable the CAN Interrupt */
  NVIC_EnableIRQ(CAN_IRQn);
}


/******************************************************************************
 *
 * Description:
 *    Call this function regularly to process messages and handle subscriptions
 *
 *****************************************************************************/
void canpt_task(void)
{
  CAN_MSG_OBJ *msg;
  int i = 0;

  uint32_t last = 0;
  uint32_t value = 0;
  uint32_t read = 0;
  uint8_t update = 0;


//  if (!publishSent) {
//    publishSent = 1;
//    sendPublish(0);
//  }


  if (!q_isEmpty()) {

    msg = q_get();

    if ((msg->mode_id & ~CANPT_MSG_CMN_MASK) != 0) {
      processCmnMessage(msg);
    }
    else {
      processMessage(msg);
    }

  }

  if (subNumRegistered != 0) {

    for (i = 0; i < SUB_MAX_NUM; i++) {
      if (subscribers[i].reqId != 0) {
        last  = subscribers[i].last;
        value = subscribers[i].value;

        switch (subscribers[i].periphId) {
        case CANPT_MSG_DEV_TEMP:
          read = lm75a_readTemp();
          break;
        case CANPT_MSG_DEV_LIGHT:
          read = light_read();
          break;
        case CANPT_MSG_DEV_BTN:
          read = btn_get();
          break;
        }


        switch (subscribers[i].act) {
        case CANPT_MSG_SUB_GTE:

          if (last < value && read >= value) {
            update = 1;
          }

          subscribers[i].last = read;
          break;
        case CANPT_MSG_SUB_LTE:

          if (last > value && read <= value) {
            update = 1;
          }

          subscribers[i].last = read;

          break;
        case CANPT_MSG_SUB_DIF:

          if ((last > read && last-read > value)
              || (read > last && read-last > value)) {
            update = 1;
            subscribers[i].last = read;
          }

          break;
        }

        if (update) {

          switch (subscribers[i].periphId) {
          case CANPT_MSG_DEV_TEMP:
            sendTemperature(subscribers[i].reqId);
            break;
          case CANPT_MSG_DEV_LIGHT:
            sendLight(subscribers[i].reqId);
            break;
          case CANPT_MSG_DEV_BTN:
            sendBtn(subscribers[i].reqId);
            break;
          }

          update = 0;
        }

      }
    }

    return;
  }

  __WFI();

}

/******************************************************************************
 *
 * Description:
 *    CAN interrupt handler.
 *
 *****************************************************************************/
void CAN_IRQHandler (void) {
  // It's function is to call the isr() API located in the ROM
  (*rom)->pCAND->isr();
}

