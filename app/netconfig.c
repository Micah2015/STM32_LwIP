/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：LWIP_Init.c
 * 描述    ：LWIP协议栈初始化，初始化网卡         
 * 实验平台：野火STM32开发板
 
 * 库版本  ：ST3.5.0
 * 作者    ：wildfire team  
 * 论坛    ：www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
 * 淘宝    ：http://firestm32.taobao.com
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
 	
struct netif enc28j60;	  		//netif 网络接口结构，用于描述网络硬件接口的特性
uint8_t macaddress[6]={0x54,0x55,0x58,0x10,0x00,0x24};	//设置网络接口的mac地址
extern __IO uint32_t LocalTime;

/**外部函数*/
extern err_t ethernetif_init( struct netif *netif );  //在ethernetif.c中定义
extern err_t ethernetif_input( struct netif *netif );  //在ethernetif.c中定义

/*
 * 函数名：LWIP_Init
 * 描述  ：初始化LWIP协议栈，主要是把enc28j60与LWIP连接起来。
 			包括IP、MAC地址，接口函数
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void LwIP_Init( void )
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;

   /*调用LWIP初始化函数，
   初始化网络接口结构体链表、内存池、pbuf结构体*/
   lwip_init();	
	  
#if LWIP_DHCP			   					//若使用DHCP协议
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0; 
#else										//
  IP4_ADDR(&ipaddr, 192, 168, 1, 18);  		//设置网络接口的ip地址
  IP4_ADDR(&netmask, 255, 255, 255, 0);		//子网掩码
  IP4_ADDR(&gw, 192, 168, 1, 1);			//网关

#endif
   
  /*初始化enc28j60与LWIP的接口，参数为网络接口结构体、ip地址、
  子网掩码、网关、网卡信息指针、初始化函数、输入函数*/
  netif_add(&enc28j60, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
 
  /*把enc28j60设置为默认网卡 .*/
  netif_set_default(&enc28j60);


#if LWIP_DHCP	   			//若使用了DHCP
  /*  Creates a new DHCP client for this interface on the first call.
  Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
  the predefined regular intervals after starting the client.
  You can peek in the netif->dhcp struct for the actual DHCP status.*/
  dhcp_start(&enc28j60);   	//启动DHCP
#endif

  /*  When the netif is fully configured this function must be called.*/
  netif_set_up(&enc28j60); //使能enc28j60接口
}

/*
 * 函数名：Time_Update
 * 描述  ：用于更新LocalTime的值   	 		
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */	 
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}


/*
 * 函数名：LwIP_Periodic_Handle
 * 描述  ：lwip协议栈要求周期调用一些函数
 			tcp_tmr  etharp_tmr   dhcp_fine_tmr dhcp_coarse_tmr	 		
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void LwIP_Periodic_Handle(__IO uint32_t localtime)
{
	//err_t  err;
	
	if(localtime - INPUT_Timer >= INPUT_TMR_INTERVAL)
		{
		    /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
			ethernetif_input(&enc28j60); 			//轮询是否接收到数据
			//	err = ethernetif_input(&enc28j60); //轮询是否接收到数据

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
	    tcp_tmr();		 //每250ms调用一次
	  }
	  /* ARP periodic process every 5s */
	  if (localtime - ARPTimer >= ARP_TMR_INTERVAL)
	  {
	    ARPTimer =  localtime;
	    etharp_tmr();	  //每5s调用一次
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
