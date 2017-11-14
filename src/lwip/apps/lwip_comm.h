

#ifndef __LWIP_COMM_H_
#define __LWIP_COMM_H_
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <stm32f2xx.h>

typedef struct 
{
	u8 mac[6];
	u8 remote_ip[4];
	u8 ip[4];
	u8 netmask[4];
	u8 gateway[4];
	u8 dhcp_status;
	
}lwip_dev_t;


extern lwip_dev_t lwip_dev;



#define LWIP_MAX_DHCP_TRIES		10   //DHCP服务器最大重试次数



void lwip_comm_default_ip_set(lwip_dev_t *lwipx);
u8 lwip_comm_dev_init(void);
void lwip_comm_pkt_handle(void);




#endif
