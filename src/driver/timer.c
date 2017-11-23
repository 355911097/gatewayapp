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

extern u32 lwip_localtime;	


u32 heart_tick = 0;

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
*Function: TIM2_IRQHandler      
*Description: 定时器2中断服务程序
*Calls: 
*Data Accessed:  
*Data Updated: g_tim_cnt   
*Input: 无
*Output: 无
*Return: 成功TRUE 失败FALSH    
*Others: 无    
*/
void TIM3_IRQHandler(void)
{
	uint16_t i = 0;

	if (SET == TIM_GetITStatus(TIM3, TIM_IT_Update))
    {
        TIM_ClearITPendingBit( TIM3, TIM_IT_Update);
		
		if (g_timeout_cnt != 0x00)
		{
			g_timeout_cnt--;
		}
				
        for (i = 0; i<(uint8_t)timer_max; i++)
        {
            g_timer_cnt[i]++;
        }

		heart_tick++;	
		
		lwip_localtime++;
    }
}


void timer_delay_1ms(uint32_t ms)
{
	g_timeout_cnt = ms;		 
	while(g_timeout_cnt != 0);					//
}


u32 get_heart_tick(void)
{
	return heart_tick;
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











