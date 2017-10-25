#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

//�߳����ȼ�
#ifndef TCPIP_THREAD_PRIO
#define TCPIP_THREAD_PRIO		5	//�����ں���������ȼ�Ϊ5
#endif
#undef  DEFAULT_THREAD_PRIO
#define DEFAULT_THREAD_PRIO		4

	

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT 1		//Ϊ1ʱʹ��ʵʱ����ϵͳ������������,�����ؼ����벻���жϴ��

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS 0 //ʹ��UCOS����ϵͳ
/**
 * NO_SYS_NO_TIMERS==1: Drop support for sys_timeout when NO_SYS==1
 * Mainly for compatibility to old versions.
 */
//#define NO_SYS_NO_TIMERS 0  // �����Ͳ���Ҫ����sys_now����



//#define LWIP_NETIF_LINK_CALLBACK        1

/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT 4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE (5 * 1024)

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF 10
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB 6
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB 10
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 6
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG 12
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
#define MEMP_NUM_SYS_TIMEOUT 8




/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE 10

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE 1500




/* ---------- TCP options ---------- */
#define LWIP_TCP 1
#define TCP_TTL 255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#undef TCP_QUEUE_OOSEQ
#define TCP_QUEUE_OOSEQ 0

#undef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE         	MAX_QUEUE_ENTRIES   //tcpip�������߳�ʱ����Ϣ�����С

#undef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE       MAX_QUEUE_ENTRIES  

#undef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE         MAX_QUEUE_ENTRIES  





/* TCP Maximum segment size. */
#define TCP_MSS (1500 - 40) /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF (2 * TCP_MSS)

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN (6 * TCP_SND_BUF) / TCP_MSS

/* TCP receive window. */
#define TCP_WND (2 * TCP_MSS)






/* ---------- ICMP options ---------- */
#define LWIP_ICMP 1

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work. */
#define LWIP_DHCP 1

//#define DHCP_DOES_ARP_CHECK 1	//wzy

#define LWIP_TCPIP_CORE_LOCKING_INPUT 1


/* ---------- UDP options ---------- */
#define LWIP_UDP 1
#define UDP_TTL 255






//֡У���ѡ�STM32F4x7����ͨ��Ӳ��ʶ��ͼ���IP,UDP��ICMP��֡У���
//#define CHECKSUM_BY_HARDWARE //����CHECKSUM_BY_HARDWARE,ʹ��Ӳ��֡У��
#ifdef CHECKSUM_BY_HARDWARE
  //CHECKSUM_GEN_IP==0: Ӳ������IP���ݰ���֡У���
  #define CHECKSUM_GEN_IP                 0
  //CHECKSUM_GEN_UDP==0: Ӳ������UDP���ݰ���֡У���
  #define CHECKSUM_GEN_UDP                0
  //CHECKSUM_GEN_TCP==0: Ӳ������TCP���ݰ���֡У���
  #define CHECKSUM_GEN_TCP                0 
  //CHECKSUM_CHECK_IP==0: Ӳ����������IP���ݰ�֡У���
  #define CHECKSUM_CHECK_IP               0
  //CHECKSUM_CHECK_UDP==0: Ӳ����������UDP���ݰ�֡У���
  #define CHECKSUM_CHECK_UDP              0
  //CHECKSUM_CHECK_TCP==0: Ӳ����������TCP���ݰ�֡У���
  #define CHECKSUM_CHECK_TCP              0
#else
  //CHECKSUM_GEN_IP==1: �������IP���ݰ�֡У���
  #define CHECKSUM_GEN_IP                 1
  // CHECKSUM_GEN_UDP==1: �������UDOP���ݰ�֡У���
  #define CHECKSUM_GEN_UDP                1
  //CHECKSUM_GEN_TCP==1: �������TCP���ݰ�֡У���
  #define CHECKSUM_GEN_TCP                1
  // CHECKSUM_CHECK_IP==1: �����������IP���ݰ�֡У���
  #define CHECKSUM_CHECK_IP               1
  // CHECKSUM_CHECK_UDP==1: �����������UDP���ݰ�֡У���
  #define CHECKSUM_CHECK_UDP              1
  //CHECKSUM_CHECK_TCP==1: �����������TCP���ݰ�֡У���
  #define CHECKSUM_CHECK_TCP              1
#endif

//#define LWIP_TCPIP_CORE_LOCKING 0	//wzy
#define LWIP_COMPAT_MUTEX_ALLOWED	//wzy

/* ---------- Statistics options ---------- */
#define LWIP_STATS 0
#define LWIP_PROVIDE_ERRNO 1

// LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
#define LWIP_NETCONN 1

// LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
#define LWIP_SOCKET 1

#define LWIP_COMPAT_MUTEX               1
#define LWIP_SO_RCVTIMEO                1 	//ͨ������LWIP_SO_RCVTIMEOʹ��netconn�ṹ����recv_timeout,ʹ��recv_timeout���Ա��������߳�


//�й�ϵͳ��ѡ��
#define TCPIP_THREAD_STACKSIZE          1000	//�ں������ջ��С
#define DEFAULT_UDP_RECVMBOX_SIZE       2000
#define DEFAULT_THREAD_STACKSIZE        512



// �������Ҫ����, ������ִ��lwip_init����ʱ���ڴ�������ʾ:
// Assertion "Struct packing not implemented correctly. Check your lwIP port." failed at line 345 in Library\lwip\core\init.c
#define PACK_STRUCT_BEGIN __packed







#endif /* __LWIPOPTS_H__ */