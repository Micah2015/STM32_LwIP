#ifndef __UDPD_H
#define __UDPD_H

#include "stm32f10x.h"
#include "udp.h"

#define ERR_Mine -15

void udpd_Send(void);
void Delay1(unsigned long  ulVal);

#endif /* __UDPD_H */
