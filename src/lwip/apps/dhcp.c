



#include "dhcp.h" 
#include "usart.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
#include "timer.h"
#include "lwip/ip4_addr.h"
#include "lwip/dhcp.h"
#include "lan8720.h"
#include "lwip_comm.h"
#include "app.h"
#include "rawudp.h"


extern struct netif lwip_netif;	


OS_TCB 	dhcp_task_TCB;
CPU_STK	dhcp_task_stk[DHCP_TASK_STK_SIZE];

static void dhcp_task_fun(void *p_arg);
void dhcp_task_delete(void);

void dhcp_task_create(void)
{
	OS_ERR os_err;
	CPU_SR_ALLOC();

	
	CPU_CRITICAL_ENTER();
	OSTaskCreate((OS_TCB 	* )&dhcp_task_TCB,		
				 (CPU_CHAR	* )"dhcp_task", 		
                 (OS_TASK_PTR )dhcp_task_fun, 			
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
			 
	USART_OUT(USART3, "\dhcp_task err=%d\r", os_err);
				 
	CPU_CRITICAL_EXIT();
				 
	OSTimeDly(10, OS_OPT_TIME_DLY, &os_err);
	USART_OUT(USART3, "\rrawudp_task err=%d\r", os_err);	


}






static void dhcp_task_fun(void *p_arg)
{
	OS_ERR err;
	u8 nn = 0;
	u32 ip=0, netmask=0, gw=0;
	struct dhcp *pdhcp;
	u32_t ucos_time, lwip_time;


	nn = lwip_comm_dev_init();
	if(nn == 0)
	{
		USART_OUT(USART3, "lwip_dev_init OK\r");
	}	
		
	pdhcp->tries = 0;
	
	while(DEF_TRUE)
	{
		
		switch(lwip_dev.dhcp_status)
		{
			case 0:
				dhcp_start(&lwip_netif);//����DHCP 
				USART_OUT(USART3, "����DHCP������,���Ե�...........\r\n"); 
				lwip_dev.dhcp_status = 1;
			break;
			
			case 1:
				ip=lwip_netif.ip_addr.addr;		//��ȡ��IP��ַ
				netmask=lwip_netif.netmask.addr;//��ȡ��������
				gw=lwip_netif.gw.addr;			//��ȡĬ������ 
			
				if(ip != 0)			//��ȷ��ȡ��IP��ַ��ʱ��
				{
					lwip_dev.dhcp_status = 2;	//DHCP�ɹ�
					USART_OUT(USART3, "����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwip_dev.mac[0],lwip_dev.mac[1],lwip_dev.mac[2],lwip_dev.mac[3],lwip_dev.mac[4],lwip_dev.mac[5]);
					//������ͨ��DHCP��ȡ����IP��ַ
					lwip_dev.ip[3]=(uint8_t)(ip>>24); 
					lwip_dev.ip[2]=(uint8_t)(ip>>16);
					lwip_dev.ip[1]=(uint8_t)(ip>>8);
					lwip_dev.ip[0]=(uint8_t)(ip);
					USART_OUT(USART3, "ͨ��DHCP��ȡ��IP��ַ..............%d.%d.%d.%d\r\n",lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
					//����ͨ��DHCP��ȡ�������������ַ
					lwip_dev.netmask[3]=(uint8_t)(netmask>>24);
					lwip_dev.netmask[2]=(uint8_t)(netmask>>16);
					lwip_dev.netmask[1]=(uint8_t)(netmask>>8);
					lwip_dev.netmask[0]=(uint8_t)(netmask);
					USART_OUT(USART3, "ͨ��DHCP��ȡ����������............%d.%d.%d.%d\r\n",lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
					//������ͨ��DHCP��ȡ����Ĭ������	
					lwip_dev.gateway[3]=(uint8_t)(gw>>24);
					lwip_dev.gateway[2]=(uint8_t)(gw>>16);
					lwip_dev.gateway[1]=(uint8_t)(gw>>8);
					lwip_dev.gateway[0]=(uint8_t)(gw);
					USART_OUT(USART3, "ͨ��DHCP��ȡ����Ĭ������..........%d.%d.%d.%d\r\n",lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);

				}
				else if(0 > LWIP_MAX_DHCP_TRIES) //ͨ��DHCP�����ȡIP��ַʧ��,�ҳ�������Դ���
				{
					
					lwip_dev.dhcp_status = 0xFF;//DHCP��ʱʧ��.
					//ʹ�þ�̬IP��ַ
					IP4_ADDR(&(lwip_netif.ip_addr),lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
					IP4_ADDR(&(lwip_netif.netmask),lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
					IP4_ADDR(&(lwip_netif.gw),lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);
					USART_OUT(USART3, "DHCP����ʱ,ʹ�þ�̬IP��ַ!\r\n");
					USART_OUT(USART3, "����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwip_dev.mac[0],lwip_dev.mac[1],lwip_dev.mac[2],lwip_dev.mac[3],lwip_dev.mac[4],lwip_dev.mac[5]);
					USART_OUT(USART3, "��̬IP��ַ........................%d.%d.%d.%d\r\n",lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
					USART_OUT(USART3, "��������..........................%d.%d.%d.%d\r\n",lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
					USART_OUT(USART3, "Ĭ������..........................%d.%d.%d.%d\r\n",lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);

				}
			break;
				
			case 2:
				
				rawudp_task_create();		//udp����ȷ��
				lwip_dev.dhcp_status = 3;
			break;
			
			case 0xFF:
					
			break;
			
			
			default: 
			break;
		}
		

		OSTimeDly(250, OS_OPT_TIME_DLY, &err);
	}

	
	
	dhcp_task_delete();//ɾ��DHCP���� 			
}


void dhcp_task_delete(void)
{
	OS_ERR err;
	
	dhcp_stop(&lwip_netif); 		//�ر�DHCP
	OSTaskDel(&dhcp_task_TCB, &err);//ɾ��DHCP����
}



