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
#ifndef __RFPT_H
#define __RFPT_H

/******************************************************************************
 * Includes
 *****************************************************************************/

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

// Get a value for a peripheral on a node
#define RFPT_MSG_GET   (0x01)
// Set a value for a peripheral on a node
#define RFPT_MSG_SET   (0x02)
// subscribe to value changes for a peripheral on a node
#define RFPT_MSG_SUB   (0x03)
// unsubscribe
#define RFPT_MSG_UNSUB (0x04)
// Value message -> contains a value for a peripheral
#define RFPT_MSG_VAL   (0x05)
// poll a node -> check if it is alive
#define RFPT_MSG_POLL  (0x06)
// publish capabilities (usually as a response to discovery request)
#define RFPT_MSG_PUBLISH  (0x07)
// get capabilities from a node
#define RFPT_MSG_DISCOVER  (0x08)

// response bit for a POLL message
#define RFPT_MSG_POLL_RESP  (0x80)

// ### Subscribe actions ###

#define RFPT_MSG_SUB_ACT_MASK (0xF0)

// Greater than or equal -> publish state if >= specified value
#define RFPT_MSG_SUB_GTE (1 << 4)
// Less than or equal -> publish state if <= specified value
#define RFPT_MSG_SUB_LTE (2 << 4)
// Difference -> publish state if it has changed by at least specified value
#define RFPT_MSG_SUB_DIF (3 << 4)
// response to a subscribe request
#define RFPT_MSG_SUB_RESP (0xF << 4)

// ### IDs for specific peripherals on a node ###

#define RFPT_MSG_DEV_MASK (0xF0)

#define RFPT_MSG_DEV_TEMP    (0x01 << 4)
#define RFPT_MSG_DEV_LIGHT   (0x02 << 4)
#define RFPT_MSG_DEV_BTN     (0x03 << 4)
#define RFPT_MSG_DEV_RGB     (0x04 << 4)
#define RFPT_MSG_DEV_LED     (0x05 << 4)


typedef struct {
  void (*nodeAttached)(uint8_t nodeId);
  void (*nodeDetached)(uint8_t nodeId);
  void (*value)(uint8_t nodeId, uint8_t periphId, uint8_t* valbuf, uint8_t len);
  void (*subStarted)(uint8_t nodeId, uint8_t subId);
} rfpt_callb_t;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

error_t rf_init(uint8_t coordinator, rfpt_callb_t* callbacks);
void rf_task(void);
error_t rf_getTemperature(uint8_t nodeId);
error_t rf_getButton(uint8_t nodeId);
error_t rf_getLight(uint8_t nodeId);
error_t rf_setRGB(uint8_t nodeId, uint8_t mask, uint8_t on);
error_t rf_setLed(uint8_t nodeId, uint8_t on);
error_t rf_subscribe(uint8_t nodeId, uint8_t periphId, uint8_t subAct,
    uint8_t* valBuf, uint8_t len);
error_t rf_unsubscribe(uint8_t nodeId, uint8_t subId);
error_t rf_getNodes(uint8_t* nodeIdBuf, uint8_t len, uint8_t* numNodes);
error_t rf_getNodeCaps(uint8_t nodeId, uint8_t* nodeCapBuf, uint8_t len,
    uint8_t* numCaps);

#endif /* end __RFPT_H */



