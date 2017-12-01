#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H

#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"






#define RAWUDP_RX_LENGHT		512	//定义udp最大接收数据长度 
#define RAWUDP_PORT				16650  //8089	//定义udp连接的端口 

 

void rawudp_recv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void rawudp_send_data(struct udp_pcb *upcb, u8 *pdata, u16 pdata_size);
void rawudp_connection_close(struct udp_pcb *upcb);
void rawudp_task_create(void);



#endif

