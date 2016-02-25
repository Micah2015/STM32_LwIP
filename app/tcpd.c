#include "tcpd.h"

const static uint8_t test[]="I have receive the command! LED1 is on!\r\n";
const static uint8_t test2[]="I have receive the command! LED1 is off!\r\n";

/*
 * 函数名：tcpd_init
 * 描述  ：初始化tcp连接
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void tcpd_init(void)
{
	struct tcp_pcb *pcb;		//定义一个tcp控制块            		
  
  /* Create a new TCP control block  */
  pcb = tcp_new();	   		//给tcp控制块分配内存空间             		 	

  /* Assign to the new pcb a local IP address and a port number */
  /* Using IP_ADDR_ANY allow the pcb to be used by any local interface */
  tcp_bind(pcb, IP_ADDR_ANY, 1200);     //把PCB控制块绑定到本机的所有IP地址，端口为1200

  /* Set the connection to the LISTEN state */
  pcb = tcp_listen(pcb);			  //监听该端口	

  /* Specify the function to be called when a connection is established */	
  tcp_accept(pcb, tcpd_accept);   	 //监听的端口接通后调用的函数tcpd_accept
}

/*
 * 函数名：tcp_accept
 * 描述  ：tcp连接后的回调函数，建立连接后调用
 * 输入  ：tcp_arg设置的参数、pcb、err
 * 输出  ：err
 * 调用  ：内部调用 
 */
static err_t tcpd_accept(void *arg,struct tcp_pcb *pcb,err_t err)
{
  tcp_setprio(pcb, TCP_PRIO_MIN);     /* 设置回调函数优先级，当存在几个连接时特别重要，此函数必须调用*/
								
  tcp_recv(pcb,tcpd_recv);            /* 设置TCP接收到数据后的回调函数 */
     
  return ERR_OK;
}

/*
 * 函数名：tcpd_recv
 * 描述  ：tcp接收到数据后的回调函数
 * 输入  ：tcp_arg设置的参数、tcp_pcb、pcb、err
 * 输出  ：err
 * 调用  ：内部调用 
 */
static err_t tcpd_recv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err)
{
	char * data = NULL;
	data = p->payload;	//把接收到的数据指针交给data
	
	if (err == ERR_OK && p != NULL)	//判断是否非空
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
		pbuf_free(p);  /* 释放该pbuf段 */
	}
	//tcp_close(pcb);  /* 关闭这个连接 */
  err = ERR_OK;
  
  return err;
}	
