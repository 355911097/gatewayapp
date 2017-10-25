



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
* Description : W25XX ʹ��д��
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
* Description : W25QXX ��ʹ��д��
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
	while((w25qxx_read_SR()&0x01)==0x01);   // �ȴ�BUSYλ���
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
*				0XEF13,��ʾоƬ�ͺ�ΪW25Q80 
*				0XEF14,��ʾоƬ�ͺ�ΪW25Q16 
*				0XEF15,��ʾоƬ�ͺ�ΪW25Q32 
*				0XEF16,��ʾоƬ�ͺ�ΪW25Q64
*				0XEF17,��ʾоƬ�ͺ�ΪW25Q128
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

	
	SPI1_SCN_L();                            //ʹ������   
    spi1_exchange_byte(W25X_ReadData);         //���Ͷ�ȡ����   
    spi1_exchange_byte((u8)((addr)>>16));  //����24bit��ַ    
    spi1_exchange_byte((u8)((addr)>>8));   
    spi1_exchange_byte((u8)addr);   
    for(i=0;i<size;i++)
	  { 
        buff[i]=spi1_exchange_byte(0XFF);   //ѭ������  
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
	SPI1_SCN_L();                         	 	//ʹ������   
    spi1_exchange_byte(W25X_PageProgram);      //����дҳ����   
    spi1_exchange_byte((u8)((addr)>>16)); 		//����24bit��ַ    
    spi1_exchange_byte((u8)((addr)>>8));   
    spi1_exchange_byte((u8)addr);   
    for(i=0;i<size;i++)spi1_exchange_byte(buff[i]);//ѭ��д��  
	SPI1_SCN_H();                            //ȡ��Ƭѡ 
	w25qxx_wait_busy();					   	//�ȴ�д�����
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
		
	pagera_main=256-addr%256; //��ҳʣ����ֽ���		 	    
	if(size<=pagera_main)pagera_main=size;//������256���ֽ�
	while(1)
	{	   
		w25qxx_write_page(buff,addr,pagera_main);
		if(size==pagera_main)break;//д�������
	 	else //size>pagera_main
		{
			buff+=pagera_main;
			addr+=pagera_main;	

			size-=pagera_main;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(size>256)pagera_main=256; //һ�ο���д��256���ֽ�
			else pagera_main=size; 	  //����256���ֽ���
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
 	secpos = addr/4096;//������ַ  
	secoff = addr%4096;//�������ڵ�ƫ��
	secremain = 4096-secoff;//����ʣ��ռ��С   

 	if(size <= secremain) secremain=size;//������4096���ֽ�
	while(1) 
	{	
		w25qxx_read(w25qxx_buf,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(w25qxx_buf[secoff+i]!=0XFF)break;		//��Ҫ����  	  
		}
		if(i<secremain)															//��Ҫ����
		{
			w25qxx_erase_sector(secpos);						//�����������
			for(i=0;i<secremain;i++)	   							//����
			{
				w25qxx_buf[i+secoff]=buff[i];	  
			}
			w25qxx_write_no_check(w25qxx_buf,secpos*4096,4096);	//д����������  

		}else w25qxx_write_no_check(buff,addr,secremain);	//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(size==secremain)break;					//д�������
		else																				//д��δ����
		{
			secpos++;																	//������ַ��1
			secoff = 0;																	//ƫ��λ��Ϊ0 	 

		   	buff += secremain;  										//ָ��ƫ��
			addr += secremain;											//д��ַƫ��	   
		   	size -= secremain;							//�ֽ����ݼ�
			if(size>4096) secremain=4096;				//��һ����������д����
			else secremain=size;						//��һ����������д����
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
	SPI1_SCN_L();                           			//ʹ������   
    spi1_exchange_byte(W25X_SectorErase);      	//������������ָ�� 
    spi1_exchange_byte((u8)((addr)>>16));  	//����24bit��ַ    
    spi1_exchange_byte((u8)((addr)>>8));   
    spi1_exchange_byte((u8)addr);  
	SPI1_SCN_H();
	w25qxx_wait_busy();	 				   					//�ȴ��������
}
	





/*
//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI1_SCN_L();;                            //ʹ������   
	spi1_exchange_byte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=spi1_exchange_byte(0Xff);             //��ȡһ���ֽ�  
	SPI1_SCN_H();                            //ȡ��Ƭѡ     
	return byte;   
} 

//дSPI_FLASH״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI1_SCN_L();;                            //ʹ������   
	spi1_exchange_byte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	spi1_exchange_byte(sr);               //д��һ���ֽ�  
	SPI1_SCN_H();                            //ȡ��Ƭѡ     	      
}

//SPI_FLASHдʹ��	
//��WEL��λ   
void SPI_FLASH_Write_Enable(void)   
{
	SPI1_SCN_L();;                            //ʹ������   
    spi1_exchange_byte(W25X_WriteEnable);      //����дʹ��  
	SPI1_SCN_H();                            //ȡ��Ƭѡ     	      
}

//SPI_FLASHд��ֹ	
//��WEL����  
void SPI_FLASH_Write_Disable(void)   
{  
	SPI1_SCN_L();;                            //ʹ������   
    spi1_exchange_byte(W25X_WriteDisable);     //����д��ָֹ��    
	SPI1_SCN_H();                            //ȡ��Ƭѡ     	      
} 

//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64   	  
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI1_SCN_L();;				    
	spi1_exchange_byte(0x90);//���Ͷ�ȡID����	    
	spi1_exchange_byte(0x00); 	    
	spi1_exchange_byte(0x00); 	    
	spi1_exchange_byte(0x00); 	 			   
	Temp|=spi1_exchange_byte(0xFF)<<8;  
	Temp|=spi1_exchange_byte(0xFF);	 
	SPI1_SCN_H();				    
	return Temp;
}

//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	SPI1_SCN_L();;                            //ʹ������   
    spi1_exchange_byte(W25X_ReadData);         //���Ͷ�ȡ����   
    spi1_exchange_byte((u8)((ReadAddr)>>16));  //����24bit��ַ    
    spi1_exchange_byte((u8)((ReadAddr)>>8));   
    spi1_exchange_byte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	  { 
        pBuffer[i]=spi1_exchange_byte(0XFF);   //ѭ������  
    }
	SPI1_SCN_H();  				    	      
}

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI1_SCN_L();;                            //ʹ������   
    spi1_exchange_byte(W25X_PageProgram);      //����дҳ����   
    spi1_exchange_byte((u8)((WriteAddr)>>16)); //����24bit��ַ    
    spi1_exchange_byte((u8)((WriteAddr)>>8));   
    spi1_exchange_byte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)spi1_exchange_byte(pBuffer[i]);//ѭ��д��  
	SPI1_SCN_H();                            //ȡ��Ƭѡ 
	SPI_Flash_Wait_Busy();					   //�ȴ�д�����
}

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
}

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   	 
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * SPI_FLASH_BUF;	  
   	SPI_FLASH_BUF=w25qxx_flash_buf;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;		//��Ҫ����  	  
		}
		if(i<secremain)															//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);						//�����������
			for(i=0;i<secremain;i++)	   							//����
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);	//д����������  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);	//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;					//д�������
		else																				//д��δ����
		{
			secpos++;																	//������ַ��1
			secoff=0;																	//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  										//ָ��ƫ��
			WriteAddr+=secremain;											//д��ַƫ��	   
		   	NumByteToWrite-=secremain;							//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;		//��һ����������д����
			else secremain=NumByteToWrite;						//��һ����������д����
		}	 
	};	 
}

//��������оƬ		  
//�ȴ�ʱ�䳬��...
void SPI_Flash_Erase_Chip(void)   
{                                   
    SPI_FLASH_Write_Enable();                  	//SET WEL 
    SPI_Flash_Wait_Busy();   
  	SPI1_SCN_L();;                            			//ʹ������   
    spi1_exchange_byte(W25X_ChipErase);        	//����Ƭ��������  
	SPI1_SCN_H();                            				//ȡ��Ƭѡ     	      
	SPI_Flash_Wait_Busy();   				   						//�ȴ�оƬ��������
}

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{  
	//����falsh�������,������     
		Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                  	//SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	SPI1_SCN_L();;                            			//ʹ������   
    spi1_exchange_byte(W25X_SectorErase);      	//������������ָ�� 
    spi1_exchange_byte((u8)((Dst_Addr)>>16));  	//����24bit��ַ    
    spi1_exchange_byte((u8)((Dst_Addr)>>8));   
    spi1_exchange_byte((u8)Dst_Addr);  
		SPI1_SCN_H();                            			//ȡ��Ƭѡ     	      
    SPI_Flash_Wait_Busy();   				   					//�ȴ��������
}

//�ȴ�����
void SPI_Flash_Wait_Busy(void)   
{   
	while((SPI_Flash_ReadSR()&0x01)==0x01);   		// �ȴ�BUSYλ���
}

//�������ģʽ
void SPI_Flash_PowerDown(void)   
{ 
  	SPI1_SCN_L();;                            			//ʹ������   
    spi1_exchange_byte(W25X_PowerDown);        	//���͵�������  
		SPI1_SCN_H();                            			//ȡ��Ƭѡ     	      
                                  	//�ȴ�TPD  
}

//����
void SPI_Flash_WAKEUP(void)   
{  
  	SPI1_SCN_L();;                            			//ʹ������   
    spi1_exchange_byte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB    
		SPI1_SCN_H();                            			//ȡ��Ƭѡ     	      
                                	//�ȴ�TRES1
}   
*/



	
	

