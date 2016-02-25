#include "udpd.h"

const static char UDPData[]="Test! Send Udp packet. A Client program to link PC server.\r\n"; 

void  Delay1(unsigned long  ulVal)  /* ����ѭ������һ������ʱ */ 
{ 
    while ( --ulVal  !=  0 ); 
} 

/*
 * ��������updd_Send
 * ����  ������udp���ݰ�
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void udpd_Send(void)
{
	struct udp_pcb *UdpPcb; 
  struct ip_addr ipaddr; 
  struct pbuf *p;
	err_t err = ERR_Mine;	//������
	
	p = pbuf_alloc(PBUF_RAW,sizeof(UDPData),PBUF_RAM); 
  p->payload=(void *)UDPData; 
   
	UdpPcb = udp_new(); 
	udp_bind(UdpPcb,IP_ADDR_ANY,6000);       /* �󶨱���IP ��ַ */ 
  IP4_ADDR(&ipaddr,192,168,1,122); 
  
  err = udp_connect(UdpPcb,&ipaddr,6001);   /* ����Զ������ */ 	

	if(err == ERR_OK)
	{
		err = ERR_Mine;	//������
		err = udp_send(UdpPcb, p);
	}
	
	udp_disconnect(UdpPcb);
	pbuf_free(p);
	udp_remove(UdpPcb);
}
