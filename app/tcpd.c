#include "tcpd.h"

const static uint8_t test[]="I have receive the command! LED1 is on!\r\n";
const static uint8_t test2[]="I have receive the command! LED1 is off!\r\n";

/*
 * ��������tcpd_init
 * ����  ����ʼ��tcp����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void tcpd_init(void)
{
	struct tcp_pcb *pcb;		//����һ��tcp���ƿ�            		
  
  /* Create a new TCP control block  */
  pcb = tcp_new();	   		//��tcp���ƿ�����ڴ�ռ�             		 	

  /* Assign to the new pcb a local IP address and a port number */
  /* Using IP_ADDR_ANY allow the pcb to be used by any local interface */
  tcp_bind(pcb, IP_ADDR_ANY, 1200);     //��PCB���ƿ�󶨵�����������IP��ַ���˿�Ϊ1200

  /* Set the connection to the LISTEN state */
  pcb = tcp_listen(pcb);			  //�����ö˿�	

  /* Specify the function to be called when a connection is established */	
  tcp_accept(pcb, tcpd_accept);   	 //�����Ķ˿ڽ�ͨ����õĺ���tcpd_accept
}

/*
 * ��������tcp_accept
 * ����  ��tcp���Ӻ�Ļص��������������Ӻ����
 * ����  ��tcp_arg���õĲ�����pcb��err
 * ���  ��err
 * ����  ���ڲ����� 
 */
static err_t tcpd_accept(void *arg,struct tcp_pcb *pcb,err_t err)
{
  tcp_setprio(pcb, TCP_PRIO_MIN);     /* ���ûص��������ȼ��������ڼ�������ʱ�ر���Ҫ���˺����������*/
								
  tcp_recv(pcb,tcpd_recv);            /* ����TCP���յ����ݺ�Ļص����� */
     
  return ERR_OK;
}

/*
 * ��������tcpd_recv
 * ����  ��tcp���յ����ݺ�Ļص�����
 * ����  ��tcp_arg���õĲ�����tcp_pcb��pcb��err
 * ���  ��err
 * ����  ���ڲ����� 
 */
static err_t tcpd_recv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err)
{
	char * data = NULL;
	data = p->payload;	//�ѽ��յ�������ָ�뽻��data
	
	if (err == ERR_OK && p != NULL)	//�ж��Ƿ�ǿ�
	{
		if(strncmp(data,"abcdefghijklmnopqrstuvwxyz",26)==0) 
		{
			LED1(ON);
			tcp_write(pcb,test,sizeof(test),0);
		}
		if(strncmp(data,"abcdefghijklmnopqrstuvwxya",26)==0) 
		{
			LED1(OFF);
			tcp_write(pcb,test2,sizeof(test),0);
		}
		pbuf_free(p);  /* �ͷŸ�pbuf�� */
	}
	//tcp_close(pcb);  /* �ر�������� */
  err = ERR_OK;
  
  return err;
}	
