/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：用3.5.0版本建的工程模板。         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"

#include "err.h"
#include "etharp.h"
	
#include "netconfig.h"
#include "usart1.h"
#include "SysTick.h" 
#include "spi.h"
#include "led.h"

#include "cmd.h"
#include "httpd.h"
#include "tcpd.h"
#include "udpd.h"

/*****************************************************************************************************************************/

__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
	
/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  : 无
 * 调用  ：
 */
int main(void)
{ 	
	/*初始化串口*/
  USART1_Config();  	

	/*初始化 以太网SPI接口*/
	ENC_SPI_Init(); 		

	/*初始化systick，用于定时轮询输入或给LWIP提供定时*/
	SysTick_Init();		

	/*初始化LED使用的端口*/
	LED_GPIO_Config();
	 
	printf("\r\n**************野火STM32_enc8j60+lwip移植实验*************\r\n");
	  
  	/* 初始化LWIP协议栈*/
	LwIP_Init(); 

	/*初始化web server 显示网页程序*/
	//httpd_init();
  
	/* 初始化telnet   远程控制 程序 */   
	//CMD_init();    
	
//	/* 初始化tcp连接 */   
//	tcpd_init();	//tcp服务器
	
	/* Infinite loop */
	while ( 1 )
	{	
//		udpd_Send();	//udp客户端
//		Delay1(20000000UL); //约2s
		/*轮询*/  
		LwIP_Periodic_Handle(LocalTime);	
	}
}






