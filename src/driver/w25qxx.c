



/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include "bsp.h"
#include "spi.h"
#include "w25qxx.h"
#include "usart.h"


/*
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*/
u16 w25qxx_type = W25Q32;
u8 w25qxx_flash_buf[4096]; 



/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/





/*
*********************************************************************************************************
*                                           w25qxx_init()
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
void w25qxx_init(void)
{
	u16 id = 0;
	
	SPI1_SCN_H();
	bsp_spi1_init();
	spi1_exchange_byte(0xFF);
	id = w25qxx_read_id();	
	USART_OUT(USART3, "w25qxxID = %d\r", id);
}




/*
*********************************************************************************************************
*                                           w25qxx_read_SR()
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
u8 w25qxx_read_SR(void)
{
	u8 byte = 0;
	
	SPI1_SCN_L();
	
	spi1_exchange_byte(W25X_ReadStatusReg);
	byte = spi1_exchange_byte(0xFF);
	
	SPI1_SCN_H();
	
	return byte;
}

/*
*********************************************************************************************************
*                                           w25qxx_write_enable()
*
* Description : W25XX 使能写入
*
* Argument(s) : none
*
* Return(s)   : none.
*
* Caller(s)   : w25qxx_write_page
*
* Note(s)     : none.
*********************************************************************************************************
*/
void w25qxx_write_enable(void)
{
	SPI1_SCN_L();
	spi1_exchange_byte(W25X_WriteEnable);
	SPI1_SCN_H();
}

/*
*********************************************************************************************************
*                                           w25qxx_write_disable()
*
* Description : W25QXX 不使能写入
*
* Argument(s) : none:
*
* Return(s)   : none.
*
* Caller(s)   : w25qxx_write_page.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void w25qxx_write_disable(void)
{
	SPI1_SCN_L();
	spi1_exchange_byte(W25X_WriteDisable);
	SPI1_SCN_H();

}

/*
*********************************************************************************************************
*                                           w25qxx_wait_busy()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void w25qxx_wait_busy(void)
{
	while((w25qxx_read_SR()&0x01)==0x01);   // 等待BUSY位清空
}


/*
*********************************************************************************************************
*                                           w25qxx_read_id()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : 
*				0XEF13,表示芯片型号为W25Q80 
*				0XEF14,表示芯片型号为W25Q16 
*				0XEF15,表示芯片型号为W25Q32 
*				0XEF16,表示芯片型号为W25Q64
*				0XEF17,表示芯片型号为W25Q128
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
u16 w25qxx_read_id(void)
{

	u16 ret = 0;
	SPI1_SCN_L();
	spi1_exchange_byte(0x90);
	spi1_exchange_byte(0x00);
	spi1_exchange_byte(0x00);
	spi1_exchange_byte(0x00);
	ret |= spi1_exchange_byte(0xFF)<<8;
	ret |= spi1_exchange_byte(0xFF);
	SPI1_SCN_H();

	return ret;
}


/*
*********************************************************************************************************
*                                           w25qxx_read()
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
void w25qxx_read(u8 *buff, u32 addr, u16 size)
{
	u16 i = 0;

	
	SPI1_SCN_L();                            //使能器件   
    spi1_exchange_byte(W25X_ReadData);         //发送读取命令   
    spi1_exchange_byte((u8)((addr)>>16));  //发送24bit地址    
    spi1_exchange_byte((u8)((addr)>>8));   
    spi1_exchange_byte((u8)addr);   
    for(i=0;i<size;i++)
	  { 
        buff[i]=spi1_exchange_byte(0XFF);   //循环读数  
    }
	SPI1_SCN_H(); 
	
}
/*
*********************************************************************************************************
*                                           w25qxx_write_page()
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
void w25qxx_write_page(u8 *buff, u32 addr, u16 size)
{
	u16 i = 0;
		
	w25qxx_write_enable();						//SET WEL 
	SPI1_SCN_L();                         	 	//使能器件   
    spi1_exchange_byte(W25X_PageProgram);      //发送写页命令   
    spi1_exchange_byte((u8)((addr)>>16)); 		//发送24bit地址    
    spi1_exchange_byte((u8)((addr)>>8));   
    spi1_exchange_byte((u8)addr);   
    for(i=0;i<size;i++)spi1_exchange_byte(buff[i]);//循环写数  
	SPI1_SCN_H();                            //取消片选 
	w25qxx_wait_busy();					   	//等待写入结束
}


/*
*********************************************************************************************************
*                                           w25qxx_write_no_check()
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
void w25qxx_write_no_check(u8 *buff, u32 addr, u16 size)
{
	u16 pagera_main = 0;
		
	pagera_main=256-addr%256; //单页剩余的字节数		 	    
	if(size<=pagera_main)pagera_main=size;//不大于256个字节
	while(1)
	{	   
		w25qxx_write_page(buff,addr,pagera_main);
		if(size==pagera_main)break;//写入结束了
	 	else //size>pagera_main
		{
			buff+=pagera_main;
			addr+=pagera_main;	

			size-=pagera_main;			  //减去已经写入了的字节数
			if(size>256)pagera_main=256; //一次可以写入256个字节
			else pagera_main=size; 	  //不够256个字节了
		}
	};
}

/*
*********************************************************************************************************
*                                           w25qxx_write()
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

void w25qxx_write(u8 *buff, u32 addr, u16 size)
{
	u32 secpos = 0;
	u16 secoff = 0;
	u16 secremain = 0;	   
 	u16 i = 0;
	u8 *w25qxx_buf;	
	
//	w25qxx_buf = (u8*)malloc(sizeof(u8));
   	w25qxx_buf = w25qxx_flash_buf;	     
 	secpos = addr/4096;//扇区地址  
	secoff = addr%4096;//在扇区内的偏移
	secremain = 4096-secoff;//扇区剩余空间大小   

 	if(size <= secremain) secremain=size;//不大于4096个字节
	while(1) 
	{	
		w25qxx_read(w25qxx_buf,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(w25qxx_buf[secoff+i]!=0XFF)break;		//需要擦除  	  
		}
		if(i<secremain)															//需要擦除
		{
			w25qxx_erase_sector(secpos);						//擦除这个扇区
			for(i=0;i<secremain;i++)	   							//复制
			{
				w25qxx_buf[i+secoff]=buff[i];	  
			}
			w25qxx_write_no_check(w25qxx_buf,secpos*4096,4096);	//写入整个扇区  

		}else w25qxx_write_no_check(buff,addr,secremain);	//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(size==secremain)break;					//写入结束了
		else																				//写入未结束
		{
			secpos++;																	//扇区地址增1
			secoff = 0;																	//偏移位置为0 	 

		   	buff += secremain;  										//指针偏移
			addr += secremain;											//写地址偏移	   
		   	size -= secremain;							//字节数递减
			if(size>4096) secremain=4096;				//下一个扇区还是写不完
			else secremain=size;						//下一个扇区可以写完了
		}	 
	};	 
}


/*
*********************************************************************************************************
*                                           w25qxx_erase_chip()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void w25qxx_erase_chip(void)
{

	w25qxx_write_enable();
	w25qxx_wait_busy();
	SPI1_SCN_L();
	spi1_exchange_byte(W25X_ChipErase);
	SPI1_SCN_H();
	w25qxx_wait_busy();
}
	
	



/*
*********************************************************************************************************
*                                           w25qxx_erase_sector()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : addr	
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/	
void w25qxx_erase_sector(u32 addr)	
{
	
	addr *= 4096;
	w25qxx_write_enable();
	w25qxx_wait_busy();
	SPI1_SCN_L();                           			//使能器件   
    spi1_exchange_byte(W25X_SectorErase);      	//发送扇区擦除指令 
    spi1_exchange_byte((u8)((addr)>>16));  	//发送24bit地址    
    spi1_exchange_byte((u8)((addr)>>8));   
    spi1_exchange_byte((u8)addr);  
	SPI1_SCN_H();
	w25qxx_wait_busy();	 				   					//等待擦除完成
}
	





/*
//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI1_SCN_L();;                            //使能器件   
	spi1_exchange_byte(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=spi1_exchange_byte(0Xff);             //读取一个字节  
	SPI1_SCN_H();                            //取消片选     
	return byte;   
} 

//写SPI_FLASH状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI1_SCN_L();;                            //使能器件   
	spi1_exchange_byte(W25X_WriteStatusReg);   //发送写取状态寄存器命令    
	spi1_exchange_byte(sr);               //写入一个字节  
	SPI1_SCN_H();                            //取消片选     	      
}

//SPI_FLASH写使能	
//将WEL置位   
void SPI_FLASH_Write_Enable(void)   
{
	SPI1_SCN_L();;                            //使能器件   
    spi1_exchange_byte(W25X_WriteEnable);      //发送写使能  
	SPI1_SCN_H();                            //取消片选     	      
}

//SPI_FLASH写禁止	
//将WEL清零  
void SPI_FLASH_Write_Disable(void)   
{  
	SPI1_SCN_L();;                            //使能器件   
    spi1_exchange_byte(W25X_WriteDisable);     //发送写禁止指令    
	SPI1_SCN_H();                            //取消片选     	      
} 

//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64   	  
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI1_SCN_L();;				    
	spi1_exchange_byte(0x90);//发送读取ID命令	    
	spi1_exchange_byte(0x00); 	    
	spi1_exchange_byte(0x00); 	    
	spi1_exchange_byte(0x00); 	 			   
	Temp|=spi1_exchange_byte(0xFF)<<8;  
	Temp|=spi1_exchange_byte(0xFF);	 
	SPI1_SCN_H();				    
	return Temp;
}

//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	SPI1_SCN_L();;                            //使能器件   
    spi1_exchange_byte(W25X_ReadData);         //发送读取命令   
    spi1_exchange_byte((u8)((ReadAddr)>>16));  //发送24bit地址    
    spi1_exchange_byte((u8)((ReadAddr)>>8));   
    spi1_exchange_byte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	  { 
        pBuffer[i]=spi1_exchange_byte(0XFF);   //循环读数  
    }
	SPI1_SCN_H();  				    	      
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI1_SCN_L();;                            //使能器件   
    spi1_exchange_byte(W25X_PageProgram);      //发送写页命令   
    spi1_exchange_byte((u8)((WriteAddr)>>16)); //发送24bit地址    
    spi1_exchange_byte((u8)((WriteAddr)>>8));   
    spi1_exchange_byte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)spi1_exchange_byte(pBuffer[i]);//循环写数  
	SPI1_SCN_H();                            //取消片选 
	SPI_Flash_Wait_Busy();					   //等待写入结束
}

//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
}

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   	 
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * SPI_FLASH_BUF;	  
   	SPI_FLASH_BUF=w25qxx_flash_buf;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;		//需要擦除  	  
		}
		if(i<secremain)															//需要擦除
		{
			SPI_Flash_Erase_Sector(secpos);						//擦除这个扇区
			for(i=0;i<secremain;i++)	   							//复制
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);	//写入整个扇区  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);	//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;					//写入结束了
		else																				//写入未结束
		{
			secpos++;																	//扇区地址增1
			secoff=0;																	//偏移位置为0 	 

		   	pBuffer+=secremain;  										//指针偏移
			WriteAddr+=secremain;											//写地址偏移	   
		   	NumByteToWrite-=secremain;							//字节数递减
			if(NumByteToWrite>4096)secremain=4096;		//下一个扇区还是写不完
			else secremain=NumByteToWrite;						//下一个扇区可以写完了
		}	 
	};	 
}

//擦除整个芯片		  
//等待时间超长...
void SPI_Flash_Erase_Chip(void)   
{                                   
    SPI_FLASH_Write_Enable();                  	//SET WEL 
    SPI_Flash_Wait_Busy();   
  	SPI1_SCN_L();;                            			//使能器件   
    spi1_exchange_byte(W25X_ChipErase);        	//发送片擦除命令  
	SPI1_SCN_H();                            				//取消片选     	      
	SPI_Flash_Wait_Busy();   				   						//等待芯片擦除结束
}

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{  
	//监视falsh擦除情况,测试用     
		Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                  	//SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	SPI1_SCN_L();;                            			//使能器件   
    spi1_exchange_byte(W25X_SectorErase);      	//发送扇区擦除指令 
    spi1_exchange_byte((u8)((Dst_Addr)>>16));  	//发送24bit地址    
    spi1_exchange_byte((u8)((Dst_Addr)>>8));   
    spi1_exchange_byte((u8)Dst_Addr);  
		SPI1_SCN_H();                            			//取消片选     	      
    SPI_Flash_Wait_Busy();   				   					//等待擦除完成
}

//等待空闲
void SPI_Flash_Wait_Busy(void)   
{   
	while((SPI_Flash_ReadSR()&0x01)==0x01);   		// 等待BUSY位清空
}

//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
  	SPI1_SCN_L();;                            			//使能器件   
    spi1_exchange_byte(W25X_PowerDown);        	//发送掉电命令  
		SPI1_SCN_H();                            			//取消片选     	      
                                  	//等待TPD  
}

//唤醒
void SPI_Flash_WAKEUP(void)   
{  
  	SPI1_SCN_L();;                            			//使能器件   
    spi1_exchange_byte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB    
		SPI1_SCN_H();                            			//取消片选     	      
                                	//等待TRES1
}   
*/



	
	

