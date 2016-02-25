/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：cmd.c
 * 描述    ：telnet程序相关函数   
 * 实验平台：野火STM32开发板
 
 * 库版本  ：ST3.5.0
 * 作者    ：wildfire team 
 * 论坛    ：www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "cmd.h"
#include <stdio.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define IN_USER 	"Please enter your username:\r\n"  //输入用户名提示
#define USER_ERR 	"UserName wrong:\r\n"			   //用户名错误提示
#define IN_PWD 		"Please enter your password:\r\n"  //输入密码提示
#define PWD_ERR 	"\r\nPWD wrong:\r\n"
#define IN_CMD 		"\r\nPlease enter command:\r\n"	  //输入命令提示
#define CMD_ERR		"\r\nCommand not found!"
#define STATUS_ON	"\r\n Success,LED1 status is ON!\r\n"
#define STATUS_OFF	"\r\n Success,LED1 status is OFF!\r\n"

#define MAX_NAME_SIZE 15						  //名字的最长字节数

struct dat 		//用于保存client端输入的名字。
{
  int length;
  char bytes[MAX_NAME_SIZE];
};

struct r_ctrl
{
 	char user[MAX_NAME_SIZE];//用户名
	char PWD[MAX_NAME_SIZE]; //密码
	char CMD[MAX_NAME_SIZE]; //命令
};

struct r_ctrl r_msg;

/* Private functions ---------------------------------------------------------*/


/*
 * 函数名：CMD_recv
 * 描述  ：接收client端的数据
 * 输入  ：tcp_arg设置的参数，pcb结构体，pbuf，err错误变量
 * 输出  : err_t
 * 调用  ：内部调用
 */
static err_t CMD_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  struct pbuf *q;
  struct dat *dat = (struct dat *)arg;
  int done;
  char *c;
  int i;	  

  /* We perform here any necessary processing on the pbuf */
  if (p != NULL)   //传入的pbuf非空
  {        
	/* We call this function to tell the LwIp that we have processed the data */
	/* This lets the stack advertise a larger window, so more data can be received*/
	tcp_recved(pcb, p->tot_len); //接收pbuf数据

    /* Check the name if NULL, no data passed, return withh illegal argument error */
	if(!dat) 				 //为空时释放内存
    {
      pbuf_free(p);
      return ERR_ARG;
    }

    done = 0;
    for(q=p; q != NULL; q = q->next) 	 //q为pbuf的指针
    {
      c = q->payload;					 //c获取pbuf中的数据指针payload
      for(i=0; i<q->len && !done; i++) 
	  {
        done = ((c[i] == '\r') || (c[i] == '\n'));	//接收到回车
        if(dat->length < MAX_NAME_SIZE) 			 //把字符串保存到结构体
	    {
          dat->bytes[dat->length++] = c[i];	//保存payload中的数据到name结构体
        }
      }
    } 


    if(done) 
    {
		/*在windows下，回车等于 \r\n 在linux下，回车等于 \n */

      if(dat->bytes[dat->length-2] != '\r' || dat->bytes[dat->length-1] != '\n') 
	  {
        if((dat->bytes[dat->length-1] == '\r' || dat->bytes[dat->length-1] == '\n') && (dat->length+1 <= MAX_NAME_SIZE)) 
	    {
	      dat->length += 1; //linux 
        } 
	    else if(dat->length+2 <= MAX_NAME_SIZE) 
	    {
          dat->length += 2; //windows
        } 
	    else 
	    {
          dat->length = MAX_NAME_SIZE;	//达到最大值的情况
        }

//        name->bytes[name->length-2] = '\r';	 //转化为标准的\r\n两个字符表示回车结束
//        name->bytes[name->length-1] = '\n';	 
	
		dat->bytes[dat->length-2] = '\0'; //加结束符，提取出bytes的数据
      }
		

	   	if(strcmp(r_msg.user,"wildfire")==0) //已输入过用户名
			{
				//	 printf("\r\n r_msg.user=%s",r_msg.user);

			   if(strcmp(r_msg.PWD,"123456")==0)	//已输入过密码
			   		{
						strcpy(r_msg.CMD,dat->bytes);	 //复制命令

						 if(strcmp(r_msg.CMD,"LED1_ON")==0)	//分析命令，开灯
						   {
						   	 LED1(ON);
						   	 tcp_write(pcb,STATUS_ON, strlen(STATUS_ON), 1);//控制成功	 
							 tcp_write(pcb,IN_CMD, strlen(IN_CMD), 1);	 //提示输入下一命令
							  }
						 else if(strcmp(r_msg.CMD,"LED1_OFF")==0)	//分析命令，关灯
						   {
						   		LED1(OFF);
							   	tcp_write(pcb,STATUS_OFF, strlen(STATUS_OFF), 1);//控制成功	 
								tcp_write(pcb,IN_CMD, strlen(IN_CMD), 1);	  //提示输入下一命令
						   }
						 else  //查找不到命令
						 	{
							tcp_write(pcb,CMD_ERR, strlen(CMD_ERR), 1);	 //命令错误
							tcp_write(pcb,IN_CMD, strlen(IN_CMD), 1);	 //提示输入下一命令
							}

					 }
				else if(strcmp(dat->bytes,"123456")==0)//第一次输入密码，密码正确
				   	{
					strcpy(r_msg.PWD,dat->bytes);	 //复制密码
					tcp_write(pcb,IN_CMD, strlen(IN_CMD), 1);	 //提示输入下一命令

					}
				else //密码错误
				{	
				tcp_write(pcb,PWD_ERR, strlen(PWD_ERR), 1);	 //密码错误
				}
			}
		else //第一次输入用户名
			{
				if(strcmp(dat->bytes,"wildfire")==0)	//用户名正确
				{
					strcpy(r_msg.user,dat->bytes);	//正确，复制用户名
				  	tcp_write(pcb,IN_PWD, strlen(IN_PWD), 1);	 //提示输入密码
				
				 }
				else//用户名错误
					{
					tcp_write(pcb, USER_ERR, strlen(USER_ERR), 1); //用户名错误
					tcp_write(pcb, IN_USER, strlen(IN_USER), 1); //提示再次输入
					}			 	
			} 
      dat->length = 0;	
    }
	
	/* End of processing, we free the pbuf */
    pbuf_free(p);	  //释放pbuf
  }  
  else if (err == ERR_OK) 
  {
    /* When the pbuf is NULL and the err is ERR_OK, the remote end is closing the connection. */
    /* We free the allocated memory and we close the connection */
    mem_free(dat);
    return tcp_close(pcb);
  }	    

  return ERR_OK; 

}  




/*
 * 函数名：CMD_accept
 * 描述  ：telnet连接建立后调用的回调函数
 * 输入  ：用户自定义参数、建立连接的pcb控制块、error值
 * 输出  : ERROK
 * 调用  ：外部调用
 */  
static err_t CMD_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{     
   /*把指定的参数（用mem_calloc分配的一个空间指针）传递给接下来的回调函数*/
  /* Tell LwIP to associate this structure with this connection. */
  tcp_arg(pcb, mem_calloc(sizeof(struct dat), 1));	  
  
  /* Configure LwIP to use our call back functions. */
  tcp_err(pcb, CMD_conn_err);   //错误时调用的回调函数HelloWorld_conn_err
  tcp_recv(pcb, CMD_recv);	   //用于接收client端数据的回调函数HelloWorld_recv
  
  /* Send out the first message */  
  tcp_write(pcb, IN_USER, strlen(IN_USER), 1); //发送欢迎字符串

  return ERR_OK;
}

/*
 * 函数名：CMD_init
 * 描述  ：初始化telnet用的远程控制小程序
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void CMD_init(void)
{
  struct tcp_pcb *pcb;		//定义一个tcp控制块            		
  
  /* Create a new TCP control block  */
  pcb = tcp_new();	   		//给tcp控制块分配内存空间             		 	

  /* Assign to the new pcb a local IP address and a port number */
  /* Using IP_ADDR_ANY allow the pcb to be used by any local interface */
  tcp_bind(pcb, IP_ADDR_ANY, 23);     //把PCB控制块绑定到本机的所有IP地址，端口为23  


  /* Set the connection to the LISTEN state */
  pcb = tcp_listen(pcb);			  //监听该端口	

  /* Specify the function to be called when a connection is established */	
  tcp_accept(pcb, CMD_accept);   	 //监听的端口接通后调用的函数HelloWorld_accept
										
}


  /*
 * 函数名：CMD_conn_err
 * 描述  ：产生错误时调用本函数
 * 输入  ：tcp_arg的输入，错误变量err
 * 输出  : 无
 * 调用  ：外部调用
 */
static void CMD_conn_err(void *arg, err_t err)
{
  struct dat *dat;
  dat = (struct dat *)arg;

  mem_free(dat);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/


