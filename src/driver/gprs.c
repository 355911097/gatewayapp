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
    2. ...
*************************************************/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <string.h>
#include "gprs.h"
#include "bsp.h"
#include "usart.h"
#include "timer.h"


dev_info_t dev_info;

extern usart_buff_t  *gprs_rx_buff;
extern u16 usart2_rx_status;




uint8_t gprs_status = 0;	//GPRS的状态
uint8_t gprs_rx_buf[256];
uint16_t gprs_rx_cnt = 0;

uint8_t send_at_flag = 0;	

/*
*********************************************************************************************************
*                                          led0_task_create()
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
void gprs_power_on(void)
{
	OS_ERR err;
	
	GPIO_SetBits(GPIOC, GPIO_Pin_1);				//GPRS_PWR
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
	
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);
	OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err);
	GPIO_SetBits(GPIOA, GPIO_Pin_7);				//GPRS_RESET
	OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err);

	
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	OSTimeDlyHMSM(0,0,1,100,OS_OPT_TIME_HMSM_STRICT,&err);
	
}



/*
*********************************************************************************************************
*                                          gprs_check_cmd()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 *gprs_check_cmd(u8 *p_str)
{
	char *str = 0;

	USART_OUT(USART3, gprs_rx_buff->pdata);

	str = strstr((const char*)gprs_rx_buff->pdata, (const char*)p_str);
	

	return (u8*)str;
}


/*
*********************************************************************************************************
*                                          gprs_send_at()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 gprs_send_at(u8 *cmd, u8 *ack, u16 waittime, u16 timeout)
{
	OS_ERR err;
	u8 res = 1;
	
	timer_is_timeout_1MS(timer_at, 0);	//开始定时器timer_at
	while (res == 1)
	{
		
		memset(gprs_rx_buff, 0, sizeof(usart_buff_t));
		usart2_rx_status = 0;
		USART_OUT(USART2, cmd);							
		OSTimeDly(waittime, OS_OPT_TIME_DLY, &err);		//AT指令延时
		
		usart2_rx_status = 1;	//数据未处理 不在接收数据
		
		if (gprs_check_cmd(ack))	
		{
			res = 0;		//监测到正确的应答数据
			usart2_rx_status = 0;	//数据处理完 开始接收数据
			break;
		}
			
		if (timer_is_timeout_1MS(timer_at, timeout))	//定时器timer_at结束
		{
			res = 1;
			usart2_rx_status = 0;	//数据处理完 开始接收数据
			break;
		}
		
		
	}
	
	return res;
}

















