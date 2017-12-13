


#include "rawudp.h" 
#include "usart.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
#include "timer.h"
#include "lwip/ip4_addr.h"
#include "lan8720.h"
#include "lwip_comm.h"
#include "app.h"
#include "protocol.h"




extern u8 protocol_buff[PROTOCOL_BUFF_LENGHT];
extern u32 protocol_buff_len;

struct udp_pcb *udppcb;  	//����һ��UDP���������ƿ�


OS_TCB 	rawudp_task_TCB;
CPU_STK	rawudp_task_stk[RAWUDP_TASK_STK_SIZE];



u8 rawudp_recv_buff[RAWUDP_RX_LENGHT];		//UDP�������ݻ����� 
u8 rawudp_send_buff[RAWUDP_RX_LENGHT];		//UDP������������
u32 rawudp_recv_buff_len = 0;				//UDP�������ݵĳ���




u8 eth_rx_flag = 0;

static void rawudp_task_fun(void *p_arg);


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
	
	if(os_err != OS_ERR_NONE)
	{
		USART_OUT(USART3, "\rrawudp_task fail\r");			 
	}			 

}


static void rawudp_task_fun(void *p_arg)
{
	OS_ERR err;

	struct ip4_addr rmtipaddr;  	//Զ��ip��ַ
 	
	
	udppcb = udp_new();

	if( udppcb )//�����ɹ�
	{		
		IP4_ADDR(&rmtipaddr, lwip_dev.remote_ip[0], lwip_dev.remote_ip[1], lwip_dev.remote_ip[2], lwip_dev.remote_ip[3]);
		USART_OUT(USART3,   "Զ��IP........................%d.%d.%d.%d\r\n", lwip_dev.remote_ip[0], lwip_dev.remote_ip[1], lwip_dev.remote_ip[2], lwip_dev.remote_ip[3]);
		err = udp_connect(udppcb, &rmtipaddr, 16650);			//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err == ERR_OK)
		{
			err = udp_bind(udppcb, IP_ADDR_ANY, 16650);			//�󶨱���IP��ַ��˿ں�
			if(err == ERR_OK)	//�����
			{						
				udp_recv(udppcb, rawudp_recv_callback, NULL);	//ע����ջص����� 
			}
		}		
	}
	
	rawudp_send_data(udppcb, "www", 3);
	protocol_task_create();
	while(DEF_TRUE)
	{
		
		OSTimeDly(250, OS_OPT_TIME_DLY, &err);
	}
	
}





void rawudp_recv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{

	struct pbuf *q;
	LWIP_UNUSED_ARG(arg);
	
	
	if(p != NULL)	//���յ���Ϊ�յ�����ʱ
	{
		
		memset(rawudp_recv_buff, 0, RAWUDP_RX_LENGHT);  //���ݽ��ջ���������
		rawudp_recv_buff_len = 0;						//���ݽ��ռ�������
		
		for(q=p; q!=NULL; q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (RAWUDP_RX_LENGHT - rawudp_recv_buff_len)) 
			{	
				memcpy(rawudp_recv_buff+rawudp_recv_buff_len,q->payload,(RAWUDP_RX_LENGHT-rawudp_recv_buff_len));//��������
			}
			else
			{
				memcpy(rawudp_recv_buff+rawudp_recv_buff_len,q->payload,q->len);
			}
			rawudp_recv_buff_len += q->len;  
				
			if(rawudp_recv_buff_len > RAWUDP_RX_LENGHT) break; //����TCP�ͻ��˽�������,����
			
			memcpy(protocol_buff, rawudp_recv_buff, rawudp_recv_buff_len);	//����̫�����յ����ݸ��Ƶ�Э��buff
			protocol_buff_len = rawudp_recv_buff_len;
			
			eth_rx_flag = 1; 												//���յ����ݱ�־λ
			
			memset(rawudp_recv_buff, 0, RAWUDP_RX_LENGHT);  //����ʹ��������ݽ��ջ���������
			rawudp_recv_buff_len = 0;				
			
		}
		
		upcb->remote_ip = *addr; 				//��¼Զ��������eIP��ַ
		upcb->remote_port = port;  				//��¼Զ�������Ķ˿ں�
		lwip_dev.remote_ip[0] = upcb->remote_ip.addr&0xff; 		//IADDR4
		lwip_dev.remote_ip[1] = (upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwip_dev.remote_ip[2] = (upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwip_dev.remote_ip[3] = (upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		pbuf_free(p);//�ͷ��ڴ�
	}
	else
	{
		udp_disconnect(upcb); 
	} 
} 


void rawudp_send_data(struct udp_pcb *upcb, u8 *pdata, u16 pdata_size)
{
	struct pbuf *ptr;

	memcpy(rawudp_send_buff, pdata, pdata_size);

	ptr=pbuf_alloc(PBUF_TRANSPORT, pdata_size, PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr, (char*)rawudp_send_buff, pdata_size); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
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




























