/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��cmd.c
 * ����    ��telnet������غ���   
 * ʵ��ƽ̨��Ұ��STM32������
 
 * ��汾  ��ST3.5.0
 * ����    ��wildfire team 
 * ��̳    ��www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "cmd.h"
#include <stdio.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define IN_USER 	"Please enter your username:\r\n"  //�����û�����ʾ
#define USER_ERR 	"UserName wrong:\r\n"			   //�û���������ʾ
#define IN_PWD 		"Please enter your password:\r\n"  //����������ʾ
#define PWD_ERR 	"\r\nPWD wrong:\r\n"
#define IN_CMD 		"\r\nPlease enter command:\r\n"	  //����������ʾ
#define CMD_ERR		"\r\nCommand not found!"
#define STATUS_ON	"\r\n Success,LED1 status is ON!\r\n"
#define STATUS_OFF	"\r\n Success,LED1 status is OFF!\r\n"

#define MAX_NAME_SIZE 15						  //���ֵ���ֽ���

struct dat 		//���ڱ���client����������֡�
{
  int length;
  char bytes[MAX_NAME_SIZE];
};

struct r_ctrl
{
 	char user[MAX_NAME_SIZE];//�û���
	char PWD[MAX_NAME_SIZE]; //����
	char CMD[MAX_NAME_SIZE]; //����
};

struct r_ctrl r_msg;

/* Private functions ---------------------------------------------------------*/


/*
 * ��������CMD_recv
 * ����  ������client�˵�����
 * ����  ��tcp_arg���õĲ�����pcb�ṹ�壬pbuf��err�������
 * ���  : err_t
 * ����  ���ڲ�����
 */
static err_t CMD_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  struct pbuf *q;
  struct dat *dat = (struct dat *)arg;
  int done;
  char *c;
  int i;	  

  /* We perform here any necessary processing on the pbuf */
  if (p != NULL)   //�����pbuf�ǿ�
  {        
	/* We call this function to tell the LwIp that we have processed the data */
	/* This lets the stack advertise a larger window, so more data can be received*/
	tcp_recved(pcb, p->tot_len); //����pbuf����

    /* Check the name if NULL, no data passed, return withh illegal argument error */
	if(!dat) 				 //Ϊ��ʱ�ͷ��ڴ�
    {
      pbuf_free(p);
      return ERR_ARG;
    }

    done = 0;
    for(q=p; q != NULL; q = q->next) 	 //qΪpbuf��ָ��
    {
      c = q->payload;					 //c��ȡpbuf�е�����ָ��payload
      for(i=0; i<q->len && !done; i++) 
	  {
        done = ((c[i] == '\r') || (c[i] == '\n'));	//���յ��س�
        if(dat->length < MAX_NAME_SIZE) 			 //���ַ������浽�ṹ��
	    {
          dat->bytes[dat->length++] = c[i];	//����payload�е����ݵ�name�ṹ��
        }
      }
    } 


    if(done) 
    {
		/*��windows�£��س����� \r\n ��linux�£��س����� \n */

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
          dat->length = MAX_NAME_SIZE;	//�ﵽ���ֵ�����
        }

//        name->bytes[name->length-2] = '\r';	 //ת��Ϊ��׼��\r\n�����ַ���ʾ�س�����
//        name->bytes[name->length-1] = '\n';	 
	
		dat->bytes[dat->length-2] = '\0'; //�ӽ���������ȡ��bytes������
      }
		

	   	if(strcmp(r_msg.user,"wildfire")==0) //��������û���
			{
				//	 printf("\r\n r_msg.user=%s",r_msg.user);

			   if(strcmp(r_msg.PWD,"123456")==0)	//�����������
			   		{
						strcpy(r_msg.CMD,dat->bytes);	 //��������

						 if(strcmp(r_msg.CMD,"LED1_ON")==0)	//�����������
						   {
						   	 LED1(ON);
						   	 tcp_write(pcb,STATUS_ON, strlen(STATUS_ON), 1);//���Ƴɹ�	 
							 tcp_write(pcb,IN_CMD, strlen(IN_CMD), 1);	 //��ʾ������һ����
							  }
						 else if(strcmp(r_msg.CMD,"LED1_OFF")==0)	//��������ص�
						   {
						   		LED1(OFF);
							   	tcp_write(pcb,STATUS_OFF, strlen(STATUS_OFF), 1);//���Ƴɹ�	 
								tcp_write(pcb,IN_CMD, strlen(IN_CMD), 1);	  //��ʾ������һ����
						   }
						 else  //���Ҳ�������
						 	{
							tcp_write(pcb,CMD_ERR, strlen(CMD_ERR), 1);	 //�������
							tcp_write(pcb,IN_CMD, strlen(IN_CMD), 1);	 //��ʾ������һ����
							}

					 }
				else if(strcmp(dat->bytes,"123456")==0)//��һ���������룬������ȷ
				   	{
					strcpy(r_msg.PWD,dat->bytes);	 //��������
					tcp_write(pcb,IN_CMD, strlen(IN_CMD), 1);	 //��ʾ������һ����

					}
				else //�������
				{	
				tcp_write(pcb,PWD_ERR, strlen(PWD_ERR), 1);	 //�������
				}
			}
		else //��һ�������û���
			{
				if(strcmp(dat->bytes,"wildfire")==0)	//�û�����ȷ
				{
					strcpy(r_msg.user,dat->bytes);	//��ȷ�������û���
				  	tcp_write(pcb,IN_PWD, strlen(IN_PWD), 1);	 //��ʾ��������
				
				 }
				else//�û�������
					{
					tcp_write(pcb, USER_ERR, strlen(USER_ERR), 1); //�û�������
					tcp_write(pcb, IN_USER, strlen(IN_USER), 1); //��ʾ�ٴ�����
					}			 	
			} 
      dat->length = 0;	
    }
	
	/* End of processing, we free the pbuf */
    pbuf_free(p);	  //�ͷ�pbuf
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
 * ��������CMD_accept
 * ����  ��telnet���ӽ�������õĻص�����
 * ����  ���û��Զ���������������ӵ�pcb���ƿ顢errorֵ
 * ���  : ERROK
 * ����  ���ⲿ����
 */  
static err_t CMD_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{     
   /*��ָ���Ĳ�������mem_calloc�����һ���ռ�ָ�룩���ݸ��������Ļص�����*/
  /* Tell LwIP to associate this structure with this connection. */
  tcp_arg(pcb, mem_calloc(sizeof(struct dat), 1));	  
  
  /* Configure LwIP to use our call back functions. */
  tcp_err(pcb, CMD_conn_err);   //����ʱ���õĻص�����HelloWorld_conn_err
  tcp_recv(pcb, CMD_recv);	   //���ڽ���client�����ݵĻص�����HelloWorld_recv
  
  /* Send out the first message */  
  tcp_write(pcb, IN_USER, strlen(IN_USER), 1); //���ͻ�ӭ�ַ���

  return ERR_OK;
}

/*
 * ��������CMD_init
 * ����  ����ʼ��telnet�õ�Զ�̿���С����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void CMD_init(void)
{
  struct tcp_pcb *pcb;		//����һ��tcp���ƿ�            		
  
  /* Create a new TCP control block  */
  pcb = tcp_new();	   		//��tcp���ƿ�����ڴ�ռ�             		 	

  /* Assign to the new pcb a local IP address and a port number */
  /* Using IP_ADDR_ANY allow the pcb to be used by any local interface */
  tcp_bind(pcb, IP_ADDR_ANY, 23);     //��PCB���ƿ�󶨵�����������IP��ַ���˿�Ϊ23  


  /* Set the connection to the LISTEN state */
  pcb = tcp_listen(pcb);			  //�����ö˿�	

  /* Specify the function to be called when a connection is established */	
  tcp_accept(pcb, CMD_accept);   	 //�����Ķ˿ڽ�ͨ����õĺ���HelloWorld_accept
										
}


  /*
 * ��������CMD_conn_err
 * ����  ����������ʱ���ñ�����
 * ����  ��tcp_arg�����룬�������err
 * ���  : ��
 * ����  ���ⲿ����
 */
static void CMD_conn_err(void *arg, err_t err)
{
  struct dat *dat;
  dat = (struct dat *)arg;

  mem_free(dat);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/


