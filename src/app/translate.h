

#ifndef __TRANSLATE_H_
#define __TRANSLATE_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include  <includes.h>





#define TRANSLATE_BUFF_LENGHT				512



#define USART_CHANNEL1							1
#define USART_CHANNEL2							2
#define USART_CHANNEL3							3
#define USART_CHANNEL4							4



void usart3_task_create(void);
void translate_task_create(void);
static void usart3_task_fun(void *p_arg);
static void usart1_task_fun(void *p_arg);
static void translate_task_fun(void *p_arg);
u8 usart_process_protocol(u8 *buff, u16 size, u8 channel);



#endif
