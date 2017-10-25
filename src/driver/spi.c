




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
	
	
	spi_init_structure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//ͨ�ŷ�ʽ��ȫ˫������˫�������з������գ�
	spi_init_structure.SPI_Mode = SPI_Mode_Master;		//ģʽѡ���������Ǵӻ�
	spi_init_structure.SPI_DataSize = SPI_DataSize_8b;	//����֡���ȣ�8λ��16λ
	spi_init_structure.SPI_CPOL = SPI_CPOL_High;			//ʱ�Ӽ���
	spi_init_structure.SPI_CPHA = SPI_CPHA_2Edge;		//ʱ����λ������ʱ�ӵ�һ�������ز������ݣ���ڶ���������~~SPI_CPHA_2Edge
	spi_init_structure.SPI_NSS = SPI_NSS_Soft;			// NSS�ź���Ӳ�����ƻ����������SPI_NSS_Soft
	spi_init_structure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//����SPI������Ԥ��Ƶֵ��Ҳ���Ǿ���SPI��ʱ�ӵĲ�����36M/256=140.625KHz
	spi_init_structure.SPI_FirstBit = SPI_FirstBit_MSB;	//���ݴ���˳��MSB��ǰ����LSB��ǰ
	spi_init_structure.SPI_CRCPolynomial = 7;			//��������CRCУ�����ʽ�����ͨ�ſɿ��� ����1����
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











