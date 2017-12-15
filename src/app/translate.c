


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <string.h>
#include "bsp.h"
#include "translate.h"
#include "protocol.h"
#include "app.h"
#include "usart.h"
#include "rawudp.h"



extern struct udp_pcb *udppcb;  	//定义一个UDP服务器控制块

extern OS_Q usart3_msg;

extern usart_buff_t *usart3_rx_buff;
extern u8 usart3_rx_status;


OS_TCB 	usart3_task_TCB;
CPU_STK	usart3_task_stk[USART3_TASK_STK_SIZE];

OS_TCB 	translate_task_TCB;
CPU_STK	translate_task_stk[TRANSLATE_TASK_STK_SIZE];


u8 translate_buff[TRANSLATE_BUFF_LENGHT] = {0};
u16 translate_buff_len = 0;


void usart3_task_create(void)
{
	OS_ERR os_err;
	
	OSTaskCreate((OS_TCB 	* )&usart3_task_TCB,		
				 (CPU_CHAR	* )"usart3 task", 		
                 (OS_TASK_PTR )usart3_task_fun, 			
                 (void		* )0,					
                 (OS_PRIO	  )USART3_TASK_PRIO,     
                 (CPU_STK   * )&usart3_task_stk[0],	
                 (CPU_STK_SIZE)USART3_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)USART3_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err);                                             /* Create Application Kernel Objects                        */
 
	if(os_err != OS_ERR_NONE)
	{
		USART_OUT(USART3, "\r usart3_task_create fail\r");			 
	}			 

}



void translate_task_create(void)
{
	OS_ERR os_err;
	
	OSTaskCreate((OS_TCB 	* )&translate_task_TCB,		
				 (CPU_CHAR	* )"translate task", 		
                 (OS_TASK_PTR )translate_task_fun, 			
                 (void		* )0,					
                 (OS_PRIO	  )TRANSLATE_TASK_PRIO,     
                 (CPU_STK   * )&translate_task_stk[0],	
                 (CPU_STK_SIZE)TRANSLATE_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)TRANSLATE_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err);                                             /* Create Application Kernel Objects                        */
 

	if(os_err != OS_ERR_NONE)
	{
		USART_OUT(USART3, "\r translate_task_create fail\r");			 
	}
	
}


/*
*********************************************************************************************************
*                                          usart3_task_fun()
*
* Description : Create application tasks.
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
static void usart3_task_fun(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size = 0;
	u8 buff[100] = {0};
	u8 *msg = buff;



	while(DEF_TRUE)
	{	

		msg = OSQPend((OS_Q*		)&usart3_msg,   
					(OS_TICK		)0,
					(OS_OPT			)OS_OPT_PEND_NON_BLOCKING,
					(OS_MSG_SIZE*	)&size,		
					(CPU_TS*		)0,
					(OS_ERR*		)&err);

		
		if(usart3_rx_status == 1)
		{
			
			
			usart3_process_protocol(msg, size, 3);
			
			usart_printf(USART3, size, msg);
			
			memset(usart3_rx_buff, 0, sizeof(usart_buff_t));	//清空接收缓冲区
		}
	
		
		OSTimeDlyHMSM(0,0,0,500, OS_OPT_TIME_HMSM_STRICT, &err);
		
	}
		
}






/*
*********************************************************************************************************
*                                          usart3_task_fun()
*
* Description : Create application tasks.
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
static void translate_task_fun(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size = 0;
	u8 buff[100] = {0};
	u8 *msg = buff;



	while(DEF_TRUE)
	{	
		

		
		
		OSTimeDly(500, OS_OPT_TIME_DLY, &err);
		
	}
		
}




///////串口协议解析//////////////////////////




u8 usart3_process_protocol(u8 *buff, u16 size, u8 channel)
{
	
	u32 area_id = 0, addr_id = 0, point_id = 0;
	u16 cmd = 0;
	
	
	cmd = 0x0101;
	
	
	

	
	switch(cmd)
	{
		case 0x0101:
			fire_alarm_0101(CHANNEL_ETH);
			
		break;
		
		
		case 0x0102:
			
		break;
	
	}

	return TRUE;
}








