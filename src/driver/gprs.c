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
#include "app.h"
#include "protocol.h"


extern usart_buff_t  *gprs_rx_buff;
extern u16 usart2_rx_status;

extern OS_TCB 	gprs_init_task_TCB;
extern CPU_STK	gprs_init_task_stk[GPRS_INIT_TASK_STK_SIZE];

extern u8 protocol_buff[PROTOCOL_BUFF_LENGHT];
extern u32 protocol_buff_len;





//dev_info_t dev_info;


uint8_t gprs_err_cnt = 0; 	//GPRS错误计数器
uint8_t gprs_status = 0;	//GPRS的状态

uint8_t gprs_rx_buf[256];
uint16_t gprs_rx_cnt = 0;

uint8_t gprs_send_at_flag = 0;	
uint8_t gprs_rx_flag = 0;



static void gprs_init_task_fun(void *p_arg);


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
			res = 0;				//监测到正确的应答数据
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






void gprs_task_create(void)
{
	OS_ERR os_err;
	CPU_SR_ALLOC();

	
	CPU_CRITICAL_ENTER();
	OSTaskCreate((OS_TCB 	* )&gprs_init_task_TCB,		
				 (CPU_CHAR	* )"gprs_init_task", 		
                 (OS_TASK_PTR )gprs_init_task_fun, 			
                 (void		* )0,					
                 (OS_PRIO	  )GPRS_INIT_TASK_PRIO,     
                 (CPU_STK   * )&gprs_init_task_stk[0],	
                 (CPU_STK_SIZE)GPRS_INIT_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)GPRS_INIT_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err);  
				 				 
	CPU_CRITICAL_EXIT();
	
	if(os_err != OS_ERR_NONE)
	{
		USART_OUT(USART3, "\gprs_init_task fail\r");			 
	}
		
}



static void gprs_init_task_fun(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size = 0;
	u8 *msg;
	u8 size1;
	u8 ret = 0;
	static u8 gprs_init_flag = TRUE;		//
		
	while(DEF_TRUE)
	{
		
		switch(gprs_status)
		{
			case 0:
				gprs_power_on();
				OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
				gprs_status = 1;
				gprs_err_cnt = 0;
			break;
					
			case 1:
				ret = gprs_send_at("\r\nAT\r\n", "OK", 800,10000);
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 2:
				ret = gprs_send_at("\r\nATI\r\n", "OK", 800, 10000);
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 3:			
				ret = gprs_send_at("\r\nAT+CPIN?\r\n", "OK", 800, 10000);
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 4:
				ret = gprs_send_at("\r\nAT+CREG=1\r\n", "OK", 800, 10000);
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 5:
				ret = gprs_send_at("\r\nAT+CSQ\r\n", "OK", 800, 10000);
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 6:
				ret = gprs_send_at("\r\nAT+CREG?\r\n", "OK", 800, 10000);
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 7:
				ret = gprs_send_at("\r\nAT^SICS=0,conType,GPRS0\r\n", "OK", 800, 10000);//?¨á￠á??óProfile éè??conType
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 8:
				ret = gprs_send_at("\r\nAT^SICS=0,APN,CMNET\r\n", "OK", 800, 10000);//
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 9:
				ret = gprs_send_at("\r\nAT^SISS=0,srvType,Socket\r\n", "OK", 800, 10000);//
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 10:
				ret = gprs_send_at("\r\nAT^SISS=0,conId,0\r\n", "OK", 800, 10000);//
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
					
			case 11:
				ret = gprs_send_at("\r\nAT^SISS=0,address,\"socktcp://180.169.14.34:16650\"\r\n", "OK", 800, 10000);//
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 12:
				ret = gprs_send_at("\r\nAT^SISO=0\r\n", "OK", 5000, 20000);//
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}

			break;
			
			case 13:
				ret = gprs_send_at("\r\nAT^IPCFL=5,20\r\n", "OK", 800, 10000);//
				if (ret == 0)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}

			break;
			
			case 14:
				ret = gprs_send_at("\r\nAT^IPENTRANS=0\r\n", "OK", 800, 10000);//
				if (ret == 0)
				{
					gprs_status = 255;
					gprs_init_flag = FALSE;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}		
			break;
				
			default:
			break;		
		}	//switch end			
	} // while end
	
	
	OSTaskDel(&gprs_init_task_TCB, &err);//
		
}






















