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
#ifndef __TIMER_H_
#define __TIMER_H_
#include "bsp.h"
#include "stm32f2xx.h"





typedef struct
{
	u8 hour;
	u8 minute;
	u8 second;
	u32 ms;

}date_time_t;




enum timer3
{
    tim_led1_cnt1,
    tim_cnt2,
	tim_cnt3,
	tim_cnt4,
	tim_cnt5,
	tim_uart2_cnt6,
	tim_ftp_cnt7,
	timer_gprs,
	timer_at,
	timer_hear,
	timer_protocol,
	timer_max
};




//如果index对应的时间超时，清零并返回1，未超时返回0
#define IS_TIMEOUT_1MS(index, count)    ((g_tim_cnt[(uint8_t)(index)] >= (count)) ?  	\
                                        ((g_tim_cnt[(uint8_t)(index)] = 0) == 0) : 0)



extern volatile uint32_t g_timer_cnt[(uint8_t)timer_max];


void timer3_init(u16 arr, u16 psc);
void TIM3_IRQHandler(void);
int timer_is_timeout_1MS(uint8_t type, uint32_t count);
u32 timer_get_heart_ms(void);
void timer_delay_1ms(uint32_t ms);
u8 timer_get_time_minute(void);
u8 timer_get_time_second(void);
date_time_t timer_get_time(void);


#endif
