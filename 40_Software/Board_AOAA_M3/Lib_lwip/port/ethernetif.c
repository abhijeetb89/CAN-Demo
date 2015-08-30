/**
 * @file
 * Ethernet Interface
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "lwip/timers.h"


#include "lpc17xx_emac.h"
#include "lpc17xx_timer.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#define LINK_CHECK_MS (2000)

static u32_t lastLinkCheck = 0;

// only supports one interface
struct netif *eth0Netif = NULL;




/* Forward declarations. */
static void  ethernetif_input(struct netif *netif);

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static err_t
low_level_init(struct netif *netif)
{
  EMAC_CFG_Type emacConfig;
  EMAC_Status status;

  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] = 0x00;
  netif->hwaddr[1] = 0x1A;
  netif->hwaddr[2] = 0x00;
  netif->hwaddr[3] = 0xF1;
  netif->hwaddr[4] = 0x01;
  netif->hwaddr[5] = 0x36;
  netif->hwaddr[6] = 0x9C;

  /* maximum transfer unit */
  netif->mtu = 1500;
  
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
 
  emacConfig.Mode = EMAC_MODE_AUTO;
  emacConfig.pbEMAC_Addr = &netif->hwaddr[0];

  status = EMAC_Init(&emacConfig);
  if (status != EMAC_SUCCESS && status != EMAC_LINK_DOWN) {
    return ERR_IF;
  }

  if (status == EMAC_SUCCESS) {
    netif_set_link_up(netif);
  }

  return ERR_OK;
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  u8_t* dst;
  u32_t idx, sz = 0;


  if (EMAC_CheckTransmitIndex() == FALSE) {
    return ERR_IF;
  }

  
#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

  idx = LPC_EMAC->TxProduceIndex;
  dst  = (u8_t*)TX_DESC_PACKET(idx);

  for(q = p; q != NULL; q = q->next) {
    MEMCPY(dst, (u8_t*) q->payload, q->len);
    dst += q->len;
    sz += q->len;
  }

  TX_DESC_CTRL(idx) = (sz - 1) | (EMAC_TCTRL_INT | EMAC_TCTRL_LAST);
  EMAC_UpdateTxProduceIndex();

  /*
   * TODO: Investigate why this delay is needed. Without it a large file
   * transfer takes significantly longer time.
   *
   * Below is a part of a network capture. One can clearly see that without
   * a delay the transfer is driven by the ACKs which come at about 200 ms
   * delays (Delayed ACK?). With a delay, two data packets are usually sent
   * before an ACK is received and as seen at the end of the excerpt there
   * is no long delay between the sent data and received ACK.
   *
   * Are outgoing packets discarded by the EMAC when there isn't a delay
   * meaning that they will be retransmitted by TCP protocol?
   *
   * Without delay                 With delay
   * --------------------          -------------------
   * > 0.000 GET request           > 0.000 GET request
   * < 0.080 1460 b data           < 0.009 1460 b data
   * > 0.273 TCP ACK               > 0.287 TCP ACK
   * < 0.274 1460 b data           < 0.314 1460 b data
   * > 0.474 TCP ACK               < 0.316 1318 b data
   * < 0.499 1460 b data           > 0.316 TCP ACK
   * > 0.693 TCP ACK               < 0.318 1460 b data
   * < 0.694 1460 b data           > 0.517 TCP ACK
   * > 0.894 TCP ACK               < 0.546 1460 b data
   * < 0.921 1460 b data           < 0.547 142 b data
   * > 1.103 TCP ACK               > 0.547 TCP ACK
   * < 1.104 1460 b data           < 0.549 1460 b data
   * > 1.313 TCP ACK               > 0.747 TCP ACK
   * < 1.336 1460 b data           < 0.771 1460 b data
   * > 1.533 TCP ACK               < 0.772 1460 b data
   * < 1.534 1460 b data           > 0.772 TCP ACK
   * > 1.733 TCP ACK               < 0.802 1460 b data
   * < 1.762 1460 b data           < 0.803 1460 b data
   * > 1.953 TCP ACK               > 0.804 TCP ACK
   * ...
   * ...
   */
  Timer0_Wait(1);

#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

  LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len;
  EMAC_PACKETBUF_Type RxPack;

  if (EMAC_CheckReceiveIndex() == FALSE){
    return (NULL);
  }

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = EMAC_GetReceiveDataSize();

#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  
  if (p != NULL) {

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
    for(q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
       * available data in the pbuf is given by the q->len
       * variable.
       * This does not necessarily have to be a memcpy, you can also preallocate
       * pbufs for a DMA-enabled MAC and after receiving truncate it to the
       * actually received size. In this case, ensure the tot_len member of the
       * pbuf is the sum of the chained pbuf len members.
       */

      RxPack.pbDataBuf = (uint32_t *)q->payload;
      RxPack.ulDataLen = q->len;
      EMAC_ReadPacketBuffer(&RxPack);

    }
    //acknowledge that packet has been read();
    EMAC_UpdateRxConsumeIndex();

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.recv);
  } else {
    //drop packet();
    EMAC_UpdateRxConsumeIndex();
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
  }

  return p;  
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
static void
ethernetif_input(struct netif *netif)
{
  struct pbuf *p;

  do {

    /* move received packet into a new pbuf */
    p = low_level_input(netif);
    if (p != NULL) {
      ethernet_input(p, netif);
    }
  } while (p != NULL);

}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
    

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);


  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  
  // save netif struct
  eth0Netif = netif;
  
  /* initialize the hardware */
  return low_level_init(netif);

}

void ethernetif_poll(void)
{
  u32_t up = 0;

  if (sys_now() > lastLinkCheck + LINK_CHECK_MS) {
    lastLinkCheck = sys_now();

    up = EMAC_CheckPHYStatus(EMAC_PHY_STAT_LINK);
    if (up == 1 && !netif_is_link_up(eth0Netif)) {
      // down -> up
      EMAC_UpdatePHYStatus();
      netif_set_link_up(eth0Netif);
    }
    else if (up == 0 && netif_is_link_up(eth0Netif)) {
      // up -> down
      netif_set_link_down(eth0Netif);
    }
  }

  sys_check_timeouts();
  if (netif_is_link_up(eth0Netif)) {
    ethernetif_input(eth0Netif);
  }
}



