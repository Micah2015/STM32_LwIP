/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��main.c
 * ����    ����3.5.0�汾���Ĺ���ģ�塣         
 * ʵ��ƽ̨��Ұ��STM32������
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
 * �Ա�    ��http://firestm32.taobao.com
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
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  : ��
 * ����  ��
 */
int main(void)
{ 	
	/*��ʼ������*/
  USART1_Config();  	

	/*��ʼ�� ��̫��SPI�ӿ�*/
	ENC_SPI_Init(); 		

	/*��ʼ��systick�����ڶ�ʱ��ѯ������LWIP�ṩ��ʱ*/
	SysTick_Init();		

	/*��ʼ��LEDʹ�õĶ˿�*/
	LED_GPIO_Config();
	 
	printf("\r\n**************Ұ��STM32_enc8j60+lwip��ֲʵ��*************\r\n");
	  
  	/* ��ʼ��LWIPЭ��ջ*/
	LwIP_Init(); 

	/*��ʼ��web server ��ʾ��ҳ����*/
	//httpd_init();
  
	/* ��ʼ��telnet   Զ�̿��� ���� */   
	//CMD_init();    
	
//	/* ��ʼ��tcp���� */   
//	tcpd_init();	//tcp������
	
	/* Infinite loop */
	while ( 1 )
	{	
//		udpd_Send();	//udp�ͻ���
//		Delay1(20000000UL); //Լ2s
		/*��ѯ*/  
		LwIP_Periodic_Handle(LocalTime);	
	}
}






