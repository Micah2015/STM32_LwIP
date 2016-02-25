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

/* lwIP includes. */
#include <string.h>
#include "err.h"
#include "netif.h"
#include "stm32f10x.h"
#include "etharp.h"
#include "enc28j60.h"
#include "mem.h"
#include "sys.h" 

#define netifMTU							( 1500 )
#define netifINTERFACE_TASK_STACK_SIZE		( 350 )
#define netifGUARD_BLOCK_TIME				( 250 )
#define IFNAME0 'S'
#define IFNAME1 'T'

extern  uint8_t macaddress[6];	   //�� LWIP_Init.c�ļ��ж���
unsigned char Data_Buf[1520 *4 +1];
unsigned char Tx_Data_Buf[1500 *2 ];

/* lwIP definitions. */
struct ethernetif
{
	struct eth_addr *ethaddr;
};

/* Forward declarations. */
err_t  ethernetif_input(struct netif *netif);
//static err_t ethernetif_output( struct netif *netif, struct pbuf *p, struct ip_addr *ipaddr );
err_t ethernetif_init( struct netif *netif );
extern void enc28j60PacketSend(u16 len, u8* packet);

/*-----------------------------------------------------------*/
     //������ʼ������
static void low_level_init( struct netif *netif )
{
	/* set MAC hardware address length */
	netif->hwaddr_len = 6;

	/* set MAC hardware address */
	/* MAC��ַ */
	netif->hwaddr[0] = macaddress[0];
	netif->hwaddr[1] = macaddress[1];
	netif->hwaddr[2] = macaddress[2];
	netif->hwaddr[3] = macaddress[3];
	netif->hwaddr[4] = macaddress[4];
	netif->hwaddr[5] = macaddress[5];

	/* maximum transfer unit */
	/* ����䵥Ԫ */
	netif->mtu = netifMTU;

	/* broadcast capability */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

	/* Initialise the EMAC.  This routine contains code that polls status bits.  
	If the Ethernet cable is not plugged in then this can take a considerable 
	time.  To prevent this starving lower priority tasks of processing time we
	lower our priority prior to the call, then raise it back again once the
	initialisation is complete. */

	enc28j60Init(netif->hwaddr);	  //��ʼ��enc28j60
 //   enc28j60PhyWrite(PHLCON, 0x476);  //����PHY
    enc28j60clkout(1); 				// change clkout from 6.25MHz to 12.5MHz
}		   
/*-----------------------------------------------------------*/

/*
 * low_level_output(): Should do the actual transmission of the packet. The 
 * packet is contained in the pbuf that is passed to the function. This pbuf 
 * might be chained.
 */
static err_t low_level_output( struct netif *netif, struct pbuf *p )  /*�ײ㷢�����ݺ���*/
{     
  struct pbuf *q;
  int i = 0;   

  err_t xReturn = ERR_OK;

  /* Parameter not used. */
  for(q = p; q != NULL; q = q->next) 
  {
    memcpy(&Tx_Data_Buf[i], (u8_t*)q->payload, q->len); // ping -t 192.168.1.15
   	i = i + q->len;
  }		
 	enc28j60PacketSend(i,Tx_Data_Buf); //�������ݰ�

    return xReturn;
}
/*--------------------------------------------p---------------*/

/*
 * low_level_input(): Should allocate a pbuf and transfer the bytes of the 
 * incoming packet from the interface into the pbuf. 
 */

static struct pbuf *low_level_input( struct netif *netif )
{	                                                   

	struct pbuf *q,*p = NULL;
	u16 Len = 0; 

  	int i =0;
			
	Len = enc28j60PacketReceive(1520 *4,  Data_Buf);	//���ؽ��յ������ݰ�����		

	if ( Len == 0 ) return 0;
					
	p = pbuf_alloc(PBUF_RAW, Len, PBUF_POOL);
		
	if (p != NULL) 
	{

        for (q = p; q != NULL; q = q->next) 
		{	 
             memcpy((u8_t*)q->payload, (u8_t*)&Data_Buf[i], q->len);

             i = i + q->len;
        }
		if( i != p->tot_len ){ return 0;}  //���ʱ������������β
    }

	return p;
}
/*-----------------------------------------------------------*/

/*
 * ethernetif_output(): This function is called by the TCP/IP stack when an 
 * IP packet should be sent. It calls the function called low_level_output() 
 * to do the actual transmission of the packet.
 */
//static err_t ethernetif_output( struct netif *netif, struct pbuf *p, struct ip_addr *ipaddr )
//{
//    /* resolve hardware address, then send (or queue) packet */
//    return etharp_output( netif, p, ipaddr );
//}
/*-----------------------------------------------------------*/

/*
 * ethernetif_input(): This function should be called when a packet is ready to 
 * be read from the interface. It uses the function low_level_input() that 
 * should handle the actual reception of bytes from the network interface.
 */	
err_t  ethernetif_input(struct netif *netif)  
{  	
 err_t err = ERR_OK;		
 struct pbuf *p = NULL;
						  			
   /* move received packet into a new pbuf */
  p = low_level_input(netif);

  if (p == NULL) return ERR_MEM;

  err = netif->input(p, netif);
  if (err != ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;
  }

  return err;
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
err_t ethernetif_init( struct netif *netif )
{
struct ethernetif   *ethernetif;

	ethernetif = mem_malloc( sizeof(struct ethernetif) );

	if( ethernetif == NULL )
	{
		LWIP_DEBUGF( NETIF_DEBUG, ("ethernetif_init: out of memory\n\r") );
		return ERR_MEM;
	}

	netif->state = ethernetif;
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	netif->output = etharp_output;
	netif->linkoutput = low_level_output;

	ethernetif->ethaddr = ( struct eth_addr * ) &( netif->hwaddr[0] );

	low_level_init( netif );
	
	return ERR_OK;
}
/*************************************************************************************************************/


