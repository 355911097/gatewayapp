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
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM3220G-EVAL
*                                         Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <string.h>
#include  <includes.h>
#include "app.h"
#include "usart.h"
#include "timer.h"
#include "gprs.h"
#include "w25qxx.h"
#include "rtc.h"
#include "ffconf.h"
#include "diskio.h"
#include "ff.h"
#include "udp_demo.h"
#include "lan8720.h"
#include "lwip/dhcp.h"






extern void init_http(void);
extern u8 lwip_dev_init(void);
extern void lwip_periodic_handle(void);

extern void socket_examples_init(void);

extern u16 usart2_rx_status;
extern usart_buff_t *usart3_buff;
extern struct netif lwip_netif;	

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define START_TASK_PRIO			3
#define LED0_TASK_PRIO			5
#define LED1_TASK_PRIO			5
#define USART3_TASK_PRIO		4
#define GPRS_INIT_TASK_PRIO		5
#define ETH_INIT_TASK_PRIO		5
#define DHCP_TASK_PRIO			5

#define USART3_Q_NUM			1
#define ETH_Q_NUM			1
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/ 

         /* ----------------- APPLICATION GLOBALS ------------------ */
static OS_TCB   AppTaskStartTCB;
static CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static OS_TCB	led0_task_TCB;
static CPU_STK	led0_task_stk[LED0_TASK_STK_SIZE];

static OS_TCB	led1_task_TCB;
static CPU_STK	led1_task_stk[LED1_TASK_STK_SIZE];

OS_TCB 	gprs_init_task_TCB;
CPU_STK	gprs_init_task_stk[GPRS_INIT_TASK_STK_SIZE];


OS_TCB 	eth_init_task_TCB;
CPU_STK	eth_init_task_stk[ETH_INIT_TASK_STK_SIZE];

OS_TCB 	dhcp_task_TCB;
CPU_STK	dhcp_task_stk[DHCP_TASK_STK_SIZE];

OS_TCB 	usart3_task_TCB;
CPU_STK	usart3_task_stk[USART3_TASK_STK_SIZE];


CPU_STK *TCPIP_THREAD_TASK_STK;	



OS_Q usart3_msg;
OS_Q eth_msg;



OS_TMR tmr1;
OS_TMR gprs;



uint8_t status = 0;	//GPRS???
uint8_t gprs_err_cnt = 0;

FATFS *fatfs[FF_VOLUMES];//逻辑磁盘工作区.	 




/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void AppObjCreate(void);
static void AppTaskCreate(void);
static void AppTaskStart(void *p_arg);
static void led0_task_create(void *p_arg);
static void led1_task_create(void *p_arg);
static void usart3_task_create(void *p_arg);
static void gprs_init_task_create(void *p_arg);
static void eth_init_task_create(void *p_arg);
void tmr1_callback(void *p_tmr, void *p_arg);
void gprs_callback(void *p_tmr, void *p_arg);
static void dhcp_task_create(void *p_arg);



/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
	struct udp_pcb *udppcb;
	u8 nn = 0;
	FRESULT res = 0;
	FATFS fs;	
	FIL fil;
	FRESULT fr;
	DIR dirs;
	u8 work[FF_MAX_SS];
	u8 buff[100] = {0};
	u32 bw;
	OS_ERR  err;
    CPU_SR_ALLOC();			//临界区函数使用                                        /* Init OS.                                                 */
	
	BSP_Init(); 


//	nn = lwip_dev_init();
//	if(nn == 0)
//	{
//		USART_OUT(USART3, "lwip_dev_init OK\r");
//	}
//	timer_delay_1ms(1000);
//			
//#if LWIP_DHCP
//	while((lwip_dev.dhcp_status != 2)&&(lwip_dev.dhcp_status != 0XFF))//等待DHCP获取成功/超时溢出
//	{
//		lwip_periodic_handle();
//	}
//#endif			
//			
//		udppcb = udp_demo_init(); 
//	
//		udp_demo_senddata(udppcb);	
//		
//		while(1)
//		{
//			lwip_periodic_handle();
//		}
/*	
//	fs = (FATFS*)malloc(sizeof(FATFS));
//	fil = (FIL*)malloc(sizeof(FIL));
//	res = f_mkfs("", FM_ANY, 4096, work, sizeof (work));
		
//	res = f_mount(&fs, "0:", 0); 
//	if(res == 0)
//	{
//		USART_OUT(USART3, "mkfs\r");
//	}
//	
//	fr = f_mkdir("sub");
//	if(fr == 0)
//	{
//		USART_OUT(USART3, "fr1= %d\r", fr);
//	}
//	
//	fr = f_open(&fil, "123.txt", FA_CREATE_ALWAYS|FA_WRITE);
//	if(fr == 0)
//	{
//		USART_OUT(USART3, "fr1= %d\r", fr);
//	}

//	fr = f_write(&fil, "Hello, World!\r\n", 15, &bw);
//	if(fr == 0)
//	{
//		USART_OUT(USART3, "fr1= %d\r", fr);
//	}
//	f_close(&fil);

//	fr = f_open(&fil, "123.txt", FA_READ);
//	if(fr == 0)
//	{
//		USART_OUT(USART3, "fr1= %d\r", fr);
//	}
//	fr = f_read(&fil, buff, 15, &bw);
//	if(fr == 0)
//	{
//		USART_OUT(USART3, "fr1= %d\r", fr);
//	}
*/


//    BSP_IntDisAll();                                            /* Disable all interrupts.                                  */
	
	CPU_Init();                                                 /* Initialize uC/CPU services.                              */

    OSInit(&err);  
//	nn = lwip_dev_init();
//	if(nn == 0)
//	{
//		USART_OUT(USART3, "lwip_dev_init OK\r");
//	}
	
	CPU_CRITICAL_ENTER();
	
	OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                    */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK     )(APP_TASK_START_STK_SIZE / 10u),
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);


	CPU_CRITICAL_EXIT();
	OSStart(&err);			//系统开始

	while (DEF_ON) {                                            /* Should Never Get Here                                    */
    };

}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static void  AppTaskStart (void *p_arg)
{
    OS_ERR os_err;
	u8 nn = 0;
	CPU_SR_ALLOC();
	
	(void)p_arg;

//	BSP_Init();                                                 /* Init BSP fncts.                                          */   
    CPU_Init();                                                 /* Init CPU name & int. dis. time measuring fncts.          */
	BSP_CPU_TickInit();                                              /* Init CPU name & int. dis. time measuring fncts.          */
	
	
#if(OS_CFG_STAT_TASK_EN > 0)
	OSStatTaskCPUUsageInit(&os_err);	//统计任务
#endif	

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&os_err);  
#endif	


	CPU_CRITICAL_ENTER();
	AppObjCreate();
	
	AppTaskCreate();
	
	CPU_CRITICAL_EXIT();
				 		 
}








/*
*********************************************************************************************************
*                                          AppTaskCreate()
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

static  void  AppTaskCreate (void)
{
	OS_ERR os_err;
	CPU_SR_ALLOC();
	
	
	OSTaskCreate((OS_TCB 	* )&led0_task_TCB,		
				 (CPU_CHAR	* )"led0 task", 		
                 (OS_TASK_PTR )led0_task_create, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK   * )&led0_task_stk[0],	
                 (CPU_STK_SIZE)LED0_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err);                                             /* Create Application Kernel Objects                        */

	
	OSTaskCreate((OS_TCB 	* )&led1_task_TCB,		
				 (CPU_CHAR	* )"led1 task", 		
                 (OS_TASK_PTR )led1_task_create, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED1_TASK_PRIO,     
                 (CPU_STK   * )&led1_task_stk[0],	
                 (CPU_STK_SIZE)LED1_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED1_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err);                                             /* Create Application Kernel Objects                        */
				 
				 
	OSTaskCreate((OS_TCB 	* )&usart3_task_TCB,		
				 (CPU_CHAR	* )"usart3 task", 		
                 (OS_TASK_PTR )usart3_task_create, 			
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

	OSTaskCreate((OS_TCB 	* )&gprs_init_task_TCB,		
				 (CPU_CHAR	* )"gprs_init_task", 		
                 (OS_TASK_PTR )gprs_init_task_create, 			
                 (void		* )0,					
                 (OS_PRIO	  )GPRS_INIT_TASK_PRIO,     
                 (CPU_STK   * )&gprs_init_task_stk[0],	
                 (CPU_STK_SIZE)GPRS_INIT_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)GPRS_INIT_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err);                                             /* Create Application Kernel Objects                        */

	OSTaskCreate((OS_TCB 	* )&eth_init_task_TCB,		
				 (CPU_CHAR	* )"eth_init_task", 		
                 (OS_TASK_PTR )eth_init_task_create, 			
                 (void		* )0,					
                 (OS_PRIO	  )ETH_INIT_TASK_PRIO,     
                 (CPU_STK   * )&eth_init_task_stk[0],	
                 (CPU_STK_SIZE)ETH_INIT_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)ETH_INIT_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err);                                             /* Create Application Kernel Objects                        */
	
	OS_CRITICAL_ENTER();//进入临界区			 
	OSTaskCreate((OS_TCB 	* )&dhcp_task_TCB,		
				 (CPU_CHAR	* )"dhcp_task", 		
                 (OS_TASK_PTR )dhcp_task_create, 			
                 (void		* )0,					
                 (OS_PRIO	  )DHCP_TASK_PRIO,     
                 (CPU_STK   * )&dhcp_task_stk[0],	
                 (CPU_STK_SIZE)DHCP_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)DHCP_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err);                                             /* Create Application Kernel Objects                        */
	OS_CRITICAL_EXIT();	//退出临界区				 
				 
}





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
static void led0_task_create(void *p_arg)
{
	OS_ERR err;

	while(DEF_TRUE)
	{
	
		if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_3) == 1)
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_3);
		}
		else
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_3);
		//	USART_OUT(USART3, "george\r");
		}
				
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	//	OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}
	
}

static void led1_task_create(void *p_arg)
{
	OS_ERR err;

	while(DEF_TRUE)
	{

			
		if(timer_is_timeout_1MS(tim_led1_cnt1, 1000) == 0)
		{
			if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_2) == 1)
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_2);
			}
			else
			{
				GPIO_SetBits(GPIOC, GPIO_Pin_2);
			//	USART_OUT(USART3, "wangzhongya\r");
			}
		}
			
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时
//		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
	}
	
}

static void usart3_task_create(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size = 0;
	u8 buff[100] = {0};
	u8 *msg = buff;
	static u8 frist_flag = 0;
	FRESULT res = 0;
	FATFS fs;	
	FIL fil;
	FRESULT fr;
	DIR dirs;
	u32 bw;
	u8 work[FF_MAX_SS];
	rtc_calendar_t ret;


	while(DEF_TRUE)
	{



	
	
	
//	res = f_mkfs("", FM_ANY, 4096, work, sizeof (work));
		
//	res = f_mount(&fs, "", 0); 
//	if(res == 0)
//	{
//		USART_OUT(USART3, "mkfs\r");
//	}
//		
//	fr = f_open(&fil, "123.txt", FA_CREATE_ALWAYS|FA_WRITE);
//	if(fr == 0)
//	{
//		USART_OUT(USART3, "fr1= %d\r", fr);
//	}

//	fr = f_write(&fil, "Hello, World!\r\n", 15, &bw);
//	if(fr == 0)
//	{
//		USART_OUT(USART3, "fr1= %d\r", fr);
//	}
//	f_close(&fil);

//	fr = f_open(&fil, "123.txt", FA_READ);
//	if(fr == 0)
//	{
//		USART_OUT(USART3, "fr1= %d\r", fr);
//	}
//	fr = f_read(&fil, buff, 15, &bw);
//	if(fr == 0)
//	{
//		USART_OUT(USART3, "fr1= %d\r", fr);
//	}

		dhcp_coarse_tmr();
		OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err);
		
	}
		
}

void lwip_comm_dhcp_delete(void)
{
	OS_ERR err;
	
	dhcp_stop(&lwip_netif); 		//关闭DHCP
	OSTaskDel(&dhcp_task_TCB, &err);//删除DHCP任务
}


static void dhcp_task_create(void *p_arg)
{
	OS_ERR err;
	u8 nn = 0;
	u32 ip=0, netmask=0, gw=0;
	struct dhcp *pdhcp;
	u32_t ucos_time, lwip_time;


	nn = lwip_dev_init();
	if(nn == 0)
	{
		USART_OUT(USART3, "lwip_dev_init OK\r");
	}	
	
	dhcp_start(&lwip_netif);//开启DHCP 
	lwip_dev.dhcp_status=0;	//正在DHCP
	USART_OUT(USART3, "正在查找DHCP服务器,请稍等...........\r\n"); 
	
	while(DEF_TRUE)
	{
//		ucos_time=OSTimeGet(&err);
//		USART_OUT(USART3, "ucos_time = %d\r\n", ucos_time); 

		ip=lwip_netif.ip_addr.addr;		//读取新IP地址
		netmask=lwip_netif.netmask.addr;//读取子网掩码
		gw=lwip_netif.gw.addr;			//读取默认网关 
//		USART_OUT(USART3, "正在获取地址...\r\n"); 
		if(ip != 0)			//正确获取到IP地址的时候
		{
			lwip_dev.dhcp_status = 2;	//DHCP成功
			USART_OUT(USART3, "网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwip_dev.mac[0],lwip_dev.mac[1],lwip_dev.mac[2],lwip_dev.mac[3],lwip_dev.mac[4],lwip_dev.mac[5]);
			//解析出通过DHCP获取到的IP地址
			lwip_dev.ip[3]=(uint8_t)(ip>>24); 
			lwip_dev.ip[2]=(uint8_t)(ip>>16);
			lwip_dev.ip[1]=(uint8_t)(ip>>8);
			lwip_dev.ip[0]=(uint8_t)(ip);
			USART_OUT(USART3, "通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
			//解析通过DHCP获取到的子网掩码地址
			lwip_dev.netmask[3]=(uint8_t)(netmask>>24);
			lwip_dev.netmask[2]=(uint8_t)(netmask>>16);
			lwip_dev.netmask[1]=(uint8_t)(netmask>>8);
			lwip_dev.netmask[0]=(uint8_t)(netmask);
			USART_OUT(USART3, "通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
			//解析出通过DHCP获取到的默认网关	
			lwip_dev.gateway[3]=(uint8_t)(gw>>24);
			lwip_dev.gateway[2]=(uint8_t)(gw>>16);
			lwip_dev.gateway[1]=(uint8_t)(gw>>8);
			lwip_dev.gateway[0]=(uint8_t)(gw);
			USART_OUT(USART3, "通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);
			break;
		}
		else if(0 > LWIP_MAX_DHCP_TRIES) //通过DHCP服务获取IP地址失败,且超过最大尝试次数
		{
			lwip_dev.dhcp_status=0XFF;//DHCP超时失败.
			//使用静态IP地址
			IP4_ADDR(&(lwip_netif.ip_addr),lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
			IP4_ADDR(&(lwip_netif.netmask),lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
			IP4_ADDR(&(lwip_netif.gw),lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);
			USART_OUT(USART3, "DHCP服务超时,使用静态IP地址!\r\n");
			USART_OUT(USART3, "网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwip_dev.mac[0],lwip_dev.mac[1],lwip_dev.mac[2],lwip_dev.mac[3],lwip_dev.mac[4],lwip_dev.mac[5]);
			USART_OUT(USART3, "静态IP地址........................%d.%d.%d.%d\r\n",lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
			USART_OUT(USART3, "子网掩码..........................%d.%d.%d.%d\r\n",lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
			USART_OUT(USART3, "默认网关..........................%d.%d.%d.%d\r\n",lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);
			break;
		}

		dhcp_fine_tmr();
		OSTimeDly(250, OS_OPT_TIME_DLY, &err);
	}

	lwip_comm_dhcp_delete();//删除DHCP任务 
}




static void eth_init_task_create(void *p_arg)
{
	OS_ERR err;
	u8 nn = 0;

	while(DEF_TRUE)
	{


		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}

}


static void gprs_init_task_create(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size = 0;
	u8 *msg;
	u8 size1;
	u8 ret = 0;
	static u8 gprs_init_flag = TRUE;		//第一次上电运行
	
	
	while(DEF_TRUE)
	{
		if (gprs_init_flag == TRUE)
		{			
			
			switch(status)
			{
				case 0:
					gprs_power_on();
					OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
					status = 1;
					gprs_err_cnt = 0;
				break;
						
				case 1:
					ret = gprs_send_at("\r\nAT\r\n", "OK", 800,10000);
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 2:
					ret = gprs_send_at("\r\nATI\r\n", "OK", 800, 10000);
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 3:			
					ret = gprs_send_at("\r\nAT+CPIN?\r\n", "OK", 800, 10000);
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 4:
					ret = gprs_send_at("\r\nAT+CREG=1\r\n", "OK", 800, 10000);
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 5:
					ret = gprs_send_at("\r\nAT+CSQ\r\n", "OK", 800, 10000);
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 6:
					ret = gprs_send_at("\r\nAT+CREG?\r\n", "OK", 800, 10000);
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 7:
					ret = gprs_send_at("\r\nAT^SICS=0,conType,GPRS0\r\n", "OK", 800, 10000);//建立连接Profile 设置conType
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 8:
					ret = gprs_send_at("\r\nAT^SICS=0,APN,CMNET\r\n", "OK", 800, 10000);//建立连接Profile 设置APN
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 9:
					ret = gprs_send_at("\r\nAT^SISS=0,srvType,Socket\r\n", "OK", 800, 10000);//建立服务Profile 设置srvType
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 10:
					ret = gprs_send_at("\r\nAT^SISS=0,conId,0\r\n", "OK", 800, 10000);//建立服务Profile 设置conId
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
						
				case 11:
					ret = gprs_send_at("\r\nAT^SISS=0,address,\"socktcp://180.169.14.34:16650\"\r\n", "OK", 800, 10000);//建立服务Profile 设置address
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}
				break;
				
				case 12:
					ret = gprs_send_at("\r\nAT^SISO=0\r\n", "OK", 5000, 20000);//打开指定的Internet服务
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}

				break;
				
				case 13:
					ret = gprs_send_at("\r\nAT^IPCFL=5,20\r\n", "OK", 800, 10000);//设置透传模式网络参数 设置透传定时器的值
					if (ret == 0)
					{
						status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}

				break;
				
				case 14:
					ret = gprs_send_at("\r\nAT^IPENTRANS=0\r\n", "OK", 800, 10000);//进入透传模式
					if (ret == 0)
					{
						status = 255;
						gprs_init_flag = FALSE;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 5)
						{
							status = 0;
						}
					}		
				break;
					
				default:
				break;		
			}	//switch end			
		} // if end		
	} // while end
}


/*
*********************************************************************************************************
*                                          AppObjCreate()
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
static  void  AppObjCreate (void)
{
	OS_ERR err;
		
	
	OSTmrCreate((OS_TMR		*)&tmr1,		//定时器1
                (CPU_CHAR	*)"tmr1",		//定时器名字
                (OS_TICK	 )500,			//500ms
                (OS_TICK	 )10000,          //1000ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//定时器1回调函数
                (void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码	
	
	USART_OUT(USART3, "\rerr1=%d\r", err);
	OSTmrStart(&tmr1,&err);	
				
	OSTmrCreate((OS_TMR		*)&gprs,		//定时器1
                (CPU_CHAR	*)"gprs",		//定时器名字
                (OS_TICK	 )10000,			//ms
                (OS_TICK	 )0,          	//ms
                (OS_OPT		 )OS_OPT_TMR_ONE_SHOT, //周期模式
                (OS_TMR_CALLBACK_PTR)gprs_callback,//定时器1回调函数
                (void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码	
	
	OSTmrStart(&gprs,&err);		
				
	OSQCreate ((OS_Q*		)&usart3_msg,	//消息队列
                (CPU_CHAR*	)"usart3 msg",	//消息队列名称
                (OS_MSG_QTY	)USART3_Q_NUM,	//消息队列长度，这里设置为1
                (OS_ERR*	)&err);		//错误码
	
	
	OSQCreate ((OS_Q*		)&eth_msg,	//消息队列
                (CPU_CHAR*	)"eth msg",	//消息队列名称
                (OS_MSG_QTY	)USART3_Q_NUM,	//消息队列长度，这里设置为1
                (OS_ERR*	)&err);		//错误码
					
		
				
	
	
}




void tmr1_callback(void *p_tmr, void *p_arg)
{
	u8 a = 0, b = 0;
	

	
}



void gprs_callback(void *p_tmr, void *p_arg)
{
	usart2_rx_status = 1;
	
	USART_OUT(USART3, "gprs_callback\r");
//	bsp_system_reset();
}




