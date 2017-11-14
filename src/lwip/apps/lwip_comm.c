

#include "lwip_comm.h"
#include "lan8720.h"
#include "usart.h"
#include "netif/ethernet.h"
#include "lwip/init.h"
#include "lwip/etharp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/ip_addr.h"
#include "lwip/mem.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h" 




extern err_t ethernetif_init(struct netif *netif);
extern void ethernetif_input(struct netif *netif);
extern err_t ethernet_input(struct pbuf *p, struct netif *netif);


lwip_dev_t lwip_dev;
struct netif lwip_netif;	



void lwip_comm_default_ip_set(lwip_dev_t *lwipx)
{
	
	u32 sn0;
	sn0=*(vu32*)(0x1FFF7A10);//
		
	lwipx->mac[0]=2;		//
	lwipx->mac[1]=0;
	lwipx->mac[2]=0;
	lwipx->mac[3]=(sn0>>16)&0XFF;//
	lwipx->mac[4]=(sn0>>8)&0XFFF;;
	lwipx->mac[5]=sn0&0XFF; 
	
	lwipx->remote_ip[0] = 192;
	lwipx->remote_ip[1] = 168;
	lwipx->remote_ip[2] = 3;
	lwipx->remote_ip[3] = 130;

	lwipx->ip[0] = 192;
	lwipx->ip[1] = 168;
	lwipx->ip[2] = 3;
	lwipx->ip[3] = 133;
	
	lwipx->netmask[0] = 255;
	lwipx->netmask[1] = 255;
	lwipx->netmask[2] = 255;
	lwipx->netmask[3] = 0;

	lwipx->gateway[0] = 192;
	lwipx->gateway[1] = 168;
	lwipx->gateway[2] = 3;
	lwipx->gateway[3] = 1;	
	
	lwipx->dhcp_status = 0;	
}


u8 lwip_comm_dev_init(void)
{
	CPU_SR_ALLOC();	
	struct netif *netif_init_flag;
	struct ip4_addr ipaddr, netmask, gw;

	if(LAN8720_Init())
		return 2;	

	tcpip_init(NULL,NULL);	

	lwip_comm_default_ip_set(&lwip_dev);

#if LWIP_DHCP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
	lwip_dev.dhcp_status = 0;	//开启DHCP的时候置位
#else
	lwip_dev.dhcp_status = 2;	
	IP4_ADDR(&ipaddr, lwip_dev.ip[0], lwip_dev.ip[1], lwip_dev.ip[2], lwip_dev.ip[3]); // 
    IP4_ADDR(&netmask, lwip_dev.netmask[0], lwip_dev.netmask[1], lwip_dev.netmask[2], lwip_dev.netmask[3]); // 
    IP4_ADDR(&gw, lwip_dev.gateway[0], lwip_dev.gateway[1], lwip_dev.gateway[2], lwip_dev.gateway[3]); // 
	USART_OUT(USART3, "\r??en?MAC???:................%d.%d.%d.%d.%d.%d\r\n",lwip_dev.mac[0],lwip_dev.mac[1],lwip_dev.mac[2],lwip_dev.mac[3],lwip_dev.mac[4],lwip_dev.mac[5]);
	USART_OUT(USART3, "??IP??........................%d.%d.%d.%d\r\n",lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
	USART_OUT(USART3, "????..........................%d.%d.%d.%d\r\n",lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
	USART_OUT(USART3, "????..........................%d.%d.%d.%d\r\n",lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);
#endif
	
	OS_CRITICAL_ENTER();  //进入临界区
	netif_init_flag = netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
	OS_CRITICAL_EXIT();  //退出临界区

	if(netif_init_flag == NULL)
	{
		USART_OUT(USART3, "netif_add fail\r");
		return 3;
	}
	else	
	{
		netif_set_default(&lwip_netif);
		netif_set_up(&lwip_netif);
		
		USART_OUT(USART3, "netif_add OK\r");
	}
	return 0;
}




void lwip_comm_pkt_handle(void)
{
	ethernetif_input(&lwip_netif);	
}






