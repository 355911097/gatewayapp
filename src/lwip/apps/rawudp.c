#include "udp_demo.h" 
#include "usart.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
#include "timer.h"
#include "lwip/ip4_addr.h"
#include "lan8720.h"
 
 
 
 extern u32 lwip_localtime;	
 extern lwip_dev_t lwip_dev;

struct udp_pcb *udppcb;  	//����һ��TCP���������ƿ�

OS_TCB 	rawudp_task_TCB;
CPU_STK	rawudp_task_stk[RAWUDP_TASK_STK_SIZE];
#define RAWUDP_TASK_PRIO		5



u8 rawudp_recv_buff[RAWUDP_RX_BUFSIZE];	//UDP�������ݻ����� 
u8 rawudp_send_buff[50];		//UDP������������

u32 message_count = 0;









static void rawudp_task_fun(void *p_arg);





static void rawudp_task_fun(void *p_arg)
{
	OS_ERR err;

	struct ip4_addr rmtipaddr;  	//Զ��ip��ַ
 	
	
	udppcb = udp_new();

	if( udppcb )//�����ɹ�
	{		
		IP4_ADDR(&rmtipaddr, lwip_dev.remote_ip[0], lwip_dev.remote_ip[1], lwip_dev.remote_ip[2], lwip_dev.remote_ip[3]);
		USART_OUT(USART3,   "Զ��IP........................%d.%d.%d.%d\r\n", lwip_dev.remote_ip[0], lwip_dev.remote_ip[1], lwip_dev.remote_ip[2], lwip_dev.remote_ip[3]);
		err = udp_connect(udppcb, &rmtipaddr, 16650);//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err == ERR_OK)
		{
			err = udp_bind(udppcb, IP_ADDR_ANY, 16650);//�󶨱���IP��ַ��˿ں�
			if(err == ERR_OK)	//�����
			{						
				udp_recv(udppcb, rawudp_recv, NULL);//ע����ջص����� 
			}
		}		
	}
//	udp_demo_senddata(udppcb);	
	rawudp_send_data(udppcb, "www");
	while(DEF_TRUE)
	{
		
		OSTimeDly(250, OS_OPT_TIME_DLY, &err);
	}


}





void rawudp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	LWIP_UNUSED_ARG(arg);
	
	if(p != NULL)	//���յ���Ϊ�յ�����ʱ
	{
		memset(rawudp_recv_buff, 0, RAWUDP_RX_BUFSIZE);  //���ݽ��ջ���������
		
		for(q=p; q!=NULL; q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (RAWUDP_RX_BUFSIZE - data_len)) 
			{	
				memcpy(rawudp_recv_buff+data_len,q->payload,(RAWUDP_RX_BUFSIZE-data_len));//��������
			}
			else
			{
				memcpy(rawudp_recv_buff+data_len,q->payload,q->len);
			}
			data_len += q->len;  
			USART_OUT(USART3, rawudp_recv_buff);			
			if(data_len > RAWUDP_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		
		upcb->remote_ip=*addr; 				//��¼Զ��������eIP��ַ
		upcb->remote_port=port;  			//��¼Զ�������Ķ˿ں�
		lwip_dev.remote_ip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwip_dev.remote_ip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwip_dev.remote_ip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwip_dev.remote_ip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		pbuf_free(p);//�ͷ��ڴ�
	}else
	{
		udp_disconnect(upcb); 
	
	} 
} 

//UDP��������������
//void udp_demo_senddata(struct udp_pcb *upcb)
//{
//	struct pbuf *ptr;
//	sprintf((char*)data, "sending udp client message %d\r", (int*)lwip_localtime);
//	ptr=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)data), PBUF_POOL); //�����ڴ�
//	if(ptr)
//	{
//		pbuf_take(ptr, (char*)data, strlen((char*)data)); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
//		udp_send(upcb, ptr);	//udp�������� 
//		pbuf_free(ptr);//�ͷ��ڴ�
//	} 
//} 
void rawudp_send_data(struct udp_pcb *upcb, u8 *pdata)
{
	struct pbuf *ptr;
	
	sprintf((char*)rawudp_send_buff, "%s\r", (char*)pdata);
	ptr=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)rawudp_send_buff), PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr, (char*)rawudp_send_buff, strlen((char*)rawudp_send_buff)); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
		udp_send(upcb, ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	} 
} 
//�ر�UDP����
void rawudp_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);			//�Ͽ�UDP���� 

}



void rawudp_task_create(void)
{
	OS_ERR os_err;
	CPU_SR_ALLOC();

	
	CPU_CRITICAL_ENTER();
	OSTaskCreate((OS_TCB 	* )&rawudp_task_TCB,		
				 (CPU_CHAR	* )"rawudp_task", 		
                 (OS_TASK_PTR )rawudp_task_fun, 			
                 (void		* )0,					
                 (OS_PRIO	  )RAWUDP_TASK_PRIO,     
                 (CPU_STK   * )&rawudp_task_stk[0],	
                 (CPU_STK_SIZE)RAWUDP_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)RAWUDP_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err); 
				 
	CPU_CRITICAL_EXIT();
				 
	OSTimeDly(10, OS_OPT_TIME_DLY, &os_err);
	USART_OUT(USART3, "\rrawudp_task err=%d\r", os_err);	

}

























