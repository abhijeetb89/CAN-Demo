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

/*
 * NOTE: I2C must have been initialized before calling any functions in this
 * file.
 */

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "lpc17xx_i2c.h"
#include "eeprom.h"
#include <string.h>

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

#define I2CDEV LPC_I2C0

#define EEPROM_I2C_ADDR    (0x50)

/*
 * The board can be mounted with either of two EEPROMs
 * - 24LC32AT-I/OT
 * - 24AA02E48T-I/OT (contains globally unique ID to be used e.g. for MAC)
 */
#define EEPROM_24AA_TOTAL_SIZE  128
#define EEPROM_24AA_PAGE_SIZE     8
#define EEPROM_24LC_TOTAL_SIZE 4096
#define EEPROM_24LC_PAGE_SIZE    32


#define EUI48_ORG_ID_SZ (3)

/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/

static const uint8_t eui48_org_id[EUI48_ORG_ID_SZ] = {0x00,0x04,0xa3};
static uint32_t eeprom_total_size = EEPROM_24LC_TOTAL_SIZE;
static uint32_t eeprom_page_size  = EEPROM_24LC_PAGE_SIZE;
static uint32_t eeprom_24aa02e48t = FALSE;

/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void eepromDelay(void)
{
  volatile int i = 0;

  for (i = 0; i <0x20000; i++);
}

static int I2CRead(uint8_t addr, uint8_t* buf, uint32_t len)
{
  I2C_M_SETUP_Type rxsetup;

  rxsetup.sl_addr7bit = addr;
  rxsetup.tx_data = NULL;	// Get address to read at writing address
  rxsetup.tx_length = 0;
  rxsetup.rx_data = buf;
  rxsetup.rx_length = len;
  rxsetup.retransmissions_max = 3;

  if (I2C_MasterTransferData(I2CDEV, &rxsetup, I2C_TRANSFER_POLLING) == SUCCESS){
    return (0);
  } else {
    return (-1);
  }
}

static int I2CWrite(uint8_t addr, uint8_t* buf, uint32_t len)
{
  I2C_M_SETUP_Type txsetup;

  txsetup.sl_addr7bit = addr;
  txsetup.tx_data = buf;
  txsetup.tx_length = len;
  txsetup.rx_data = NULL;
  txsetup.rx_length = 0;
  txsetup.retransmissions_max = 3;

  if (I2C_MasterTransferData(I2CDEV, &txsetup, I2C_TRANSFER_POLLING) == SUCCESS){
    return (0);
  } else {
    return (-1);
  }
}

/******************************************************************************
 * Public Functions
 *****************************************************************************/


/******************************************************************************
 *
 * Description:
 *    Initialize the EEPROM Driver
 *
 *****************************************************************************/
void eeprom_init (void)
{
  uint32_t euiOk = FALSE;
  uint8_t eui[6];

  // detect if 24AA02E48T is used by trying to read the EUI-48
  euiOk = eeprom_readEui48(eui);
  if (euiOk) {
    eeprom_24aa02e48t = TRUE;
  }

  if (eeprom_24aa02e48t) {
    eeprom_total_size = EEPROM_24AA_TOTAL_SIZE;
    eeprom_page_size  = EEPROM_24AA_PAGE_SIZE;
  }
  else {
    eeprom_total_size = EEPROM_24LC_TOTAL_SIZE;
    eeprom_page_size  = EEPROM_24LC_PAGE_SIZE;
  }
}

/******************************************************************************
 *
 * Description:
 *    Read from the EEPROM
 *
 * Params:
 *   [in] buf - read buffer
 *   [in] offset - offset to start to read from
 *   [in] len - number of bytes to read
 *
 * Returns:
 *   number of read bytes or -1 in case of an error
 *
 *****************************************************************************/
int16_t eeprom_read(uint8_t* buf, uint16_t offset, uint16_t len)
{
  uint16_t i;
  uint8_t off[2];

  if (len > eeprom_total_size || offset+len > eeprom_total_size) {
    return -1;
  }

  if (eeprom_24aa02e48t) {
    off[0] = (offset & 0xff);
    I2CWrite(EEPROM_I2C_ADDR, off, 1);
  }
  else {
    off[0] = ((offset >> 8) & 0xff);
    off[1] = (offset & 0xff);
    I2CWrite(EEPROM_I2C_ADDR, off, 2);
  }

  for ( i = 0; i < 0x2000; i++);
  I2CRead(EEPROM_I2C_ADDR, buf, len);


  return len;
}

/******************************************************************************
 *
 * Description:
 *    Write to the EEPROM
 *
 * Params:
 *   [in] buf - data to write
 *   [in] offset - offset to start to write to
 *   [in] len - number of bytes to write
 *
 * Returns:
 *   number of written bytes or -1 in case of an error
 *
 *****************************************************************************/
int16_t eeprom_write(uint8_t* buf, uint16_t offset, uint16_t len)
{
  int16_t written = 0;
  uint16_t wLen = 0;
  uint16_t off = offset;
  uint8_t tmp[eeprom_page_size+2];

  if (len > eeprom_total_size || offset+len > eeprom_total_size) {
    return -1;
  }

  wLen = eeprom_page_size - (off % eeprom_page_size);
  wLen = MIN(wLen, len);

  while (len) {

    if (eeprom_24aa02e48t) {
      tmp[0] = (off & 0xff);
      memcpy(&tmp[1], (void*)&buf[written], wLen);
    }
    else {
      tmp[0] = ((off >> 8) & 0xff);
      tmp[1] = (off & 0xff);
      memcpy(&tmp[2], (void*)&buf[written], wLen);
    }

    if (eeprom_24aa02e48t) {
      I2CWrite(EEPROM_I2C_ADDR, tmp, wLen+1);
    }
    else {
      I2CWrite(EEPROM_I2C_ADDR, tmp, wLen+2);
    }

    /* delay to wait for a write cycle */
    eepromDelay();

    len     -= wLen;
    written += wLen;
    off     += wLen;

    wLen = MIN(eeprom_page_size, len);
  }

  return written;
}

/******************************************************************************
 *
 * Description:
 *    Read globally unique EUI-48 ID (6 bytes)
 *
 * Params:
 *   [in] buf - 6 byte long buffer
 *
 * Returns:
 *   TRUE if successful; otherwise FALSE
 *
 *****************************************************************************/
uint32_t eeprom_readEui48(uint8_t* buf)
{
  uint16_t i;
  uint8_t off[1];

  off[0] = 0xFA;
  I2CWrite(EEPROM_I2C_ADDR, off, 1);

  for ( i = 0; i < 0x2000; i++);
  I2CRead(EEPROM_I2C_ADDR, buf, 6);

  if (memcmp(buf, eui48_org_id, EUI48_ORG_ID_SZ) == 0) {
    return TRUE;
  }

  return FALSE;
}

