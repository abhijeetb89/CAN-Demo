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

#include "xbee.h"


/******************************************************************************
 * Typdefs and defines
 *****************************************************************************/

#define XBEE_END_CMD ("\r")
#define CR (0x0D)
#define RESP_OK ("OK")

#define ERROR_BUF_TOO_SMALL (-1)
#define ERROR_TIMEOUT       (-2)

#define XBEE_START_DEL (0x7E)
#define XBEE_BUF_SZ (200)

#define XBEE_API_ID_TX_64    (0x00)
#define XBEE_API_ID_AT_CMD   (0x08)
#define XBEE_API_ID_AT_RESP  (0x88)
#define XBEE_API_ID_TX_STAT  (0x89)
#define XBEE_API_ID_RX_64    (0x80)
#define XBEE_API_ID_RX_16    (0x81)
#define XBEE_API_ID_MOD_STAT (0x8A)

#define XBEE_MOD_STAT_HW_RESET    (0)
#define XBEE_MOD_STAT_WD_RESET    (1)
#define XBEE_MOD_STAT_ASSOC       (2)
#define XBEE_MOD_STAT_DIASSOC     (3)
#define XBEE_MOD_STAT_SYNC_LOST   (4)
#define XBEE_MOD_STAT_COORD_REALG (5)
#define XBEE_MOD_STAT_COORD_START (6)

#define XBEE_TX_STAT_SUCCESS  (0)
#define XBEE_TX_STAT_NOACK    (1)
#define XBEE_TX_STAT_CCA_FAIL (2)
#define XBEE_TX_STAT_PURGED   (3)

#define XBEE_RECV_FRAME_TO (2000)

typedef enum {
  XBEE_FRAME,
  XBEE_LENGTH,
  XBEE_DATA,
} rfState_t;



/******************************************************************************
 * Local variables
 *****************************************************************************/

static xbee_callb_t* _cb = NULL;

static rfState_t rfState = XBEE_FRAME;
static uint32_t rfFrameLen = 0;
static uint32_t rfPos = 0;
static uint8_t rfBuf[XBEE_BUF_SZ];
static uint8_t rfFrameId = 1;
static uint32_t rfFrameTimer = 0;

static uint8_t isCoordinator = 0;
static uint8_t initialized = 0;




/******************************************************************************
 * Local functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *   Convert 4 bytes to 32-bit integer
 *
 * Params:
 *   [in] buf - buffer that must be at least 4 bytes
 *
 * Return:
 *   converted value
 *
 *****************************************************************************/
static uint32_t bufTo32bitInt(uint8_t* buf)
{
  uint32_t v = 0;

  v |= (buf[0] << 24);
  v |= (buf[1] << 16);
  v |= (buf[2] << 8);
  v |= (buf[3]);

  return v;
}

/******************************************************************************
 *
 * Description:
 *   Convert 32-bit integer to 4 bytes data
 *
 * Params:
 *   [in] v - 32-bit integer to convert
 *   [in] buf - buffer that must be at least 4 bytes
 *
 *****************************************************************************/
static void int32bitToBuf(uint32_t v, uint8_t* buf)
{

  buf[0] = ((v >> 24) & 0xff);
  buf[1] = ((v >> 16) & 0xff);
  buf[2] = ((v >> 8) & 0xff);
  buf[3] = ((v >> 0) & 0xff);

}

/******************************************************************************
 *
 * Description:
 *   Get a new frame ID
 *
 * Returns:
 *   New frame ID
 *
 *****************************************************************************/
static uint8_t getFrameId(void)
{
  rfFrameId++;
  if (rfFrameId == 0) {
    rfFrameId = 1;
  }

  return rfFrameId;
}

/******************************************************************************
 *
 * Description:
 *   Calculate checksum
 *
 * Params:
 *   [in] buf - buffer containing data
 *   [in] len - number of bytes
 *
 * Returns:
 *   Calculated checksum
 *
 *****************************************************************************/
static uint8_t checksum(uint8_t* buf, uint32_t len)
{
  int i = 0;
  uint8_t cs = 0;

  for (i = 0; i < len; i++) {
    cs += buf[i];
  }

  return (0xFF - cs);
}

/******************************************************************************
 *
 * Description:
 *   Send AT command to the XBee module when in API mode
 *
 * Params:
 *   [in] atCmd - two ascii characters indetifying the AT command
 *   [in] param - parameter to the AT command
 *   [in] setParam - 1 if a paramter is used; otherwise 0
 *
 * Returns:
 *   ERR_OK or an error code if the request failed.
 *
 *****************************************************************************/
static error_t apiAtCmd(uint8_t* atCmd, uint32_t param, uint8_t setParam)
{
  uint8_t buf[12];
  uint8_t pos = 0;

  buf[0] = XBEE_START_DEL;

  // length
  buf[1] = 0;
  buf[2] = 4;
  if (setParam) {
    buf[2] += 4;
  }

  // AP ID
  buf[3] = XBEE_API_ID_AT_CMD;

  // frame ID
  buf[4] = getFrameId();

  // AT cmd
  buf[5] = atCmd[0];
  buf[6] = atCmd[1];
  pos = 7;

  // AT parameter
  if (setParam) {
    buf[pos++] = ((param >> 24) & 0xff);
    buf[pos++] = ((param >> 16) & 0xff);
    buf[pos++] = ((param >> 8) & 0xff);
    buf[pos++] = ((param >> 0) & 0xff);
  }

  // checksum
  buf[pos] = checksum(&buf[3], pos-3);
  pos++;

  rf_uart_send(buf, pos, BLOCKING);

  return ERR_OK;
}

/******************************************************************************
 *
 * Description:
 *   Transmit data to other node using 64-bit address
 *
 * Params:
 *   [in] addrHi - upper 32 bits of 64 bit address
 *   [in] addrLo - lower 32 bits of 64 bit address
 *   [in] data - data to transmit
 *   [in] len - number of bytes
 *   [out] frameId - frame ID of the request
 *
 * Returns:
 *   ERR_OK or an error code if the request failed.
 *
 *****************************************************************************/
static error_t apiTx64(uint32_t addrHi, uint32_t addrLo, uint8_t* data,
    uint32_t len, uint8_t* frameId)
{
  uint8_t buf[35];


  // only supports sending max 20 bytes data
  // Note: the Xbee manual says up to 100 bytes per packet.
  //       but we have limited it to 20
  if (len > 20) {
    return ERR_ARGUMENT;
  }

  buf[0] = XBEE_START_DEL;

  // length
  buf[1] = 0;
  buf[2] = 11+len;

  // AP ID
  buf[3] = XBEE_API_ID_TX_64;

  // frame ID
  buf[4] = getFrameId();


  // address
  int32bitToBuf(addrHi, &buf[5]);
  int32bitToBuf(addrLo, &buf[9]);

  // options
  buf[13] = 0;

  // data
  memcpy(&buf[14], data, len);

  // checksum
  buf[14+len] = checksum(&buf[3], buf[2]);

  if (frameId != NULL) {
    *frameId = buf[4];
  }

  rf_uart_send(buf, 15+len, BLOCKING);

  return ERR_OK;
}


/******************************************************************************
 *
 * Description:
 *   Handle a TX status event
 *
 * Params:
 *   [in] frameId - ID of the transmitted frame
 *   [in] status - status of the TX
 *
 *****************************************************************************/
static void handleTxStatus(uint8_t frameId, uint8_t status)
{
  if (_cb->txStat != NULL) {
    switch(status)  {
    case XBEE_TX_STAT_SUCCESS:
      _cb->txStat(frameId, XBEE_TX_STAT_OK);
      break;
    case XBEE_TX_STAT_NOACK:
      _cb->txStat(frameId, XBEE_TX_STAT_NO_ACK);
      break;
    case XBEE_TX_STAT_CCA_FAIL:
      _cb->txStat(frameId, XBEE_TX_STAT_CCA);
      break;
    case XBEE_TX_STAT_PURGED:
      _cb->txStat(frameId, XBEE_TX_STAT_PURGED);
      break;
    }
  }
#ifdef DEBUG
  else {
    dbg("Xbee: Tx status = %d\r\n", status)
  }
#endif
}

/******************************************************************************
 *
 * Description:
 *   Process received data
 *
 * Params:
 *   [in] addrHi - upper 32 bits of 64 bit address
 *   [in] addrLo - lower 32 bits of 64 bit address
 *   [in] rssi - signal strength
 *   [in] buf - recieved data
 *   [in] len - number of bytes
 *
 *****************************************************************************/
static void processData(uint32_t addrHi, uint32_t addrLo, uint8_t rssi,
    uint8_t opt, uint8_t* buf, uint32_t len)
{


  if (_cb->data != NULL) {
    _cb->data(addrHi, addrLo, rssi, buf, len);
  }
#ifdef DEBUG
  else {
    int i = 0;
    sprintf((char*)g_dbbuf, "data: %x:%x, rssi=%d, opt=%d, len=%d\r\n",
        addrHi, addrLo, rssi, opt, len);
    console_sendString(g_dbbuf);

    for (i = 0; i < len; i++) {
      sprintf((char*)g_dbbuf, "%x ", buf[i]);
      console_sendString(g_dbbuf);
    }
    console_sendString((uint8_t*)"\r\n");
  }
#endif


}


/******************************************************************************
 *
 * Description:
 *   Handle a Node Discovery response
 *
 * Params:
 *   [in] status - 0 = OK, 1= ERROR
 *   [in] buf - data part of the response
 *   [in] len - number of bytes
 *
 *****************************************************************************/
static void handleDiscovery(uint8_t status, uint8_t* buf, uint32_t len)
{
  uint32_t addrHi = 0;
  uint32_t addrLo = 0;

  if (status == 0 && len >= 11 && _cb->node != NULL) {
    addrHi = bufTo32bitInt(&buf[2]);
    addrLo = bufTo32bitInt(&buf[6]);

    // report the node (address + signal strength)
    _cb->node(addrHi, addrLo, buf[10]);

  }
}

/******************************************************************************
 *
 * Description:
 *   Handle an AT response packet
 *
 * Params:
 *   [in] frameId - frame ID
 *   [in] atBuf - 2 bytes (ASCII) identifying the AT command
 *   [in] status - status of the command
 *   [in] valueBuf - value is stored in this buffer
 *   [in] valueLen - number of bytes in the valueBuf
 *
 *****************************************************************************/
static void handleAtResponse(uint8_t frameId, uint8_t* atBuf, uint8_t status,
    uint8_t* valueBuf, uint32_t valueLen)
{

  if (strncmp("ND", (char*)atBuf, 2) == 0) {
    handleDiscovery(status, valueBuf, valueLen);
  }
#ifdef DEBUG
  else {
    int i = 0;

    dbg("Xbee: AT response %x, %c%c, stat=%d, len=%d\r\n    ",
        frameId, atBuf[0], atBuf[1], status, valueLen);

    for (i = 0; i < valueLen; i++) {
      sprintf((char*)g_dbbuf, "%x ",
          valueBuf[i]);
      console_sendString(g_dbbuf);
    }
    console_sendString((uint8_t*)"\r\n");
  }
#endif

}

/******************************************************************************
 *
 * Description:
 *   Handle modem status
 *
 * Params:
 *   [in] status - modem status
 *
 *****************************************************************************/
static void handleModemStatus(uint8_t status)
{
  dbg("Xbee: Modem status %d\r\n", status)

  if (isCoordinator && status == XBEE_MOD_STAT_COORD_START) {
    initialized = 1;
    if (_cb->up != NULL) {
      _cb->up(1);
    }
  }
  else if (!isCoordinator && status == XBEE_MOD_STAT_ASSOC) {
    initialized = 1;
    if (_cb->up != NULL) {
      _cb->up(1);
    }
  }
  else if (!isCoordinator && status == XBEE_MOD_STAT_DIASSOC) {
    initialized = 0;
    if (_cb->up != NULL) {
      _cb->up(0);
    }
  }

}

/******************************************************************************
 *
 * Description:
 *   Process a received frame (API mode)
 *
 * Params:
 *   [in] buf - buffer containing the frame
 *   [in] len - number of received bytes
 *
 *****************************************************************************/
static void processFrame(uint8_t* buf, uint32_t len)
{
#ifdef DEBUG
  int i = 0;
#endif

  uint32_t addrLo = 0;
  uint32_t addrHi = 0;
  uint8_t* b = NULL;
  uint32_t bLen = 0;

  if (len < 2) {
    dbg("Xbee: Invalid frame length (%d)\r\n", len);
    return;
  }

  // verify checksum
  if (checksum(buf, len) != 0) {
    dbg("Xbee: Invalid checksum\r\n");
    return;
  }

  switch(buf[0]) {
  case XBEE_API_ID_AT_RESP:
    if (len < 5) {
      dbg("Xbee: AT resp data too small: %d\r\n ", len);
      return;
    }

    // there is a value
    if (len > 6) {
      b = &buf[5];
      bLen = len-5-1;
    }

    handleAtResponse(buf[1], &buf[2], buf[4], b, bLen);
    break;
  case XBEE_API_ID_TX_STAT:
    handleTxStatus(buf[1], buf[2]);
    break;
  case XBEE_API_ID_RX_64:
    if (len < 12) {
      dbg("Xbee: RX data too small: %d\r\n ", len);
      return;
    }
    addrHi = bufTo32bitInt(&buf[1]);
    addrLo = bufTo32bitInt(&buf[5]);

    processData(addrHi, addrLo, buf[9], buf[10], &buf[11], len-11-1);
    break;
  case XBEE_API_ID_RX_16:
    dbg("Xbee: RX 16 bit (unhandled)\r\n");
    break;
  case XBEE_API_ID_MOD_STAT:
    handleModemStatus(buf[1]);
    break;
  default:
    dbg("Xbee: Unhandled API ID: %x\r\n ", buf[0]);
#ifdef DEBUG
    for (i = 1; i < len-1; i++) {
      sprintf((char*)g_dbbuf, "%x ", buf[i]);
      console_sendString(g_dbbuf);
    }
    console_sendString((uint8_t*)"\r\n");
#endif
    break;
  }

}

/******************************************************************************
 *
 * Description:
 *   Process one byte of data received from the XBee module
 *
 * Params:
 *   [in] data - the received data
 *
 *****************************************************************************/
static void processByte(uint8_t data)
{
  switch(rfState) {
  case XBEE_FRAME:
    if (data == XBEE_START_DEL) {
      rfPos = 0;
      rfFrameLen = 0;
      rfState = XBEE_LENGTH;

      // start timer to make sure an entire frame is received
      // within a specific time
      rfFrameTimer = time_get() + XBEE_RECV_FRAME_TO;
    }

    break;
  case XBEE_LENGTH:
    rfFrameLen |= (data << (8*(1-rfPos)));
    rfPos++;
    if (rfPos == 2) {
      rfPos = 0;
      rfState = XBEE_DATA;

      if (rfFrameLen > XBEE_BUF_SZ) {
        dbg("Xbee: Frame len %d > max buffer len %d\r\n", rfFrameLen, XBEE_BUF_SZ);
        rfFrameLen = XBEE_BUF_SZ;
      }

    }
    break;
  case XBEE_DATA:
    rfBuf[rfPos++] = data;
    // read up until checksum (1 byte)
    if (rfPos == rfFrameLen+1) {
      rfState = XBEE_FRAME;

      // cancel timer
      rfFrameTimer = 0;

      processFrame(rfBuf, rfPos);
    }
    break;

  }
}

/******************************************************************************
 *
 * Description:
 *   Read data from UART up until CR is found.
 *
 * Params:
 *   [in] buf - read data is copied to this buffer
 *   [in] bufLen - size of buffer
 *   [in] timeout - max number of ms to wait for data
 *
 * Returns:
 *   number of read bytes or a negative number if an error occurs
 *
 *****************************************************************************/
static int32_t readLine(uint8_t* buf, uint32_t bufLen, uint32_t timeout)
{
  uint32_t pos = 0;
  uint32_t len = 0;
  uint32_t t = 0;

  t = time_get() + timeout;

  while(pos < bufLen && t > time_get()) {

    len = rf_uart_receive(&buf[pos], 1);
    if (len > 0 && buf[pos] == CR) {
      buf[pos] = '\0';
      break;
    }

    pos += len;
  }

  if (pos >= bufLen) {
    return ERROR_BUF_TOO_SMALL;
  }

  if (t <= time_get()) {
    return ERROR_TIMEOUT;
  }

  return pos;
}

/******************************************************************************
 *
 * Description:
 *   Set the XBee module in command mode
 *
 * Returns:
 *   ERR_OK or an error code if the command failed
 *
 *****************************************************************************/
static error_t commandMode(void)
{
  uint8_t ebuf[10];
  int32_t lineLen = 0;
  error_t err = ERR_OK;

  rf_uart_sendString((uint8_t*)"+++");

  lineLen = readLine(ebuf, 10, 1200);

  do {
    if (lineLen < 0) {
      // error while reading
      err = ERR_RF_READ_ERROR;
      break;
    }

    if (strcmp(RESP_OK, (char*)ebuf) != 0) {
      // didn't receive OK
      err = ERR_RF_CMD_ERROR;
      break;
    }
  } while(0);

  return err;
}

/******************************************************************************
 *
 * Description:
 *   AT command that expects a single line response
 *
 * Params:
 *   [in] atCmd - the AT command string
 *   [in] resp - response buffer
 *   [in] respLen - size of response buffer
 *
 * Returns:
 *   ERR_OK or an error code if the command failed
 *
 *****************************************************************************/
static error_t atGet(uint8_t* atCmd, uint8_t* resp, uint32_t respLen)
{

    int32_t lineLen = 0;
    error_t err = ERR_OK;

    rf_uart_sendString(atCmd);
    rf_uart_sendString((uint8_t*)XBEE_END_CMD);

    do {

      // a response is expected
      if (resp != NULL && respLen > 0) {
        lineLen = readLine(resp, respLen, 1000);

        if (lineLen < 0) {
          // error while reading
          err = ERR_RF_READ_ERROR;
          break;
        }

      }

    } while(0);

    return err;
}

/******************************************************************************
 *
 * Description:
 *   AT command that sets a value and only expects OK or ERROR as result
 *
 * Params:
 *   [in] atCmd - the AT command string
 *
 * Returns:
 *   ERR_OK or an error code if the command failed
 *
 *****************************************************************************/
static error_t atSet(uint8_t* atCmd)
{
  uint8_t b[10];
  error_t err = ERR_OK;

  err = atGet(atCmd, b, 10);
  if (err == ERR_OK) {

    if (strcmp(RESP_OK, (char*)b) != 0) {
      // didn't receive OK
      err = ERR_RF_CMD_ERROR;
    }
  }

  return err;
}


/******************************************************************************
 * Public functions
 *****************************************************************************/


/******************************************************************************
 *
 * Description:
 *   Initialize the Xbee module. This implementation will always initialize
 *   the module to API mode. It is possible to select the module to be
 *   a coordinator or an End-device.
 *
 * Params:
 *   [in] type - XBEE_END_DEVICE or XBEE_COORDINATOR
 *   [in] callbacks - callback functions where events/results are reported
 *
 * Returns:
 *   ERR_OK or an error code if initialization failed
 *
 *****************************************************************************/
error_t xbee_init(xbeeType_t type, xbee_callb_t* callbacks)
{
  error_t err = ERR_OK;

  _cb = callbacks;
  isCoordinator = (type == XBEE_COORDINATOR);


  do {
    // go to command mode
    err = commandMode();
    if (err != ERR_OK) {
      dbg("Xbee: +++ failed\r\n");
      break;
    }

    // change PAN ID to EAEA
    err = atSet((uint8_t*)"ATIDEAEA");
    if (err != ERR_OK) {
      dbg("Xbee: ATIDEAEA failed\r\n");
      break;
    }

    // use 64-bit addressing
    err = atSet((uint8_t*)"ATMYFFFE");
    if (err != ERR_OK) {
      dbg("Xbee: ATMYFFFE failed\r\n");
      break;
    }


    // Coordinator
    if (isCoordinator) {

      // allow End devices to associate with this coordinator
      err = atSet((uint8_t*)"ATA24");
      if (err != ERR_OK) {
        dbg("Xbee: ATA24 failed\r\n");
        break;
      }

      // set this node as coordinator
      err = atSet((uint8_t*)"ATCE1");
      if (err != ERR_OK) {
        dbg("Xbee: ATCE1 failed\r\n");
        break;
      }
    }

    // End-Device
    else {

      // allow End devices to associate with this coordinator
      err = atSet((uint8_t*)"ATA14");
      if (err != ERR_OK) {
        dbg("Xbee: ATA14 failed\r\n");
        break;
      }

    }

    // change to API mode
    err = atSet((uint8_t*)"ATAP1");
    if (err != ERR_OK) {
      dbg("Xbee: ATAP1 failed\r\n");
      break;
    }
  } while (0);


  return err;
}

/******************************************************************************
 *
 * Description:
 *   Send data to an Xbee module.
 *
 * Params:
 *   [in] addrHi - Upper 32 bits of a 64-bit node address
 *   [in] addrLo - Lower 32 bits of a 64-bit node address
 *   [in] data   - buffer containing data to send
 *   [in] len    - number of bytes to send
 *   [out] frameId - frame ID of the request
 *
 * Returns:
 *   ERR_OK or an error code if failed
 *
 *****************************************************************************/
error_t xbee_send(uint32_t addrHi, uint32_t addrLo, uint8_t* data,
    uint8_t len, uint8_t* frameId)
{
  if (!initialized) {
    return ERR_NOT_INIT;
  }

  return apiTx64(addrHi, addrLo, data, len, frameId);
}

/******************************************************************************
 *
 * Description:
 *   Make a Node Discovery request. A node discovery is used to discover and
 *   report all modules on the current channel and PAN ID.
 *
 * Returns:
 *   ERR_OK or an error code if the request failed
 *
 *****************************************************************************/
error_t xbee_nodeDiscover(void)
{
  if (!initialized) {
    return ERR_NOT_INIT;
  }

  return apiAtCmd((uint8_t*)"ND", 0, 0);
}

/******************************************************************************
 *
 * Description:
 *   Call this function repeatedly to run the Xbee driver
 *
 *****************************************************************************/
void xbee_task(void)
{
  uint32_t len = 0;
  uint8_t data = 0;

  if (rfFrameTimer > 0 && rfFrameTimer < time_get()) {
    rfState = XBEE_FRAME;
    dbg("Xbee: Frame timer expired\r\n");
    rfFrameTimer = 0;
  }

  if (!rf_uart_recvIsEmpty()) {

    len = rf_uart_receive(&data, 1);
    if (len > 0) {
      processByte(data);
    }

  }
}





