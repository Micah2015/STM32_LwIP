#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

#define  ENC_SPI_ReadWrite  SPI1_ReadWrite
#define  ENC_SPI_Init   SPI1_Init

void SPI1_Init(void);
uint8_t SPI1_ReadWrite(uint8_t writedat);
   

#endif
