/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��LWIP_Init.c
 * ����    ��LWIPЭ��ջ��ʼ������ʼ������         
 * ʵ��ƽ̨��Ұ��STM32������
 
 * ��汾  ��ST3.5.0
 * ����    ��wildfire team  
 * ��̳    ��www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/

#include "netconfig.h"	

#define SYSTEMTICK_PERIOD_MS  10 
#define INPUT_TMR_INTERVAL 	  2

__IO uint32_t TCPTimer = 0;
__IO uint32_t ARPTimer = 0;
__IO uint32_t INPUT_Timer=0;

#ifdef LWIP_DHCP
__IO uint32_t DHCPfineTimer = 0;
__IO uint32_t DHCPcoarseTimer = 0;

#endif 
 	
struct netif enc28j60;	  		//netif ����ӿڽṹ��������������Ӳ���ӿڵ�����
uint8_t macaddress[6]={0x54,0x55,0x58,0x10,0x00,0x24};	//��������ӿڵ�mac��ַ
extern __IO uint32_t LocalTime;

/**�ⲿ����*/
extern err_t ethernetif_init( struct netif *netif );  //��ethernetif.c�ж���
extern err_t ethernetif_input( struct netif *netif );  //��ethernetif.c�ж���

/*
 * ��������LWIP_Init
 * ����  ����ʼ��LWIPЭ��ջ����Ҫ�ǰ�enc28j60��LWIP����������
 			����IP��MAC��ַ���ӿں���
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void LwIP_Init( void )
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;

   /*����LWIP��ʼ��������
   ��ʼ������ӿڽṹ�������ڴ�ء�pbuf�ṹ��*/
   lwip_init();	
	  
#if LWIP_DHCP			   					//��ʹ��DHCPЭ��
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0; 
#else										//
  IP4_ADDR(&ipaddr, 192, 168, 1, 18);  		//��������ӿڵ�ip��ַ
  IP4_ADDR(&netmask, 255, 255, 255, 0);		//��������
  IP4_ADDR(&gw, 192, 168, 1, 1);			//����

#endif
   
  /*��ʼ��enc28j60��LWIP�Ľӿڣ�����Ϊ����ӿڽṹ�塢ip��ַ��
  �������롢���ء�������Ϣָ�롢��ʼ�����������뺯��*/
  netif_add(&enc28j60, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
 
  /*��enc28j60����ΪĬ������ .*/
  netif_set_default(&enc28j60);


#if LWIP_DHCP	   			//��ʹ����DHCP
  /*  Creates a new DHCP client for this interface on the first call.
  Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
  the predefined regular intervals after starting the client.
  You can peek in the netif->dhcp struct for the actual DHCP status.*/
  dhcp_start(&enc28j60);   	//����DHCP
#endif

  /*  When the netif is fully configured this function must be called.*/
  netif_set_up(&enc28j60); //ʹ��enc28j60�ӿ�
}

/*
 * ��������Time_Update
 * ����  �����ڸ���LocalTime��ֵ   	 		
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */	 
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}


/*
 * ��������LwIP_Periodic_Handle
 * ����  ��lwipЭ��ջҪ�����ڵ���һЩ����
 			tcp_tmr  etharp_tmr   dhcp_fine_tmr dhcp_coarse_tmr	 		
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void LwIP_Periodic_Handle(__IO uint32_t localtime)
{
	//err_t  err;
	
	if(localtime - INPUT_Timer >= INPUT_TMR_INTERVAL)
		{
		    /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
			ethernetif_input(&enc28j60); 			//��ѯ�Ƿ���յ�����
			//	err = ethernetif_input(&enc28j60); //��ѯ�Ƿ���յ�����

			//		if (err !=ERR_OK  ) 
			//		{
			//			
			//
			//			}
		}

	  /* TCP periodic process every 250 ms */
	  if (localtime - TCPTimer >= TCP_TMR_INTERVAL)
	  {
	    TCPTimer =  localtime;
	    tcp_tmr();		 //ÿ250ms����һ��
	  }
	  /* ARP periodic process every 5s */
	  if (localtime - ARPTimer >= ARP_TMR_INTERVAL)
	  {
	    ARPTimer =  localtime;
	    etharp_tmr();	  //ÿ5s����һ��
	  }

#if LWIP_DHCP
	  /* Fine DHCP periodic process every 500ms */
	  if (localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
	  {
	    DHCPfineTimer =  localtime;
	    dhcp_fine_tmr();
	  }
	  /* DHCP Coarse periodic process every 60s */
	  if (localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
	  {
	    DHCPcoarseTimer =  localtime;
	    dhcp_coarse_tmr();
	  }
#endif

}
