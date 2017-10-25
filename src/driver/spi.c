




/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "spi.h"








/*
*********************************************************************************************************
*                                           bsp_spi1_init()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  bsp_spi1_init(void)
{

	SPI_InitTypeDef spi_init_structure;
	
	
	spi_init_structure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//通信方式（全双工、半双工、串行发串行收）
	spi_init_structure.SPI_Mode = SPI_Mode_Master;		//模式选择，主机还是从机
	spi_init_structure.SPI_DataSize = SPI_DataSize_8b;	//数据帧长度，8位或16位
	spi_init_structure.SPI_CPOL = SPI_CPOL_High;			//时钟极行
	spi_init_structure.SPI_CPHA = SPI_CPHA_2Edge;		//时钟相位，串行时钟第一个跳变沿采样数据，或第二个跳变沿~~SPI_CPHA_2Edge
	spi_init_structure.SPI_NSS = SPI_NSS_Soft;			// NSS信号由硬件控制还是软件控制SPI_NSS_Soft
	spi_init_structure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//设置SPI波特率预分频值，也就是决定SPI的时钟的参数，36M/256=140.625KHz
	spi_init_structure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输顺序，MSB在前还是LSB在前
	spi_init_structure.SPI_CRCPolynomial = 7;			//用来设置CRC校验多项式，提高通信可靠性 大于1即可
	SPI_Init(SPI1, &spi_init_structure);
	
	SPI_Cmd(SPI1, ENABLE);
	
}

/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 spi1_exchange_byte(u8 ch)
{

		/* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, ch);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);	
}











