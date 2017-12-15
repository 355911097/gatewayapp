

#ifndef __TRANSLATE_H_
#define __TRANSLATE_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include  <includes.h>





#define TRANSLATE_BUFF_LENGHT				512




void usart3_task_create(void);
void translate_task_create(void);
static void usart3_task_fun(void *p_arg);
static void translate_task_fun(void *p_arg);
u8 usart3_process_protocol(u8 *buff, u16 size, u8 channel);



#endif
