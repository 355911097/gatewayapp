#include "udp_demo.h" 
#include "usart.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
#include "timer.h"
#include "lwip/ip4_addr.h"
#include "lan8720.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//UDP ���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
 
 

 extern u32 lwip_localtime;
 extern lwip_dev_t lwip_dev;
 





//UDP�������ݻ�����
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ����� 
//UDP������������
u8 data[50];
u32 message_count = 0;

//UDP ����ȫ��״̬��Ǳ���
//bit7:û���õ�
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û��������;1,��������.
//bit4~0:����
u8 udp_demo_flag;



//UDP����
struct udp_pcb *udp_demo_init(void)
{
 	err_t err;
	struct udp_pcb *udppcb;  	//����һ��TCP���������ƿ�
	struct ip4_addr rmtipaddr;  	//Զ��ip��ַ
 	
	u8 *tbuf;
 	u8 key;
	u8 res=0;		
	u8 t=0; 
 	
	
	udppcb = udp_new();
	if(udppcb)//�����ɹ�
	{		
		IP4_ADDR(&rmtipaddr,lwip_dev.remote_ip[0],lwip_dev.remote_ip[1],lwip_dev.remote_ip[2],lwip_dev.remote_ip[3]);
		USART_OUT(USART3,   "Զ��IP........................%d.%d.%d.%d\r\n",lwip_dev.remote_ip[0],lwip_dev.remote_ip[1],lwip_dev.remote_ip[2],lwip_dev.remote_ip[3]);
		err=udp_connect(udppcb, &rmtipaddr, 16650);//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,16650);//�󶨱���IP��ַ��˿ں�
			if(err==ERR_OK)	//�����
			{						
				udp_recv(udppcb, udp_demo_recv, NULL);//ע����ջص����� 
			}
		}		
	}
	
	return udppcb;
} 

//UDP�ص�����
void udp_demo_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	LWIP_UNUSED_ARG(arg);
	
	if(p != NULL)	//���յ���Ϊ�յ�����ʱ
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
		
		for(q=p; q!=NULL; q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (UDP_DEMO_RX_BUFSIZE - data_len)) 
			{	
				memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
			}
			else
			{
				memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			}
			data_len += q->len;  
			USART_OUT(USART3, udp_demo_recvbuf);			
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		
		upcb->remote_ip=*addr; 				//��¼Զ��������IP��ַ
		upcb->remote_port=port;  			//��¼Զ�������Ķ˿ں�
		lwip_dev.remote_ip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwip_dev.remote_ip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwip_dev.remote_ip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwip_dev.remote_ip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		pbuf_free(p);//�ͷ��ڴ�
	}else
	{
		udp_disconnect(upcb); 
	
		udp_demo_flag &= ~(1<<5);	//������ӶϿ�
	} 
} 
//UDP��������������
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	sprintf((char*)data, "sending udp client message %d\r", (int*)lwip_localtime);
	ptr=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)data), PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr, (char*)data, strlen((char*)data)); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
		udp_send(upcb, ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	} 
} 
//�ر�UDP����
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);			//�Ͽ�UDP���� 

}

























