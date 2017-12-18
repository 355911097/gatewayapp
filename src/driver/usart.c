/*************************************************
  Copyright (C), 1988-1999,  Tech. Co., Ltd.
  File name:      
  Author:       Version:        Date:
  Description:   
  Others:         
  Function List:  
    1. ....
  History:         
                   
    1. Date:
       Author:
       Modification:
*************************************************/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "usart.h"
#include "timer.h"
#include "rawudp.h"


extern OS_Q usart1_msg;
extern OS_Q usart2_msg;
extern OS_Q usart3_msg;
extern OS_Q usart4_msg;
extern OS_Q usart5_msg;


extern OS_TCB usart3_task_TCB;
extern OS_TMR gprs;
extern struct udp_pcb *udppcb;  	//定义一个UDP服务器控制块




static usart_buff_t sb = SerialBuffDefault();

usart_buff_t *usart1_rx_buff = &sb;
usart_buff_t *usart2_rx_buff = &sb;
usart_buff_t *usart3_rx_buff = &sb;
usart_buff_t *usart4_rx_buff = &sb;
usart_buff_t *usart5_rx_buff = &sb;
usart_buff_t *gprs_rx_buff = &sb;


u8 usart1_rx_status = 0;
u8 usart2_rx_status = 0;
u8 usart3_rx_status = 0;
u8 usart4_rx_status = 0;
u8 usart5_rx_status = 0;


/*
*********************************************************************************************************
*                                          usart1_init()
*
* Description : 对usart1进行初始化
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void usart1_init(u32 band_rate, u8 word_length, u8 parity, u8 stop_bit)
{
	
	USART_InitTypeDef usart_init_structre;
	
	usart_init_structre.USART_BaudRate = band_rate;
	usart_init_structre.USART_WordLength = word_length;
	usart_init_structre.USART_StopBits = stop_bit;
	usart_init_structre.USART_Parity = parity;
	usart_init_structre.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structre.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &usart_init_structre);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
	
	BSP_IntVectSet(BSP_INT_ID_USART1, USART1_IRQHandler);

}





/*
*********************************************************************************************************
*                                          usart2_init()
*
* Description : 对usart2进行初始化.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void usart2_init(u32 band_rate)
{
	USART_InitTypeDef usart_init_structre;
	
	usart_init_structre.USART_BaudRate = band_rate;
	usart_init_structre.USART_WordLength = USART_WordLength_8b;
	usart_init_structre.USART_StopBits = USART_StopBits_1;
	usart_init_structre.USART_Parity = USART_Parity_No;
	usart_init_structre.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structre.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &usart_init_structre);
		
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);
	
	BSP_IntVectSet(BSP_INT_ID_USART2, USART2_IRQHandler);

}




/*
*********************************************************************************************************
*                                          usart3_init()
*
* Description : 对usart3进行初始化
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void usart3_init(u32 band_rate)
{
	USART_InitTypeDef usart_init_structre;
	
	usart_init_structre.USART_BaudRate = band_rate;
	usart_init_structre.USART_WordLength = USART_WordLength_8b;
	usart_init_structre.USART_StopBits = USART_StopBits_1;
	usart_init_structre.USART_Parity = USART_Parity_No;
	usart_init_structre.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structre.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &usart_init_structre);
		

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	
	USART_Cmd(USART3, ENABLE);
	
	BSP_IntVectSet(BSP_INT_ID_USART3, USART3_IRQHandler);

}



/*
*********************************************************************************************************
*                                          usart4_init()
*
* Description : 对usart4进行初始化
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void usart4_init(u32 band_rate)
{
	USART_InitTypeDef usart_init_structre;
	
	usart_init_structre.USART_BaudRate = band_rate;
	usart_init_structre.USART_WordLength = USART_WordLength_8b;
	usart_init_structre.USART_StopBits = USART_StopBits_1;
	usart_init_structre.USART_Parity = USART_Parity_No;
	usart_init_structre.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structre.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &usart_init_structre);
		

	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	
	
	USART_Cmd(UART4, ENABLE);
	
	BSP_IntVectSet(BSP_INT_ID_USART4, USART3_IRQHandler);

}

/*
*********************************************************************************************************
*                                          usart5_init()
*
* Description : 对usart5进行初始化
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void usart5_init(u32 band_rate)
{
	USART_InitTypeDef usart_init_structre;
	
	usart_init_structre.USART_BaudRate = band_rate;
	usart_init_structre.USART_WordLength = USART_WordLength_8b;
	usart_init_structre.USART_StopBits = USART_StopBits_1;
	usart_init_structre.USART_Parity = USART_Parity_No;
	usart_init_structre.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_structre.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &usart_init_structre);
		

	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(UART5, ENABLE);
	
	BSP_IntVectSet(BSP_INT_ID_USART5, UART5_IRQHandler);

}




/*
*********************************************************************************************************
*                                          USART1_IRQHandler()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void USART1_IRQHandler(void)
{
	OS_ERR err;	
	u8 ch = 0;	

	OSIntEnter();   
   	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {   
	    USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
				
        ch = USART_ReceiveData(USART1);	 
		if(usart1_rx_status == 0)
		{
			if (gprs_rx_buff->index < USART_BUFF_LENGHT)
			{
				//开始定时器
//				timer_is_timeout_1MS(timer_gprs, 0);
				usart1_rx_buff->pdata[usart1_rx_buff->index++] = ch;
			
			//	USART_SendData(USART3, ch);	
//			OSQPost((OS_Q*		)&usart3_msg,
//					(void*		)usart3_buff->pdata,
//					(OS_MSG_SIZE)10,
//					(OS_OPT		)OS_OPT_POST_FIFO,
//					(OS_ERR*	)&err);
			}
			else
			{
				memset(usart1_rx_buff, 0, sizeof(usart_buff_t));	//清理缓冲区		
			}
		}
	}
	
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                  
  	{ 
     	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);					   
  	}	
	
	OSIntExit();  	
}

void usart1_recv_data(void)
{
	OS_ERR err;
	u8 ch = 0;
	
	if(usart1_rx_status == 0)
	{
	
		ch = USART_ReceiveData(USART1);	 

		if (usart1_rx_buff->index < USART_BUFF_LENGHT)
		{
			usart1_rx_buff->pdata[usart1_rx_buff->index++] = ch;
				
			if(ch == 0x16)
			{
				OSQPost((OS_Q*		)&usart1_msg,
						(void*		)usart1_rx_buff->pdata,
						(OS_MSG_SIZE)usart1_rx_buff->index,
						(OS_OPT		)OS_OPT_POST_FIFO,
						(OS_ERR*	)&err);
						
				usart1_rx_status = 1;
			}		
		}
		else
		{
			memset(usart1_rx_buff, 0, sizeof(usart_buff_t));	
		}
	}	
}

/*
*********************************************************************************************************
*                                          USART2_IRQHandler()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void USART2_IRQHandler(void)
{
	OS_ERR err;	
	u8 ch = 0;	

	OSIntEnter();   
   	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {   
	    USART_ClearITPendingBit(USART2, USART_IT_RXNE);	
		
		timer_is_timeout_1MS(timer_gprs, 0);		//定时器清零
		
		if(usart2_rx_status == 0)
		{
			ch = USART_ReceiveData(USART2);	 
			
			if (gprs_rx_buff->index < USART_BUFF_LENGHT)
			{
				
				usart2_rx_buff->pdata[usart2_rx_buff->index++] = ch;
				
			}
			else
			{
				memset(usart2_rx_buff, 0, sizeof(usart_buff_t));			
			}
		}
	}
	
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)                  
  	{ 
     	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);					   
  	}	
	
	OSIntExit();  	
}


void usart2_recv_data(void)
{
	OS_ERR err;
	u8 ch = 0;
	
	if(timer_is_timeout_1MS(timer_gprs, 40))	//40ms没接收到数据认为接收数据完成		
	{
		usart2_rx_status = 1;
	}	
}



/*
*********************************************************************************************************
*                                          USART3_IRQHandler()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void USART3_IRQHandler(void)
{
	OS_ERR err;
	
	u8 ch = 0;	
	u8 tmp[5] = {0};
	
	OSIntEnter();   
   	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {   
	    USART_ClearITPendingBit(USART3, USART_IT_RXNE);	
				
		usart3_recv_data();
	}
	
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)                  
  	{ 
     	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);					   
  	}	
	
	OSIntExit();  	
}



/*
*********************************************************************************************************
*                                          usart3_recv_data()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void usart3_recv_data(void)
{
	OS_ERR err;
	u8 ch = 0;
	
	if(usart3_rx_status == 0)
	{
	
		ch = USART_ReceiveData(USART3);	 

		if (usart3_rx_buff->index < USART_BUFF_LENGHT)
		{
			usart3_rx_buff->pdata[usart3_rx_buff->index++] = ch;
				
			if(ch == 0x16)
			{
				OSQPost((OS_Q*		)&usart3_msg,
						(void*		)usart3_rx_buff->pdata,
						(OS_MSG_SIZE)usart3_rx_buff->index,
						(OS_OPT		)OS_OPT_POST_FIFO,
						(OS_ERR*	)&err);
						
				usart3_rx_status = 1;
			}		
		}
		else
		{
			memset(usart3_rx_buff, 0, sizeof(usart_buff_t));	
		}
	}	
}






/*
*********************************************************************************************************
*                                          USART3_IRQHandler()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void UART5_IRQHandler(void)
{
	OS_ERR err;
	
	u8 ch = 0;	
	u8 tmp[5] = {0};
	
	OSIntEnter();   
   	if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {   
	    USART_ClearITPendingBit(UART5, USART_IT_RXNE);	
				
        ch = USART_ReceiveData(UART5);	 
		
		if (usart5_rx_buff->index < USART_BUFF_LENGHT)
		{
			usart5_rx_buff->pdata[usart5_rx_buff->index++] = ch;
						
		
//			OSQPost((OS_Q*		)&usart3_msg,
//					(void*		)usart3_rx_buff->pdata,
//					(OS_MSG_SIZE)usart3_rx_buff->index,
//					(OS_OPT		)OS_OPT_POST_FIFO,
//					(OS_ERR*	)&err);
			
		//			USART_SendData(USART3, ch);		
		}
		else
		{
			memset(usart5_rx_buff, 0, sizeof(usart_buff_t));
			
		}
		
	}
	
	if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)                  
  	{ 
     	USART_ITConfig(UART5, USART_IT_TXE, DISABLE);					   
  	}	
	
	OSIntExit();  	
}










/*
*********************************************************************************************************
*                                          USART_OUT()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : 但是在仔细看手册的时候发现 TC 和 TXE 标志位在复位的时候被置1 ，这样第一次while循环就是没有用的。这样导致了首次第一个字符还没有被输出，
*               就被后面的字符覆盖掉，造成实际看到的丢失现象。解决办法就很简单：在前面加上一句 USART1->SR
*********************************************************************************************************
*/
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
{ 
	const char *s;
    int d;  
    char buf[32];
    
	va_list ap;
    __va_start(ap, Data);
	
	
	USART_GetFlagStatus(USARTx, USART_FLAG_TC);	//
	while(*Data != '\0')
	{				                         
		if(*Data==0x5c)
		{									
			switch (*++Data)
			{
				case 'r':							          
					USART_SendData(USARTx, 0x0d);	   
					Data++;
				break;
				case 'n':							          
					USART_SendData(USARTx, 0x0a);	
					Data++;
				break;
				
				default:
					Data++;
			    break;
			}						 
		}
		else if(*Data=='%')
		{									  //
			switch (*++Data){				
				case 's':										  
                	s = __va_arg(ap, const char *);
                	for ( ; *s; s++) 
					{
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
            	case 'd':										 
                	d = __va_arg(ap, int);
					
					sprintf(buf, "%d", d);
                	for (s = buf; *s; s++) 
					{
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
				default:
					Data++;
				    break;
			}		 
		}
		else 
			USART_SendData(USARTx, *Data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
}




void usart_printf(USART_TypeDef* USARTx, uint16_t data_size, uint8_t *data,...)
{ 
	const char *s;
    int d;  
    char buf[32];
    
	va_list ap;
    __va_start(ap, data);
	
	
	USART_GetFlagStatus(USARTx, USART_FLAG_TC);	//
	while(data_size--)
	{				                         
		if(*data==0x5c)
		{									
			switch (*++data)
			{
				case 'r':							          
					USART_SendData(USARTx, 0x0d);	   
					data++;
				break;
				case 'n':							          
					USART_SendData(USARTx, 0x0a);	
					data++;
				break;
				
				default:
					data++;
			    break;
			}						 
		}
		else if(*data=='%')
		{									  //
			switch (*++data){				
				case 's':										  
                	s = __va_arg(ap, const char *);
                	for ( ; *s; s++) 
					{
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					data++;
                	break;
            	case 'd':										 
                	d = __va_arg(ap, int);
					
					sprintf(buf, "%d", d);
                	for (s = buf; *s; s++) 
					{
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					data++;
                	break;
				default:
					data++;
				    break;
			}		 
		}
		else 
			USART_SendData(USARTx, *data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
}














