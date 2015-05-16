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
#ifndef __XBEE_H
#define __XBEE_H

/******************************************************************************
 * Includes
 *****************************************************************************/

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

#define XBEE_ADDRLO_BROADCAST (0x0000FFFF)
#define XBEE_ADDRHI_BROADCAST (0x00000000)

typedef enum
{
  XBEE_END_DEVICE = 0,
  XBEE_COORDINATOR
} xbeeType_t;

typedef enum
{
  XBEE_TX_STAT_OK = 0,
  XBEE_TX_STAT_NO_ACK,
  XBEE_TX_STAT_CCA,
  XBEE_TX_STAT_PURGED,
} xbeeTxStatus_t;

typedef struct {
  void (*up)(uint8_t up);
  void (*node)(uint32_t addrHi, uint32_t addrLo, uint8_t rssi);
  void (*txStat)(uint8_t frameId, xbeeTxStatus_t error);
  void (*data)(uint32_t addrHi, uint32_t addrLo, uint8_t rssi,
      uint8_t* buf, uint8_t len);

} xbee_callb_t;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

error_t xbee_init(xbeeType_t type, xbee_callb_t* callbacks);
error_t xbee_send(uint32_t addrHi, uint32_t addrLo, uint8_t* data,
    uint8_t len, uint8_t* frameId);
void xbee_task(void);

#endif /* end __XBEE_H */



