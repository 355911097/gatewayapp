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
#include <stm32f2xx_tim.h>
#include "timer.h"



volatile uint32_t g_timer_cnt[(uint8_t)timer_max] = {0};
volatile uint32_t g_timeout_cnt = 0;

u32 time_heart_ms = 0;	//心跳计数器

u32 time_ms = 0;
u8 time_second = 0;
u8 time_minute = 0;
u8 time_hour = 0;


/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值 
*
* Return(s)   : 0 定时器时间到  1定时器时间未到
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void timer3_init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef tim_init_structure;
	
	TIM_DeInit(TIM3);
	tim_init_structure.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init_structure.TIM_ClockDivision = TIM_CKD_DIV1;
	tim_init_structure.TIM_Period = arr;		//发生中断时间=(TIM_Prescaler+1)* (TIM_Period+1)/FLK
	tim_init_structure.TIM_Prescaler = psc;	
	TIM_TimeBaseInit(TIM3, &tim_init_structure);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_SetCounter(TIM3, 0);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
	
	BSP_IntVectSet(BSP_INT_ID_TIM3, TIM3_IRQHandler);
}



/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值 
*
* Return(s)   : 0 定时器时间到  1定时器时间未到
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void TIM3_IRQHandler(void)
{
	uint16_t i = 0;

	if (SET == TIM_GetITStatus(TIM3, TIM_IT_Update))
    {
        TIM_ClearITPendingBit( TIM3, TIM_IT_Update);
		
		if (g_timeout_cnt != 0x00)	//延时
		{
			g_timeout_cnt--;
		}
				
        for (i = 0; i<(uint8_t)timer_max; i++)	//毫秒级定时器
        {
            g_timer_cnt[i]++;
        }
		

		/****时间****/			
		time_ms++;
		if(time_ms > 999)
		{
			time_ms = 0;	
			time_second++;
			if(time_second > 59)
			{	
				time_second = 0;
				time_minute++;
				if(time_minute > 59)
				{
					time_minute = 0;
					time_hour++;
					if (time_hour > 23)
					{
						time_hour = 0;
					}
				}
			}
			
		}	
		
	
		time_heart_ms++;
    }
}




/*
*********************************************************************************************************
*                                          timer_get_heart_ms()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值
*
* Return(s)   : 返回心跳包计数器的值（单位：ms）
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
u32 timer_get_heart_ms(void)
{
	return time_heart_ms;
}


/*
*********************************************************************************************************
*                                          timer_delay_1ms()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值 
*
* Return(s)   : 0 定时器时间到  1定时器时间未到
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 timer_get_time_minute(void)
{
	return time_minute;
}	

/*
*********************************************************************************************************
*                                          timer_delay_1ms()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值 
*
* Return(s)   : 0 定时器时间到  1定时器时间未到
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 timer_get_time_second(void)
{
	return time_second;
}	


/*
*********************************************************************************************************
*                                          timer_delay_1ms()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值 
*
* Return(s)   : 0 定时器时间到  1定时器时间未到
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
date_time_t timer_get_time(void)
{
	date_time_t dt;
	
	dt.hour = time_hour;
	dt.minute = time_minute;
	dt.second = time_second;
	dt.ms = time_ms;
	
	return dt;
}

/*
*********************************************************************************************************
*                                          timer_delay_1ms()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值 
*
* Return(s)   : 0 定时器时间到  1定时器时间未到
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void timer_delay_1ms(uint32_t ms)
{
	g_timeout_cnt = ms;		 
	while(g_timeout_cnt != 0);					//
}


/*
*********************************************************************************************************
*                                          timer_is_timeout_1MS()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type 定时器类型  enum timer3
*				count 定时器计数值 
*
* Return(s)   : 0 定时器时间到  1定时器时间未到
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
int timer_is_timeout_1MS(uint8_t type, uint32_t count)
{
	int status = 0;
	
	if (g_timer_cnt[(uint8_t)(type)] >= count)
	{
		g_timer_cnt[(uint8_t)(type)] = 0;
		status = 0;
	}
	else
	{
		status = 1;
	}
	
	return status;
}











