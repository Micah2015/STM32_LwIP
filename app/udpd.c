#include "udpd.h"

const static char UDPData[]="Test! Send Udp packet. A Client program to link PC server.\r\n"; 

void  Delay1(unsigned long  ulVal)  /* 利用循环产生一定的延时 */ 
{ 
    while ( --ulVal  !=  0 ); 
} 

/*
 * 函数名：updd_Send
 * 描述  ：发送udp数据包
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void udpd_Send(void)
{
	struct udp_pcb *UdpPcb; 
  struct ip_addr ipaddr; 
  struct pbuf *p;
	err_t err = ERR_Mine;	//测试用
	
	p = pbuf_alloc(PBUF_RAW,sizeof(UDPData),PBUF_RAM); 
  p->payload=(void *)UDPData; 
   
	UdpPcb = udp_new(); 
	udp_bind(UdpPcb,IP_ADDR_ANY,6000);       /* 绑定本地IP 地址 */ 
  IP4_ADDR(&ipaddr,192,168,1,122); 
  
  err = udp_connect(UdpPcb,&ipaddr,6001);   /* 连接远程主机 */ 	

	if(err == ERR_OK)
	{
		err = ERR_Mine;	//测试用
		err = udp_send(UdpPcb, p);
	}
	
	udp_disconnect(UdpPcb);
	pbuf_free(p);
	udp_remove(UdpPcb);
}
