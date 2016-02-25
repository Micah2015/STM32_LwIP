#ifndef __TCPD_H
#define __TCPD_H

//#ifdef __cplusplus	//cplusplus是cpp中自定义的一个宏
// extern "C" {				//告诉编译器这部分代码按c语言格式编译，而不是c++
//#endif

#include "stm32f10x.h"
#include "tcp.h"

#include "led.h"

#include <string.h>
	 
void tcpd_init(void);
static err_t tcpd_accept(void *arg,struct tcp_pcb *pcb,err_t err);
static err_t tcpd_recv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err);

//#ifdef __cplusplus
//}
//#endif

#endif	/*__TCPD_H*/
