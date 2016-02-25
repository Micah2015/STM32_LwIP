/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：SPI.c
 * 描述    ：初始化enc28j60使用到的SPI1，及底层收发数据函数       
 * 实验平台：野火STM32开发板
 * 硬件连接： ------------------------------------
 *           |PB13         ：ENC28J60-INT (没用到)|
 *           |PA6-SPI1-MISO：ENC28J60-SO          |
 *           |PA7-SPI1-MOSI：ENC28J60-SI          |
 *           |PA5-SPI1-SCK ：ENC28J60-SCK         |
 *           |PA4-SPI1-NSS ：ENC28J60-CS          |
 *           |PE1          ：ENC28J60-RST (没用到)  |
 *            ------------------------------------
 * 库版本  ：ST3.5.0
 * 作者    ：wildfire team 
 * 论坛    ：www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/


#include "SPI.h"

/*
 * 函数名：SPI1_Init
 * 描述  ：初始化SPI1端口及基模式
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void SPI1_Init(void)
{
   SPI_InitTypeDef SPI_InitStructure;
   GPIO_InitTypeDef GPIO_InitStructure;

   /* Enable SPI1 and GPIOA clocks */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;		//CS
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);  

   /* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure); 

   /* SPI1 configuration */ 
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 7;
   SPI_Init(SPI1, &SPI_InitStructure);

   /* Enable SPI1  */
   SPI_Cmd(SPI1, ENABLE);
}

/*
 * 函数名：SPI1_ReadWrite
 * 描述  ：最底层的使用SPI1收发一节字数据
 * 输入  ：要写入的数据
 * 输出  : 接收到的数据
 * 调用  ：外部调用
 */
uint8_t SPI1_ReadWrite(uint8_t dat)
{
   /* Loop while DR register in not emplty */
   while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

   /* Send byte through the SPI1 peripheral */
   SPI_I2S_SendData(SPI1, dat);

   /* Wait to receive a byte */
   while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

   /* Return the byte read from the SPI bus */
   return SPI_I2S_ReceiveData(SPI1);
}


