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
#ifndef __CANPT_H
#define __CANPT_H

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

// The Unique ID for this Node
#define CANPT_NODE_UNIQUE_ID (0x10)

// Mask for common IDs -> 0x0 to 0xF
#define CANPT_MSG_CMN_MASK (0x7F0)

// ### Common message IDs 'broadcasted' on the network for all nodes ###

// Discovery ID -> search for nodes or publish a node
#define CANPT_MSG_CMN_ID_DISC (0x01)

// ### Specific messages directed to a specific node ###

#define CANPT_MSG_TYPE_MASK (0x0F)

// Get a value for a peripheral on a node
#define CANPT_MSG_GET   (0x01)
// Set a value for a peripheral on a node
#define CANPT_MSG_SET   (0x02)
// subscribe to value changes for a peripheral on a node
#define CANPT_MSG_SUB   (0x03)
// unsubscribe
#define CANPT_MSG_UNSUB (0x04)
// Value message -> contains a value for a peripheral
#define CANPT_MSG_VAL   (0x05)
// poll a node -> check if it is alive
#define CANPT_MSG_POLL  (0x06)
// publish capabilities (usually as a response to discovery request)
#define CANPT_MSG_PUBLISH  (0x07)
// response bit for a POLL message
#define CANPT_MSG_POLL_RESP  (0x80)


// ### Subscribe actions ###

#define CANPT_MSG_SUB_ACT_MASK (0xF0)

// Greater than or equal -> publish state if >= specified value
#define CANPT_MSG_SUB_GTE (1 << 4)
// Less than or equal -> publish state if <= specified value
#define CANPT_MSG_SUB_LTE (2 << 4)
// Difference -> publish state if it has changed by at least specified value
#define CANPT_MSG_SUB_DIF (3 << 4)
// response to a subscribe request
#define CANPT_MSG_SUB_RESP (0xF << 4)

// ### IDs for specific peripherals on a node ###

#define CANPT_MSG_DEV_MASK (0xF0)

#define CANPT_MSG_DEV_TEMP  (0x01 << 4)
#define CANPT_MSG_DEV_LIGHT (0x02 << 4)
#define CANPT_MSG_DEV_BTN   (0x03 << 4)
#define CANPT_MSG_DEV_RGB   (0x04 << 4)
#define CANPT_MSG_DEV_LED   (0x05 << 4)


typedef struct {
  void (*nodeAttached)(uint8_t reqId);
  void (*nodeDetached)(uint8_t reqId);
  void (*value)(uint8_t reqId, uint8_t periphId, uint8_t* valbuf, uint8_t len);
  void (*subStarted)(uint8_t reqId, uint8_t subId);
} canpt_callb_t;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void canpt_init(canpt_callb_t* callbacks);
error_t canpt_discover(void);
void canpt_task(void);
error_t canpt_subscribe(uint8_t reqId, uint8_t periphId, uint8_t subAct,
    uint8_t* valBuf, uint8_t len);
error_t canpt_unsubscribe(uint8_t reqId, uint8_t subId);
error_t canpt_getTemperature(uint8_t reqId);
error_t canpt_getLight(uint8_t reqId);
error_t canpt_getButton(uint8_t reqId);
error_t canpt_setRgb(uint8_t reqId, uint8_t mask, uint8_t on);
error_t canpt_setLed(uint8_t reqId, uint8_t on);

error_t canpt_getNodes(uint8_t* nodeIdBuf, uint8_t len, uint8_t* numNodes);
error_t canpt_getNodeCaps(uint8_t nodeId, uint8_t* nodeCapBuf, uint8_t len,
    uint8_t* numCaps);

#endif /* end __CANPT_H */

/****************************************************************************
**                            End Of File
*****************************************************************************/



