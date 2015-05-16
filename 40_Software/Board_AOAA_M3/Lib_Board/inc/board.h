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
#ifndef __BOARD_H
#define __BOARD_H

#include "lpc17xx_uart.h"

typedef enum {
  ERR_OK = 0,
  ERR_ARGUMENT,
  ERR_TIMEOUT,
  ERR_NOT_INIT,
  ERR_CAN_SEND,
  ERR_RF_CMD_ERROR,
  ERR_RF_READ_ERROR,

} error_t;

void console_init(void);
uint32_t console_send(uint8_t *txbuf, uint32_t buflen,
		TRANSFER_BLOCK_Type flag);
uint32_t console_sendString(uint8_t *str);
uint32_t console_receive(uint8_t *rxbuf, uint32_t buflen,
		TRANSFER_BLOCK_Type flag);

void i2c0_init(void);
void ssp1_init(void);

void trimpot_init(void);
uint16_t trimpot_get(void);
void rf_uart_init(void);

uint32_t rf_uart_send(uint8_t *txbuf, uint32_t buflen,
		TRANSFER_BLOCK_Type flag);
uint32_t rf_uart_sendString(uint8_t *str);
uint32_t rf_uart_receive(uint8_t *rxbuf, uint32_t buflen);
uint8_t rf_uart_recvIsEmpty(void);

void can1_pinConfig(void);

void emac_pinConfig(void);


int net_init(uint8_t* ipAddr, uint8_t* mask, uint8_t* gateway);
void net_task(void);

#endif /* end __BOARD_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
