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

#include "lpc17xx_uart.h"
#include "lpc17xx_timer.h"
#include <string.h>
#include <stdio.h>
#include "eadebug.h"
#include "board.h"
#include "time.h"
#include "btn.h"
#include "rgb.h"

#include "rfpt.h"
#include "xbee.h"

/******************************************************************************
 * Forward declarations
 *****************************************************************************/

static void xbeeUp(uint8_t up);
static void xbeeNode(uint32_t addrHi, uint32_t addrLo, uint8_t rssi);
static void xbeeTxStatus(uint8_t frameId, xbeeTxStatus_t error);
static void xbeeData(uint32_t addrHi, uint32_t addrLo, uint8_t rssi,
    uint8_t* buf, uint8_t len);

/******************************************************************************
 * Typdefs and defines
 *****************************************************************************/

#define DISCOVER_TIME_MS (6000)


#define RF_MAX_NODES  (10)
#define RF_NODE_MAX_CAPS (12)
#define RF_NODE_POLL_TIME  (1000)
#define RF_NODE_ALIVE_TIME (3000)

typedef struct {
  uint32_t addrHi;
  uint32_t addrLo;
  uint32_t aliveTime;
  uint32_t lastPoll;
  uint8_t caps[RF_NODE_MAX_CAPS];
  uint8_t numCaps;
} node_t;

#define RF_MAX_SUBS (20)

typedef struct {
  uint32_t addrHi;
  uint32_t addrLo;
  uint8_t periphId;
  uint8_t act;
  uint32_t value;
  uint32_t last;
} sub_t;

/******************************************************************************
 * Local variables
 *****************************************************************************/


static uint32_t lastDiscover = 0;
static uint8_t isCoordinator = 0;

static node_t nodes[RF_MAX_NODES];

static xbee_callb_t callbacks = {
    xbeeUp,
    xbeeNode,
    xbeeTxStatus,
    xbeeData
};

static rfpt_callb_t* _cb = NULL;

static uint8_t subNumRegistered = 0;
static sub_t subscribers[RF_MAX_SUBS];

/******************************************************************************
 * Local functions
 *****************************************************************************/

static void getAddress(uint8_t nodeId, uint32_t* addrHi, uint32_t* addrLo)
{
  *addrHi = 0;
  *addrLo = 0;

  if (nodeId > 0 && nodeId <= RF_MAX_NODES) {
    *addrHi = nodes[nodeId-1].addrHi;
    *addrLo = nodes[nodeId-1].addrLo;
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

  for (i = 0; i < RF_MAX_SUBS; i++) {
    subscribers[i].addrHi = 0;
    subscribers[i].addrLo = 0;
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
static uint8_t subscriptionExists(uint32_t addrHi, uint32_t addrLo,
    uint8_t action, uint8_t pId, uint32_t value)
{
  int i = 0;
  uint8_t subId = 0;

  for (i = 0; i < RF_MAX_SUBS; i++) {
    if (subscribers[i].addrHi == addrHi
        && subscribers[i].addrLo == addrLo
        && subscribers[i].periphId == pId
        && subscribers[i].act == action
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
 *   Send Publish message
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *
 *****************************************************************************/
static error_t sendPublish(uint32_t addrHi, uint32_t addrLo)
{
  uint8_t id = 0;
  uint8_t buf[5];

  buf[0] = RFPT_MSG_PUBLISH;

  // capabilities
  buf[1] = RFPT_MSG_DEV_BTN;
  buf[2] = RFPT_MSG_DEV_RGB;
  buf[3] = RFPT_MSG_DEV_TEMP;
  buf[4] = RFPT_MSG_DEV_LED;

  return xbee_send(addrHi, addrLo, buf, 5, &id);
}

/******************************************************************************
 *
 * Description:
 *   Send a Poll request
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *
 *****************************************************************************/
static error_t sendPoll(uint32_t addrHi, uint32_t addrLo)
{
  uint8_t id = 0;
  uint8_t buf[1];

  buf[0] = RFPT_MSG_POLL;

  return xbee_send(addrHi, addrLo, buf, 1, &id);
}

/******************************************************************************
 *
 * Description:
 *   Send a Poll response
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *
 *****************************************************************************/
static error_t sendPollResponse(uint32_t addrHi, uint32_t addrLo)
{
  uint8_t id = 0;
  uint8_t buf[1];

  buf[0] = RFPT_MSG_POLL|RFPT_MSG_POLL_RESP;

  return xbee_send(addrHi, addrLo, buf, 1, &id);
}

/******************************************************************************
 *
 * Description:
 *   Send temperature value to requesting node
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *
 *****************************************************************************/
static error_t sendTemperature(uint32_t addrHi, uint32_t addrLo)
{
  uint8_t id = 0;
  uint8_t buf[3];


  // using the value from the trimming potentiometer as
  // temperature
  uint16_t v = trimpot_get();

  buf[0] = (RFPT_MSG_VAL|RFPT_MSG_DEV_TEMP);
  buf[1] = ((v >> 8) & 0xff);
  buf[2] = (v & 0xff);

  return xbee_send(addrHi, addrLo, buf, 3, &id);
}

/******************************************************************************
 *
 * Description:
 *   Send state of button SW2 to requesting node
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *
 *****************************************************************************/
static error_t sendBtn(uint32_t addrHi, uint32_t addrLo)
{
  uint8_t id = 0;
  uint8_t buf[2];
  uint8_t v = btn_get();

  buf[0] = (RFPT_MSG_VAL|RFPT_MSG_DEV_BTN);
  buf[1] = (v & BTN_SW2);


  return xbee_send(addrHi, addrLo, buf, 2, &id);
}

/******************************************************************************
 *
 * Description:
 *   Send a response to a subscription request
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *   [in] subId - subscription ID
 *
 *****************************************************************************/
static error_t sendSubResponse(uint32_t addrHi, uint32_t addrLo, uint8_t subId)
{
  uint8_t id = 0;
  uint8_t buf[2];

  buf[0] = (RFPT_MSG_SUB|RFPT_MSG_SUB_RESP);
  buf[1] = subId;


  return xbee_send(addrHi, addrLo, buf, 2, &id);
}


/******************************************************************************
 *
 * Description:
 *   Handle a Publish message
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *   [in] buf - capabilities
 *   [in] len - number of capabilities
 *
 *****************************************************************************/
static void handlePublish(uint32_t addrHi, uint32_t addrLo, uint8_t* buf,
    uint8_t len)
{

  int i = 0;
  uint8_t freeNodeId = 0;
  uint8_t exists = 0;

  for (i = 0; i < RF_MAX_NODES; i++) {

    // get a free node position in case we need to add the node
    if (freeNodeId == 0 && nodes[i].addrHi == 0
        && nodes[i].addrLo == 0) {
      freeNodeId = (i+1);
    }

    // check if the node exists
    if (nodes[i].addrHi == addrHi && nodes[i].addrLo == addrLo) {
      exists = 1;
      break;
    }

  }

  if (!exists && freeNodeId != 0) {
    uint32_t t = 0;
    // add node to list
    nodes[freeNodeId-1].addrHi = addrHi;
    nodes[freeNodeId-1].addrLo = addrLo;
    nodes[freeNodeId-1].aliveTime = time_get();
    t = time_get();

    for (i = 0; i < RF_NODE_MAX_CAPS && i < len; i++) {
      nodes[freeNodeId-1].caps[i] = buf[i];
    }
    if (len > RF_NODE_MAX_CAPS) {
      nodes[freeNodeId-1].numCaps = RF_NODE_MAX_CAPS;
    }
    else {
      nodes[freeNodeId-1].numCaps = len;
    }

    dbg("new node %x: %x\r\n", addrHi, addrLo);
    if (_cb->nodeAttached != NULL) {
      _cb->nodeAttached(freeNodeId);
    }
  }
}

/******************************************************************************
 *
 * Description:
 *   Handle a poll response
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *
 *****************************************************************************/
static void handlePollResponse(uint32_t addrHi, uint32_t addrLo)
{
  int i = 0;

  for (i = 0; i < RF_MAX_NODES; i++) {

    // check if the node exists
    if (nodes[i].addrHi == addrHi && nodes[i].addrLo == addrLo) {
      nodes[i].aliveTime = time_get();

      break;
    }

  }
}

/******************************************************************************
 *
 * Description:
 *   Handle value update message
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *
 *****************************************************************************/
static void handleValueUpdate(uint32_t addrHi, uint32_t addrLo,
    uint8_t periphId,  uint8_t* buf, int16_t len)
{
  int i = 0;
  if (_cb->value != NULL) {

    for (i = 0; i < RF_MAX_NODES; i++) {

      // get node id and then report value update
      if (nodes[i].addrHi == addrHi && nodes[i].addrLo == addrLo) {
        _cb->value((i+1), periphId, buf, len);

        break;
      }

    }
#ifdef DEBUG
      if (i >= RF_MAX_NODES) {
        dbg("RF: value update from unknown node %x: %x\r\n", addrHi, addrLo);
      }
#endif
  }
}

/******************************************************************************
 *
 * Description:
 *   Handle a subscription request
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *   [in] action - subscribe action
 *   [in] buf - peripheral and value related to the subscribe action
 *   [in] len - numberof bytes in the buffer
 *
 *****************************************************************************/
static void handleSubscribe(uint32_t addrHi, uint32_t addrLo, uint8_t action,
    uint8_t* buf, int16_t len)
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
  case RFPT_MSG_DEV_TEMP:
    val = (int16_t)(buf[1] << 8 | buf[2]);
    pId = RFPT_MSG_DEV_TEMP;

    // using trimming potentiometer for temperature request
    cval = trimpot_get();
    break;
  case RFPT_MSG_DEV_LIGHT:
    // not supported
    break;
  case RFPT_MSG_DEV_BTN:
    val = buf[1];
    if (val == 0 || val == 1) {
      pId = RFPT_MSG_DEV_BTN;
      cval = btn_get();
    }
    break;
  }

  // not a valid subscription
  if (pId == 0) {
    return;
  }

  // if the subscription already exists respond with that ID
  subId = subscriptionExists(addrHi, addrLo, action, pId, val);
  if (subId != 0) {
    sendSubResponse(addrHi, addrLo, subId);
    return;
  }

  // get free subscription entry
  for (i = 0; i < RF_MAX_SUBS; i++) {
    if (subscribers[i].addrHi == 0 && subscribers[i].addrLo == 0) {
      break;
    }
  }

  if (i < RF_MAX_SUBS) {
    subscribers[i].addrHi = addrHi;
    subscribers[i].addrLo = addrLo;
    subscribers[i].periphId = pId;
    subscribers[i].act = action;
    subscribers[i].value = val;
    subscribers[i].last = cval;

    subNumRegistered++;

    sendSubResponse(addrHi, addrLo, (i+1));
  }

}

/******************************************************************************
 *
 * Description:
 *   Handle an unsubscribe request
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *   [in] subId - subscribe ID
 *
 *****************************************************************************/
static void handleUnsubscribe(uint32_t addrHi, uint32_t addrLo, uint8_t subId)
{
  if (subId == 0) {
    return;
  }

  subId--;

  if (subId >= RF_MAX_SUBS) {
    return;
  }

  if (subscribers[subId].addrHi == addrHi && subscribers[subId].addrLo == addrLo) {
    subNumRegistered--;
    subscribers[subId].addrHi = 0;
    subscribers[subId].addrLo = 0;
  }
}

/******************************************************************************
 *
 * Description:
 *   Handle an subscribe response
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *   [in] subId - subscribe ID
 *
 *****************************************************************************/
static void handleSubscribeResponse(uint32_t addrHi, uint32_t addrLo,
    uint8_t subId)
{
  int i = 0;
  uint8_t nodeId = 0;

  for (i = 0; i < RF_MAX_NODES; i++) {
    if (nodes[i].addrHi == addrHi && nodes[i].addrLo == addrLo) {
      nodeId = i+1;
      break;
    }
  }

  if (nodeId > 0 && _cb->subStarted != NULL) {
    _cb->subStarted(nodeId, subId);
  }
}

/******************************************************************************
 *
 * Description:
 *   Get value from a peripheral and send to requesting node
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *   [in] periphId - ID of the peripheral
 *
 *****************************************************************************/
static void getValue(uint32_t addrHi, uint32_t addrLo, uint8_t periphId)
{
  switch (periphId) {
  case RFPT_MSG_DEV_TEMP:
    // using trimming potentiometer
    sendTemperature(addrHi, addrLo);
    break;
//  case RFPT_MSG_DEV_LIGHT:
// no light sensor on AOA board
//    break;
  case RFPT_MSG_DEV_BTN:
    sendBtn(addrHi, addrLo);
    break;
  default:
    dbg("RF: Unknown peripheral %d\r\n", periphId)
    break;
  }
}

/******************************************************************************
 *
 * Description:
 *   Modify the state of a peripheral.
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit address
 *   [in] addrLo - lower 32 bits of the 64-bit address
 *   [in] periphId - ID of the peripheral
 *   [in] buf: buffer containing the value to set
 *   [in] len: length of buffer
 *
 *****************************************************************************/
static void setValue(uint32_t addrHi, uint32_t addrLo, uint8_t periphId,
    uint8_t* buf, int16_t len)
{
  uint8_t on = 0;
  uint8_t off = 0;

  switch (periphId) {
  case RFPT_MSG_DEV_RGB:

    if (len < 2) {
      return;
    }

    if (buf[1] > 0) {
      on = buf[0];
    }
    else {
      off = buf[0];
    }
    rgb_setLeds(LED_6, on, off);
    break;

  case RFPT_MSG_DEV_LED:
    if (len < 1) {
      return;
    }

    if (buf[0] == 1) {
      on = RGB_RED;
    }
    else {
      off = RGB_RED;
    }

    // using the RED LED on RGB_LED 7
    rgb_setLeds(LED_7, on, off);
    break;
  }
}

/******************************************************************************
 *
 * Description:
 *    For attached nodes check if they are still accessible/alive.
 *
 *****************************************************************************/
static void checkIfNodesAlive(void)
{
  int i = 0;
  uint32_t time = 0;

  for (i = 0; i < RF_MAX_NODES; i++) {
    if (nodes[i].addrHi != 0 && nodes[i].addrLo != 0) {

      time = time_get();

      if (nodes[i].aliveTime > 0
          && (nodes[i].aliveTime + RF_NODE_ALIVE_TIME) < time) {
        // remove the node
        dbg("detach: alive=%d, time=%d\r\n", nodes[i].aliveTime, time);
        if (_cb->nodeDetached != NULL) {
          _cb->nodeDetached(i+1);
        }
        nodes[i].addrHi = 0;
        nodes[i].addrLo = 0;
        nodes[i].numCaps = 0;

        continue;
      }

      if ((nodes[i].lastPoll + RF_NODE_POLL_TIME) < time) {
        nodes[i].lastPoll = time;
        sendPoll(nodes[i].addrHi, nodes[i].addrLo);
      }

    }
  }
}

/******************************************************************************
 *
 * Description:
 *   XBee node up/down callback
 *
 * Params:
 *   [in] up - 1 if the node is up, 0 if it is down
 *
 *****************************************************************************/
static void xbeeUp(uint8_t up)
{
  dbg("RF: Xbee Up (%d)\r\n", up);
}

/******************************************************************************
 *
 * Description:
 *   XBee node discover callback. Will be called as a response to a Xbee node
 *   discovery request. All found nodes are reported back one-by-one
 *   through this callback.
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit node address
 *   [in] addrLo - lower 32 bits of the 64-bit node address
 *   [in] rssi - signal strength
 *
 *****************************************************************************/
static void xbeeNode(uint32_t addrHi, uint32_t addrLo, uint8_t rssi)
{
  dbg("RF: Node %x:%x rssi=%d\r\n", addrHi, addrLo, rssi);
}

/******************************************************************************
 *
 * Description:
 *   Transmit status callback. Called as a result of a packet being sent
 *   from the Xbee node.
 *
 * Params:
 *   [in] frameId - ID of the frame that was sent
 *   [in] status - status of the transmit request
 *
 *****************************************************************************/
static void xbeeTxStatus(uint8_t frameId, xbeeTxStatus_t status)
{
  if (status != XBEE_TX_STAT_OK) {
    dbg("RF: [%d] TX failed %d\r\n", frameId, status);
  }
}

/******************************************************************************
 *
 * Description:
 *   Received data callback. Called when data has been received by the Xbee
 *   node.
 *
 * Params:
 *   [in] addrHi - upper 32 bits of the 64-bit node address
 *   [in] addrLo - lower 32 bits of the 64-bit node address
 *   [in] rssi - signal strength
 *   [in] buf - buffer containing the data
 *   [in] len - number of received bytes
 *
 *****************************************************************************/
static void xbeeData(uint32_t addrHi, uint32_t addrLo, uint8_t rssi,
    uint8_t* buf, uint8_t len)
{
  if (len < 1) {
    return;
  }

  switch (buf[0]&0x0F) {
  case RFPT_MSG_GET:
    if (!isCoordinator) {
      getValue(addrHi, addrLo, buf[0]&RFPT_MSG_DEV_MASK);
    }
    break;
  case RFPT_MSG_SET:
    if (!isCoordinator) {
      setValue(addrHi, addrLo, (buf[0]&RFPT_MSG_DEV_MASK), &buf[1], len-1);
    }

    break;
  case RFPT_MSG_SUB:
    if ((buf[0] & 0xF0) == RFPT_MSG_SUB_RESP) {
      handleSubscribeResponse(addrHi, addrLo, buf[1]);
    } else if (!isCoordinator) {
      handleSubscribe(addrHi, addrLo, (buf[0]&RFPT_MSG_DEV_MASK), &buf[1], len-1);
    }

    break;
  case RFPT_MSG_UNSUB:
    if (!isCoordinator) {
      handleUnsubscribe(addrHi, addrLo, buf[1]);
    }

    break;
  case RFPT_MSG_VAL:
    handleValueUpdate(addrHi, addrLo, (buf[0]&RFPT_MSG_DEV_MASK), &buf[1], len-1);
    break;
  case RFPT_MSG_POLL:
    if ((buf[0] & RFPT_MSG_POLL_RESP) != 0) {
      handlePollResponse(addrHi, addrLo);
    }
    else {
      sendPollResponse(addrHi, addrLo);
    }
    break;
  case RFPT_MSG_PUBLISH:
    if (isCoordinator) {
      handlePublish(addrHi, addrLo, &buf[1], len-1);
    }
    break;
  case RFPT_MSG_DISCOVER:
    if (!isCoordinator) {
      sendPublish(addrHi, addrLo);
    }
    break;
  default:
    dbg("RF: Unhandled data %x:%x rssi=%d, len=%d\r\n",
        addrHi, addrLo, rssi, len);
    break;
  }

}

/******************************************************************************
 *
 * Description:
 *   Send a Discover request. Don't confuse this with an Xbee Node Discovery
 *   request. This message is send to the broadcast address.
 *
 *****************************************************************************/
static error_t discover(void)
{
  uint8_t id = 0;
  uint8_t disc = RFPT_MSG_DISCOVER;

  return xbee_send(XBEE_ADDRHI_BROADCAST, XBEE_ADDRLO_BROADCAST, &disc, 1, &id);
}

/******************************************************************************
 * Public functions
 *****************************************************************************/


/******************************************************************************
 *
 * Description:
 *   Initialize the RF protocol
 *
 * Params:
 *   [in] coordinator - set to 1 if the node should be a coordinator; otherwise
 *                      set it to 0.
 *   [in] cbs - callback functions
 *
 *****************************************************************************/
error_t rf_init(uint8_t coordinator, rfpt_callb_t* cbs)
{
  int i = 0;

  if (cbs == NULL) {
    return ERR_ARGUMENT;
  }

  _cb = cbs;
  isCoordinator = (coordinator != 0);

  for (i = 0; i < RF_MAX_NODES; i++) {
    nodes[i].addrHi = 0;
    nodes[i].addrLo = 0;
  }

  subInit();

  return xbee_init((coordinator != 0 ? XBEE_COORDINATOR : XBEE_END_DEVICE),
      &callbacks);
}

/******************************************************************************
 *
 * Description:
 *    Get temperature from a remote node
 *
 * Params:
 *   [in] nodeId - node ID
 *
 *****************************************************************************/
error_t rf_getTemperature(uint8_t nodeId)
{
  uint8_t id = 0;
  uint8_t buf[1];

  uint32_t addrHi;
  uint32_t addrLo;

  getAddress(nodeId, &addrHi, &addrLo);

  buf[0] = (RFPT_MSG_GET|RFPT_MSG_DEV_TEMP);

  return xbee_send(addrHi, addrLo, buf, 1, &id);
}

/******************************************************************************
 *
 * Description:
 *    Get button state from a remote node
 *
 * Params:
 *   [in] nodeId - node ID
 *
 *****************************************************************************/
error_t rf_getButton(uint8_t nodeId)
{
  uint8_t id = 0;
  uint8_t buf[1];
  uint32_t addrHi;
  uint32_t addrLo;

  getAddress(nodeId, &addrHi, &addrLo);

  buf[0] = (RFPT_MSG_GET|RFPT_MSG_DEV_BTN);

  return xbee_send(addrHi, addrLo, buf, 1, &id);
}

/******************************************************************************
 *
 * Description:
 *    Get value from light sensor on remote node
 *
 * Params:
 *   [in] nodeId - node ID
 *
 *****************************************************************************/
error_t rf_getLight(uint8_t nodeId)
{
  uint8_t id = 0;
  uint8_t buf[1];

  uint32_t addrHi;
  uint32_t addrLo;

  getAddress(nodeId, &addrHi, &addrLo);

  buf[0] = (RFPT_MSG_GET|RFPT_MSG_DEV_LIGHT);

  return xbee_send(addrHi, addrLo, buf, 1, &id);
}

/******************************************************************************
 *
 * Description:
 *    Control RGB LED on remote node
 *
 * Params:
 *   [in] nodeId - node ID
 *   [in] mask - LED mask
 *   [in] on - 1 if the mask should be used to turn on LEDs; otherwise 0
 *
 *****************************************************************************/
error_t rf_setRGB(uint8_t nodeId, uint8_t mask, uint8_t on)
{
  uint8_t id = 0;
  uint8_t buf[3];

  uint32_t addrHi;
  uint32_t addrLo;

  getAddress(nodeId, &addrHi, &addrLo);

  buf[0] = (RFPT_MSG_SET|RFPT_MSG_DEV_RGB);
  buf[1] = mask;
  buf[2] = on;

  return xbee_send(addrHi, addrLo, buf, 3, &id);
}

/******************************************************************************
 *
 * Description:
 *    Control LED on remote node
 *
 * Params:
 *    [in] nodeId - ID of the remote node
 *    [in] on - 1 to turn on LED; otherwise 0
 *
 *****************************************************************************/
error_t rf_setLed(uint8_t nodeId, uint8_t on)
{
  uint8_t id = 0;
  uint8_t buf[2];

  uint32_t addrHi;
  uint32_t addrLo;

  getAddress(nodeId, &addrHi, &addrLo);

  buf[0] = (RFPT_MSG_SET|RFPT_MSG_DEV_LED);
  buf[1] = on;

  return xbee_send(addrHi, addrLo, buf, 2, &id);
}

/******************************************************************************
 *
 * Description:
 *    Subscribe to value changes for a specific peripheral on a node
 *
 * Params:
 *    [in] nodeId - ID of the  node
 *    [in] periphId - peripheral ID
 *    [in] subAct - subscribe action
 *
 *****************************************************************************/
error_t rf_subscribe(uint8_t nodeId, uint8_t periphId, uint8_t subAct,
    uint8_t* valBuf, uint8_t len)
{
  int i = 0;
  uint8_t id = 0;
  uint8_t buf[6];
  uint32_t addrHi;
  uint32_t addrLo;

  if (periphId < RFPT_MSG_DEV_TEMP || periphId > RFPT_MSG_DEV_LED) {
    return ERR_ARGUMENT;
  }

  if (subAct < RFPT_MSG_SUB_GTE || subAct > RFPT_MSG_SUB_DIF) {
    return ERR_ARGUMENT;
  }

  if (len > 4) {
    return ERR_ARGUMENT;
  }

  buf[0] = (RFPT_MSG_SUB|subAct);
  buf[1] = periphId;

  for (i = 0; i < len; i++) {
    buf[2+i] = valBuf[i];
  }

  getAddress(nodeId, &addrHi, &addrLo);

  return xbee_send(addrHi, addrLo, buf, 2+len, &id);
}

/******************************************************************************
 *
 * Description:
 *    Unsubscribe
 *
 * Params:
 *    [in] nodeId - ID of the node
 *    [in] subId - subscription ID
 *
 *****************************************************************************/
error_t rf_unsubscribe(uint8_t nodeId, uint8_t subId)
{
  uint8_t id = 0;
  uint8_t buf[2];
  uint32_t addrHi;
  uint32_t addrLo;

  buf[0] = (RFPT_MSG_UNSUB);
  buf[1] = subId;

  getAddress(nodeId, &addrHi, &addrLo);

  return xbee_send(addrHi, addrLo, buf, 2, &id);
}

/******************************************************************************
 *
 * Description:
 *    Get number of discovered nodes
 *
 * Params:
 *   [in] nodeIdBuf - node IDs will be written to this buffer
 *   [in] len - length of buffer
 *   [out] numNodes - number of copied node IDs
 *
 *****************************************************************************/
error_t rf_getNodes(uint8_t* nodeIdBuf, uint8_t len, uint8_t* numNodes)
{
  int i = 0;
  int pos = 0;

  if (numNodes == NULL || nodeIdBuf == NULL || len == 0) {
    return ERR_ARGUMENT;
  }

  for (i = 0; i < RF_MAX_NODES; i++) {
    if (nodes[i].addrHi != 0 && nodes[i].addrLo != 0 && pos < len) {
      nodeIdBuf[pos++] = (i+1);
    }
  }

  *numNodes = pos;

  return ERR_OK;
}

/******************************************************************************
 *
 * Description:
 *    Get capabilities for a specific node
 *
 * Params:
 *   [in] nodeId - Node ID
 *   [in] nodeCapBuf - capability ID will be written to this buffer
 *   [in] len - length of buffer
 *   [out] numCaps - number of copied capabilities
 *
 *****************************************************************************/
error_t rf_getNodeCaps(uint8_t nodeId, uint8_t* nodeCapBuf, uint8_t len,
    uint8_t* numCaps)
{
  int i = 0;
  int pos = 0;
  node_t* node = NULL;

  if (numCaps == NULL || nodeCapBuf == NULL || len == 0) {
    return ERR_ARGUMENT;
  }

  *numCaps = 0;

  if (nodeId > 0 && nodeId <= RF_MAX_NODES) {
    if (nodes[nodeId-1].addrHi != 0 && nodes[nodeId-1].addrLo != 0) {
      node = &nodes[nodeId-1];
    }
  }

  if (node == NULL) {
    return ERR_ARGUMENT;
  }

  for (i = 0; i < RF_NODE_MAX_CAPS; i++) {
    if (pos >= len) {
      break;
    }

    if (pos >= node->numCaps) {
      break;
    }

    nodeCapBuf[pos++] = node->caps[i];
  }

  *numCaps = pos;

  return ERR_OK;
}

/******************************************************************************
 *
 * Description:
 *   Call repeatedly to run the RF module
 *
 *****************************************************************************/
void rf_task(void)
{

  if (isCoordinator) {

    if (lastDiscover + DISCOVER_TIME_MS < time_get()) {
      lastDiscover = time_get();
      discover();
    }

    checkIfNodesAlive();

  }
  else {
    int i = 0;
    uint32_t last = 0;
    uint32_t value = 0;
    uint32_t read = 0;
    uint8_t update = 0;

    if (subNumRegistered != 0) {

      for (i = 0; i < RF_MAX_SUBS; i++) {
        if (subscribers[i].addrHi != 0 && subscribers[i].addrLo) {
          last  = subscribers[i].last;
          value = subscribers[i].value;

          switch (subscribers[i].periphId) {
          case RFPT_MSG_DEV_TEMP:
            // using trimpot value as temperature
            read = trimpot_get();
            break;
          case RFPT_MSG_DEV_LIGHT:
            // not supported on AOA board
            break;
          case RFPT_MSG_DEV_BTN:
            read = btn_get();
            break;
          }


          switch (subscribers[i].act) {
          case RFPT_MSG_SUB_GTE:

            if (last < value && read >= value) {
              update = 1;
            }

            subscribers[i].last = read;
            break;
          case RFPT_MSG_SUB_LTE:

            if (last > value && read <= value) {
              update = 1;
            }

            subscribers[i].last = read;

            break;
          case RFPT_MSG_SUB_DIF:

            if ((last > read && last-read > value)
                || (read > last && read-last > value)) {
              update = 1;
              subscribers[i].last = read;
            }

            break;
          }

          if (update) {

            switch (subscribers[i].periphId) {
            case RFPT_MSG_DEV_TEMP:
              sendTemperature(subscribers[i].addrHi, subscribers[i].addrLo);
              break;
            case RFPT_MSG_DEV_LIGHT:
              // not supported on AOA board
              break;
            case RFPT_MSG_DEV_BTN:
              sendBtn(subscribers[i].addrHi, subscribers[i].addrLo);
              break;
            }

            update = 0;
          }

        }
      }
    }
  }

  xbee_task();
}





