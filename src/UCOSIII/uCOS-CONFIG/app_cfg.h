/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM3220G-EVAL
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#include  <stdio.h>

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  BSP_CFG_LED_SPI2_EN                    DEF_ENABLED       /* Enable/disable LEDs on SPI port.                     */
#define  BSP_CFG_LED_PIOC_EN                    DEF_ENABLED       /* Enable/disable PIOC LEDs.                            */

#define  APP_CFG_SERIAL_EN                      DEF_ENABLED


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                             2u

#define  OS_TASK_TMR_PRIO                       (OS_LOWEST_PRIO - 2)


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
#define APP_TASK_START_STK_SIZE                         128u
#define LED0_TASK_STK_SIZE								128
#define LED1_TASK_STK_SIZE								128
#define USART1_TASK_STK_SIZE							600
#define USART3_TASK_STK_SIZE							1000
#define GPRS_INIT_TASK_STK_SIZE							128
#define GPRS_RECV_TASK_STK_SIZE							128
#define ETH_INIT_TASK_STK_SIZE							128
#define DHCP_TASK_STK_SIZE								128
#define RAWUDP_TASK_STK_SIZE							256
#define PROTOCOL_TASK_STK_SIZE							600
#define TRANSLATE_TASK_STK_SIZE							128






/*
*********************************************************************************************************
*                                            BSP CFG
*********************************************************************************************************
*/

#define  BSP_CFG_SER_COMM_SEL                   BSP_SER_COMM_UART_01




/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/
#if 0
#define  TRACE_LEVEL_OFF                                0
#define  TRACE_LEVEL_INFO                               1
#define  TRACE_LEVEL_DBG                                2
#endif

#define  APP_TRACE_LEVEL                                TRACE_LEVEL_INFO

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
#define  APP_TRACE                                      BSP_Ser_Printf
#else
#define  APP_TRACE                                      printf
#endif

#define  APP_TRACE_INFO(x)               ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_TRACE x) : (void)0)

#endif

