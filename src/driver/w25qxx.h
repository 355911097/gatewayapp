
#ifndef __W25QXX_H_
#define __W25QXX_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "stm32f2xx.h"
#include "stm32f2xx_spi.h"



/*
*********************************************************************************************************
*                                             DEFINES
*********************************************************************************************************
*/
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17


/*
*********************************************************************************************************
*                                           ORDER LIST DEFINES
*********************************************************************************************************
*/
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 





/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/



void w25qxx_init(void);
u8 w25qxx_read_SR(void);
void w25qxx_write_enable(void);
void w25qxx_write_disable(void);
void w25qxx_wait_busy(void);
u16 w25qxx_read_id(void);
void w25qxx_read(u8 *buff, u32 addr, u16 size);
void w25qxx_write_page(u8 *buff, u32 addr, u16 size);
void w25qxx_write_no_check(u8 *buff, u32 addr, u16 size);
void w25qxx_write(u8 *buff, u32 addr, u16 size);
void w25qxx_erase_chip(void);
void w25qxx_erase_sector(u32 addr);









void SPI_Flash_Init(void);
u16  SPI_Flash_ReadID(void);  	    			//¶ÁÈ¡FLASH ID
u8	 SPI_Flash_ReadSR(void);        			//¶ÁÈ¡×´Ì¬¼Ä´æÆ÷ 
void SPI_FLASH_Write_SR(u8 sr);  				//Ð´×´Ì¬¼Ä´æÆ÷
void SPI_FLASH_Write_Enable(void);  			//Ð´Ê¹ÄÜ 
void SPI_FLASH_Write_Disable(void);				//Ð´±£»¤
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   		//¶ÁÈ¡flash
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);		//Ð´Èëflash
void SPI_Flash_Erase_Chip(void);    	  		//ÕûÆ¬²Á³ý
void SPI_Flash_Erase_Sector(u32 Dst_Addr);		//ÉÈÇø²Á³ý
void SPI_Flash_Wait_Busy(void);           		//µÈ´ý¿ÕÏÐ
void SPI_Flash_PowerDown(void);           		//½øÈëµôµçÄ£Ê½
void SPI_Flash_WAKEUP(void);			  		//»½ÐÑ

#endif

