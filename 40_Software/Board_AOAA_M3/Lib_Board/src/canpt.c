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


#include "stdio.h"
#include <string.h>
#include "lpc17xx_can.h"
#include "board.h"
#include "canpt.h"
#include "time.h"

/******************************************************************************
 * Forward declarations
 *****************************************************************************/



/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

#define NODE_MAX_NUM  (10)
#define NODE_MAX_CAPS (12)

typedef struct
{
  uint8_t reqId;
  uint32_t aliveTime;
  uint32_t lastPoll;
  uint8_t caps[NODE_MAX_CAPS];
} node_t;

#define NODE_POLL_TIME  (2500)
#define NODE_ALIVE_TIME (500)

#define NUM_RX_MSGS (5)

/******************************************************************************
 * Local variables
 *****************************************************************************/

static CAN_MSG_Type txMsg;
static CAN_MSG_Type rxMsg;
static canpt_callb_t* _cb = NULL;

static node_t nodes[NODE_MAX_NUM];
static uint8_t numNodes = 0;

static CAN_MSG_Type rxMsgs[NUM_RX_MSGS];
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
static void q_put(CAN_MSG_Type* d)
{
  CAN_MSG_Type* m;

  if (rxMsgOut == (rxMsgIn + 1) % NUM_RX_MSGS) {
    return;
  }

  m = &rxMsgs[rxMsgIn];
  rxMsgIn = (rxMsgIn + 1) % NUM_RX_MSGS;

  m->id = d->id;
  m->type = d->type;
  m->len = d->len;
  m->format = d->format;
  memcpy(m->dataA, d->dataA, 4);
  memcpy(m->dataB, d->dataB, 4);
}

/******************************************************************************
 *
 * Description:
 *    Get a CAN message from the queue
 *
 *****************************************************************************/
static CAN_MSG_Type* q_get(void)
{
  CAN_MSG_Type* m;

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
 *    Handle a publish message
 *
 * Params:
 *   [in] reqId: CAN ID
 *   [in] buf: containing device capabilities
 *   [in] len: length of buffer
 *
 *****************************************************************************/
static void handlePublish(uint8_t reqId, uint8_t* buf, uint8_t len)
{
  int i = 0;
  int j = 0;

  // ignore the message if the node is already registered
  for (i = 0; i < NODE_MAX_NUM; i++) {
    if (nodes[i].reqId == reqId) {
      return;
    }
  }

  for (i = 0; i < NODE_MAX_NUM; i++) {
    if (nodes[i].reqId == 0) {
      nodes[i].reqId = reqId;
      nodes[i].aliveTime = time_get();
      numNodes++;

      for (j = 0; j < len; j++) {
        if (j >= NODE_MAX_CAPS) {
          break;
        }
        nodes[i].caps[j*2]   = ((buf[j]) & 0xf0);

        if (j+1 >= NODE_MAX_CAPS) {
          break;
        }
        nodes[i].caps[j*2+1] = (((buf[j] & 0x0f) << 4)& 0xff);
      }

      break;
    }
  }

  if (i < NODE_MAX_NUM && _cb != NULL && _cb->nodeAttached != NULL ) {
    _cb->nodeAttached(reqId);
  }

}

/******************************************************************************
 *
 * Description:
 *    Handle a value message
 *
 * Params:
 *   [in] reqId: CAN ID
 *   [in] periphId: peripheral ID
 *   [in] buf: buffer containing value
 *   [in] len: length of buffer
 *
 *****************************************************************************/
static void handleValue(uint8_t reqId, uint8_t periphId, uint8_t* buf, uint8_t len)
{
  if (_cb != NULL && _cb->value != NULL) {
    _cb->value(reqId, periphId, buf, len);
  }
}

/******************************************************************************
 *
 * Description:
 *    Handle a poll response message
 *
 * Params:
 *   [in] reqId: CAN ID
 *
 *****************************************************************************/
static void handlePollResponse(uint8_t reqId)
{
  int i = 0;

  for (i = 0; i < NODE_MAX_NUM; i++) {
    if (nodes[i].reqId == reqId) {
      nodes[i].aliveTime = time_get();
      break;
    }
  }
}

/******************************************************************************
 *
 * Description:
 *    Handle a subscribe response message
 *
 * Params:
 *   [in] reqId: CAN ID
 *   []
 *
 *****************************************************************************/
static void handleSubscribeResponse(uint8_t reqId, uint8_t subId)
{
  if (_cb != NULL && _cb->subStarted != NULL) {
    _cb->subStarted(reqId, subId);
  }
}

/******************************************************************************
 *
 * Description:
 *    Send message
 *
 * Params:
 *   [in] reqId: CAN ID
 *   [in] dataLen: length of data (not including data at index 0)
 *
 *****************************************************************************/
static error_t sendMessage(uint8_t reqId, uint8_t dataLen)
{
  Status status;
  error_t err = ERR_OK;
  uint32_t s = 0;
  uint32_t to = 0;

  txMsg.id = reqId;
  txMsg.dataA[0] = CANPT_NODE_UNIQUE_ID;
  txMsg.len = dataLen + 1;

  to = time_get() + 500;
  status = CAN_SendMsg(LPC_CAN1, &txMsg);
  if (status != SUCCESS) {
    err = ERR_CAN_SEND;
  }
  else {

    do {
      s = CAN_GetCTRLStatus(LPC_CAN1, CANCTRL_GLOBAL_STS);

    } while ((s & CAN_GSR_TCS) == 0 && to > time_get());

    if (to <= time_get()) {
      err = ERR_TIMEOUT;
    }

    //while (((s = CAN_GetCTRLStatus(LPC_CAN1, CANCTRL_GLOBAL_STS)) & CAN_GSR_TCS) == 0);
  }

  return err;
}

/******************************************************************************
 *
 * Description:
 *    Send a poll message.
 *
 *****************************************************************************/
static error_t canpt_poll(uint8_t reqId)
{
  txMsg.dataA[1] = CANPT_MSG_POLL;
  return sendMessage(reqId, 1);
}

/******************************************************************************
 *
 * Description:
 *    Process a received broadcast message
 *
 * Params:
 *   [in] msg: the message
 *
 *****************************************************************************/
static void processCmnMessage(CAN_MSG_Type* msg)
{
  uint8_t id = (msg->id & ~CANPT_MSG_CMN_MASK);

  switch (id) {
  case CANPT_MSG_CMN_ID_DISC:

    // only handle broadcasted publish messages, not discover requests
    if (msg->len > 2 && msg->dataA[1] == CANPT_MSG_PUBLISH) {
      handlePublish(msg->dataA[0], &msg->dataA[2], msg->len-2);
    }

    break;
  }
}

/******************************************************************************
 *
 * Description:
 *    Process a message directed to this node
 *
 * Params:
 *   [in] msg: the message
 *
 *****************************************************************************/
static void processMessage(CAN_MSG_Type* msg)
{
  uint8_t reqId = 0;

  reqId = msg->dataA[0];

  switch (msg->dataA[1] & CANPT_MSG_TYPE_MASK) {
  case CANPT_MSG_VAL:
    handleValue(reqId, (msg->dataA[1] & CANPT_MSG_DEV_MASK),
        &msg->dataA[2], msg->len-2);
    break;
  case CANPT_MSG_PUBLISH:
    handlePublish(reqId, &msg->dataA[2], msg->len-2);
    break;
  case CANPT_MSG_POLL:
    if ((msg->dataA[1] & CANPT_MSG_POLL_RESP) != 0) {
      handlePollResponse(reqId);
    }
    break;
  case CANPT_MSG_SUB:
    if ((msg->dataA[1] & CANPT_MSG_SUB_RESP) != 0) {
      handleSubscribeResponse(reqId, msg->dataA[2]);
    }
    break;
  default:
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

  for (i = 0; i < NODE_MAX_NUM; i++) {
    if (nodes[i].reqId != 0) {

      time = time_get();

      if ((nodes[i].aliveTime + NODE_ALIVE_TIME + NODE_POLL_TIME) < time) {
        // remove the node
        if (_cb != NULL && _cb->nodeDetached != NULL) {
          _cb->nodeDetached(nodes[i].reqId);
        }
        nodes[i].reqId = 0;

        continue;
      }

      if ((nodes[i].lastPoll + NODE_POLL_TIME) < time) {
        nodes[i].lastPoll = time;
        canpt_poll(nodes[i].reqId);
      }

    }
  }
}

/******************************************************************************
 *
 * Description:
 *    Get value from a peripheral on a node
 *
 * Params:
 *    [in] reqId - ID of the CAN node
 *    [in] periphId - peripheral ID
 *
 *****************************************************************************/
static error_t canpt_getValue(uint8_t reqId, uint8_t periphId)
{

  if (periphId < CANPT_MSG_DEV_TEMP || periphId > CANPT_MSG_DEV_LED) {
    return ERR_ARGUMENT;
  }

  txMsg.dataA[1] = (CANPT_MSG_GET|periphId);
  return sendMessage(reqId, 1);
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
 * Params:
 *   [in] callbacks: callback functions
 *
 *****************************************************************************/
void canpt_init(canpt_callb_t* callbacks)
{
  _cb = callbacks;

  can1_pinConfig();

  // initialize CAN to use 125 kBit bit rate
  CAN_Init(LPC_CAN1, 125000);

  // set filter to bypass mode (except all)
  CAN_LoadExplicitEntry(LPC_CAN1, CANPT_NODE_UNIQUE_ID, STD_ID_FORMAT);
  CAN_LoadGroupEntry(LPC_CAN1, 0x000, 0x00F, STD_ID_FORMAT);

  txMsg.format = STD_ID_FORMAT;
  txMsg.type = DATA_FRAME;

  rxMsg.id = 0;
  rxMsg.format = 0;
  rxMsg.type = 0;
  rxMsg.len = 0;

  CAN_IRQCmd (LPC_CAN1, CANINT_RIE, ENABLE);

  /* Enable the CAN Interrupt */
  NVIC_EnableIRQ(CAN_IRQn);

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
error_t canpt_getNodes(uint8_t* nodeIdBuf, uint8_t len, uint8_t* numNodes)
{
  int i = 0;
  int pos = 0;

  if (numNodes == NULL || nodeIdBuf == NULL || len == 0) {
    return ERR_ARGUMENT;
  }

  for (i = 0; i < NODE_MAX_NUM; i++) {
    if (nodes[i].reqId != 0 && pos < len) {
      nodeIdBuf[pos++] = nodes[i].reqId;
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
error_t canpt_getNodeCaps(uint8_t nodeId, uint8_t* nodeCapBuf, uint8_t len,
    uint8_t* numCaps)
{
  int i = 0;
  int pos = 0;
  node_t* node = NULL;

  if (numCaps == NULL || nodeCapBuf == NULL || len == 0) {
    return ERR_ARGUMENT;
  }

  *numCaps = 0;

  for (i = 0; i < NODE_MAX_NUM; i++) {
    if (nodes[i].reqId ==  nodeId ) {
      node = &nodes[i];
      break;
    }
  }

  if (node == NULL) {
    return ERR_ARGUMENT;
  }

  for (i = 0; i < NODE_MAX_CAPS; i++) {
    if (pos >= len) {
      break;
    }

    if (node->caps[i] == 0) {
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
 *    Send a discover message to find nodes attached to the CAN network.
 *
 *****************************************************************************/
error_t canpt_discover(void)
{
  return sendMessage(CANPT_MSG_CMN_ID_DISC, 0);
}



/******************************************************************************
 *
 * Description:
 *    Get temperature a remote node
 *
 * Params:
 *    [in] reqId - ID of the CAN node
 *
 *****************************************************************************/
error_t canpt_getTemperature(uint8_t reqId)
{
  return canpt_getValue(reqId, CANPT_MSG_DEV_TEMP);
}

/******************************************************************************
 *
 * Description:
 *    Get value from light sensor on a remote node
 *
 * Params:
 *    [in] reqId - ID of the CAN node
 *
 *****************************************************************************/
error_t canpt_getLight(uint8_t reqId)
{
  return canpt_getValue(reqId, CANPT_MSG_DEV_LIGHT);
}

/******************************************************************************
 *
 * Description:
 *    Get button state from a remote node
 *
 * Params:
 *    [in] reqId - ID of the CAN node
 *
 *****************************************************************************/
error_t canpt_getButton(uint8_t reqId)
{
  return canpt_getValue(reqId, CANPT_MSG_DEV_BTN);
}

/******************************************************************************
 *
 * Description:
 *    Control RGB LED on remote node
 *
 * Params:
 *    [in] reqId - ID of the CAN node
 *    [in] mask - RGB mask (which LEDs to modify)
 *    [in] on - Set to 1 for on and 0 for off
 *
 *****************************************************************************/
error_t canpt_setRgb(uint8_t reqId, uint8_t mask, uint8_t on)
{
  txMsg.dataA[1] = (CANPT_MSG_SET|CANPT_MSG_DEV_RGB);
  txMsg.dataA[2] = mask;
  txMsg.dataA[3] = on;
  return sendMessage(reqId, 3);
}

/******************************************************************************
 *
 * Description:
 *    Control RGB LED on remote node
 *
 * Params:
 *    [in] reqId - ID of the CAN node
 *    [in] mask - RGB mask (which LEDs to modify)
 *    [in] on - Set to 1 for on and 0 for off
 *
 *****************************************************************************/
error_t canpt_setLed(uint8_t reqId, uint8_t on)
{
  txMsg.dataA[1] = (CANPT_MSG_SET|CANPT_MSG_DEV_LED);
  txMsg.dataA[2] = on;
  return sendMessage(reqId, 2);
}

/******************************************************************************
 *
 * Description:
 *    Subscribe to value changes for a specific peripheral on a node
 *
 * Params:
 *    [in] reqId - ID of the CAN node
 *    [in] periphId - peripheral ID
 *    [in] subAct - subscribe action
 *
 *****************************************************************************/
error_t canpt_subscribe(uint8_t reqId, uint8_t periphId, uint8_t subAct,
    uint8_t* valBuf, uint8_t len)
{
  int i = 0;
  uint8_t* d = NULL;
  uint8_t pos = 0;
  uint8_t l = 2;

  if (periphId < CANPT_MSG_DEV_TEMP || periphId > CANPT_MSG_DEV_LED) {
    return ERR_ARGUMENT;
  }

  if (subAct < CANPT_MSG_SUB_GTE || subAct > CANPT_MSG_SUB_DIF) {
    return ERR_ARGUMENT;
  }

  if (len > 4) {
    return ERR_ARGUMENT;
  }

  txMsg.dataA[1] = (CANPT_MSG_SUB|subAct);
  txMsg.dataA[2] = periphId;

  d = &txMsg.dataA[0];
  pos = 3;
  for (i = 0; i < len; i++) {

    if (pos > 3) {
      pos = 0;
      d = &txMsg.dataB[0];
    }

    d[pos++] = valBuf[i];
  }
  l += len;


  return sendMessage(reqId, l);
}

/******************************************************************************
 *
 * Description:
 *    Unsubscribe
 *
 * Params:
 *    [in] reqId - ID of the CAN node
 *    [in] subId - subscription ID
 *
 *****************************************************************************/
error_t canpt_unsubscribe(uint8_t reqId, uint8_t subId)
{
  txMsg.dataA[1] = (CANPT_MSG_UNSUB);
  txMsg.dataA[2] = subId;

  return sendMessage(reqId, 2);
}

/******************************************************************************
 *
 * Description:
 *    Call this function regularly to handle e.g. subscriptions.
 *
 *****************************************************************************/
void canpt_task(void)
{
  CAN_MSG_Type *msg;
  if (!q_isEmpty()) {

    msg = q_get();

    if ((msg->id & ~CANPT_MSG_CMN_MASK) != 0) {
      processCmnMessage(msg);
    }
    else {
      processMessage(msg);
    }

  }

  if (numNodes > 0) {
    checkIfNodesAlive();
  }

}


/******************************************************************************
 *
 * Description:
 *    CAN interrupt handler.
 *
 *****************************************************************************/
void CAN_IRQHandler (void)
{
  uint32_t intStatus = 0;

  intStatus = LPC_CAN1->ICR;

  if (intStatus & 0x01) {
    CAN_ReceiveMsg(LPC_CAN1, &rxMsg);
    q_put(&rxMsg);
  }

}




