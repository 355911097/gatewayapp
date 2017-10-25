#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H

#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"








#define UDP_DEMO_RX_BUFSIZE		2000	//定义udp最大接收数据长度 
#define UDP_DEMO_PORT			16650  //8089	//定义udp连接的端口 

 
struct udp_pcb *udp_demo_init(void);
void udp_demo_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void udp_demo_senddata(struct udp_pcb *upcb);
void udp_demo_connection_close(struct udp_pcb *upcb);

#endif

