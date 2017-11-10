





/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "bsp.h"
#include "timer.h"
#include "lan8720.h"
#include "usart.h"
#include "string.h"
/////////////////////////////
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

u32 TCPTimer=0;		
u32 ARPTimer=0;			
u32 lwip_localtime;		//lwip???????,??:ms
#if LWIP_DHCP
u32 DHCPfineTimer=0;	//DHCP精细处理计时器
u32 DHCPcoarseTimer=0;	//DHCP粗糙处理计时器
#endif



//
void lwip_pkt_handle(void)
{
	ethernetif_input(&lwip_netif);	
}


void lwip_periodic_handle(void)
{
	
#if LWIP_TCP
	//
	if (lwip_localtime - TCPTimer >= TCP_TMR_INTERVAL)
	{
		TCPTimer =  lwip_localtime;
		tcp_tmr();
	}
#endif
  //ARP
	if ((lwip_localtime - ARPTimer) >= ARP_TMR_INTERVAL)
	{
		ARPTimer =  lwip_localtime;
		etharp_tmr();
	 
	}
  

#if LWIP_DHCP //如果使用DHCP的话
  //每500ms调用一次dhcp_fine_tmr()
	if (lwip_localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
	{
		DHCPfineTimer =  lwip_localtime;
		dhcp_fine_tmr();
		if ((lwip_dev.dhcp_status != 2)&&(lwip_dev.dhcp_status != 0XFF))
		{ 
			lwip_dhcp_process_handle();  //DHCP处理
		}
	}

  //每60s执行一次DHCP粗糙处理
	if (lwip_localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
	{
		DHCPcoarseTimer =  lwip_localtime;
		dhcp_coarse_tmr();
	}  
#endif

  
}


void lwip_comm_default_ip_set(lwip_dev_t *lwipx)
{
	
	u32 sn0;
	sn0=*(vu32*)(0x1FFF7A10);//
		
	lwipx->mac[0]=2;//
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

u8 lwip_dev_init(void)
{
	CPU_SR_ALLOC();	
	struct netif *Netif_Init_Flag;
	struct ip4_addr ipaddr, netmask, gw;

	if(LAN8720_Init())
		return 2;	

	tcpip_init(NULL,NULL);	

	lwip_comm_default_ip_set(&lwip_dev);

#if LWIP_DHCP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;	
#else	
	IP4_ADDR(&ipaddr, lwip_dev.ip[0], lwip_dev.ip[1], lwip_dev.ip[2], lwip_dev.ip[3]); // 
    IP4_ADDR(&netmask, lwip_dev.netmask[0], lwip_dev.netmask[1], lwip_dev.netmask[2], lwip_dev.netmask[3]); // 
    IP4_ADDR(&gw, lwip_dev.gateway[0], lwip_dev.gateway[1], lwip_dev.gateway[2], lwip_dev.gateway[3]); // 
	USART_OUT(USART3, "\r??en?MAC???:................%d.%d.%d.%d.%d.%d\r\n",lwip_dev.mac[0],lwip_dev.mac[1],lwip_dev.mac[2],lwip_dev.mac[3],lwip_dev.mac[4],lwip_dev.mac[5]);
	USART_OUT(USART3, "??IP??........................%d.%d.%d.%d\r\n",lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
	USART_OUT(USART3, "????..........................%d.%d.%d.%d\r\n",lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
	USART_OUT(USART3, "????..........................%d.%d.%d.%d\r\n",lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);
#endif
	
	OS_CRITICAL_ENTER();  //进入临界区
	Netif_Init_Flag = netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
//	Netif_Init_Flag = netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
	OS_CRITICAL_EXIT();  //退出临界区

	if(Netif_Init_Flag == NULL)
	{
		return 3;
	}
	else	
	{
		netif_set_default(&lwip_netif);
		netif_set_up(&lwip_netif);
		
		USART_OUT(USART3, "netif_add OK\r");
	}
	lwip_dev.dhcp_status = 0;	//DHCP标记为0
	return 0;
}



#if LWIP_DHCP

void lwip_dhcp_process_handle(void)
{
	u32 ip=0, netmask=0, gw=0;
	struct dhcp *pdhcp;

	switch(lwip_dev.dhcp_status)
	{
		case 0: 	//开启DHCP
			dhcp_start(&lwip_netif);
			lwip_dev.dhcp_status = 1;		//等待通过DHCP获取到的地址
			USART_OUT(USART3, "正在查找DHCP服务器,请稍等...........\r\n");  
		break;
		case 1:		//等待获取到IP地址
			ip=lwip_netif.ip_addr.addr;		//读取新IP地址
			netmask=lwip_netif.netmask.addr;//读取子网掩码
			gw=lwip_netif.gw.addr;			//读取默认网关 
			
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
			}
			else if(pdhcp->tries > LWIP_MAX_DHCP_TRIES) //通过DHCP服务获取IP地址失败,且超过最大尝试次数
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
			}
		break;
		default: 
		break;
	}

}


#endif







u8 LAN8720_Init(void)
{
	OS_ERR err;
	u8 rval=0;
	
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); //MAC和PHY之间使用RMII接口
	 
	ETH_RESET_L();
	timer_delay_1ms(50);
	ETH_RESET_H();
	
	BSP_IntVectSet(BSP_INT_ID_ETH, ETH_IRQHandler);
	
	rval=ETH_MACDMA_Config();		//配置MAC及DMA
	return !rval;	
	
}



u8 ETH_MACDMA_Config(void)
{
	u8 rval;
	ETH_InitTypeDef ETH_InitStructure; 
	
	//使能以太网MAC以及MAC接收和发送时钟
                        
	ETH_DeInit();  								//AHB总线重启以太网
	ETH_SoftwareReset();  						//软件重启网络
	while (ETH_GetSoftwareResetStatus() == SET);//等待软件重启网络完成 
	ETH_StructInit(&ETH_InitStructure); 	 	//初始化网络为默认值  

	///网络MAC参数设置 
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;   			//开启网络自适应功能
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;					//关闭反馈
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		//关闭重传功能
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 	//关闭自动去除PDA/CRC功能 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;						//关闭接收所有的帧
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;//允许接收所有广播帧
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			//关闭混合模式的地址过滤  
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//对于组播地址使用完美地址过滤   
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;	//对单播地址使用完美地址过滤 
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			//开启ipv4和TCP/UDP/ICMP的帧校验和卸载   
#endif
	//当我们使用帧校验和卸载功能的时候，一定要使能存储转发模式,存储转发模式中要保证整个帧存储在FIFO中,
	//这样MAC能插入/识别出帧校验值,当真校验正确的时候DMA就可以处理帧,否则就丢弃掉该帧
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; //开启丢弃TCP/IP错误帧
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;     //开启接收数据的存储转发模式    
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;   //开启发送数据的存储转发模式  

	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Disable;            //?????????
    ETH_InitStructure.ETH_TransmitThresholdControl = ETH_TransmitThresholdControl_64Bytes;    //?????64Bytes
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;                //??FIFO???????


	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;     	//禁止转发错误帧  
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	//不转发过小的好帧 
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;  		//打开处理第二帧功能
	
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;  	//开启DMA传输的地址对齐功能
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;            			//开启固定突发功能    
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;     		//DMA发送的最大突发长度为32个节拍   
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;			//DMA接收的最大突发长度为32个节拍
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
	rval=ETH_Init(&ETH_InitStructure,LAN8720_PHY_ADDRESS);		//配置ETH
	if(rval==ETH_SUCCESS)//配置成功
	{
		ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);  	//使能以太网接收中断	
	}
	return rval;
	
}





u8 LAN8720_Get_Speed(void)
{
	u8 speed = 0;
	
	speed=((ETH_ReadPHYRegister(0x00,31)&0x1C)>>2); //从LAN8720的31号寄存器中读取网络速度和双工模式
	USART_OUT(USART3, "speed=%d\r", speed);
	return speed;
}



extern void lwip_pkt_handle(void);
//以太网DMA接收中断服务函数
void ETH_IRQHandler(void)
{
	while(ETH_GetRxPktSize(DMARxDescToGet)!=0) 	//检测是否收到数据包
	{ 
		lwip_pkt_handle();		
	}
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R); 	//清除DMA中断标志位
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);	//清除DMA接收中断标志位
}  



//接收一个网卡数据包
//返回值:网络数据包帧结构体
FrameTypeDef ETH_Rx_Packet(void)
{ 
	u32 framelength=0;
	FrameTypeDef frame={0,0};   
	//检查当前描述符,是否属于ETHERNET DMA(设置的时候)/CPU(复位的时候)
	if((DMARxDescToGet->Status&ETH_DMARxDesc_OWN)!=(u32)RESET)
	{	
		frame.length=ETH_ERROR; 
		if ((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)  
		{ 
			ETH->DMASR = ETH_DMASR_RBUS;//清除ETH DMA的RBUS位 
			ETH->DMARPDR=0;//恢复DMA接收
		}
		return frame;//错误,OWN位被设置了
	}  
	if(((DMARxDescToGet->Status&ETH_DMARxDesc_ES)==(u32)RESET)&& 
	((DMARxDescToGet->Status & ETH_DMARxDesc_LS)!=(u32)RESET)&&  
	((DMARxDescToGet->Status & ETH_DMARxDesc_FS)!=(u32)RESET))  
	{       
		framelength=((DMARxDescToGet->Status&ETH_DMARxDesc_FL)>>ETH_DMARxDesc_FrameLengthShift)-4;//得到接收包帧长度(不包含4字节CRC)
 		frame.buffer = DMARxDescToGet->Buffer1Addr;//得到包数据所在的位置
	}else framelength=ETH_ERROR;//错误  
	frame.length=framelength; 
	frame.descriptor=DMARxDescToGet;  
	//更新ETH DMA全局Rx描述符为下一个Rx描述符
	//为下一次buffer读取设置下一个DMA Rx描述符
	DMARxDescToGet=(ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
	return frame;  
}





//发送一个网卡数据包
//FrameLength:数据包长度
//返回值:ETH_ERROR,发送失败(0)
//		ETH_SUCCESS,发送成功(1)
u8 ETH_Tx_Packet(u16 FrameLength)
{   
	 
	//检查当前描述符,是否属于ETHERNET DMA(设置的时候)/CPU(复位的时候)
	if((DMATxDescToSet->Status&ETH_DMATxDesc_OWN) != (u32)RESET) 
		return ETH_ERROR;//错误,OWN位被设置了 
 	DMATxDescToSet->ControlBufferSize = (FrameLength&ETH_DMATxDesc_TBS1);//设置帧长度,bits[12:0]
	DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;//设置最后一个和第一个位段置位(1个描述符传输一帧)
  	DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;//设置Tx描述符的OWN位,buffer重归ETH DMA
	if((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)//当Tx Buffer不可用位(TBUS)被设置的时候,重置它.恢复传输
	{ 
		ETH->DMASR = ETH_DMASR_TBUS;//重置ETH DMA TBUS位 
		ETH->DMATPDR = 0;//恢复DMA发送
	}

	//更新ETH DMA全局Tx描述符为下一个Tx描述符
	//为下一次buffer发送设置下一个DMA Tx描述符 
	DMATxDescToSet = (ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);  
	
	return ETH_SUCCESS;   
}



//得到当前描述符的Tx buffer地址
//返回值:Tx buffer地址
u32 ETH_GetCurrentTxBuffer(void)
{  
	return DMATxDescToSet->Buffer1Addr;//返回Tx buffer地址  
}



void lan8720_rx_packet(FrameTypeDef frame)
{
	
	frame.descriptor->Status=ETH_DMARxDesc_OWN;//设置Rx描述符OWN位,buffer重归ETH DMA 
	if((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)//当Rx Buffer不可用位(RBUS)被设置的时候,重置它.恢复传输
	{ 
		ETH->DMASR=ETH_DMASR_RBUS;//重置ETH DMA RBUS位 
		ETH->DMARPDR=0;//恢复DMA接收
	}
}

FrameTypeDef lan8720_rxDMA_config(void)
{
	u32 frame_len = 0;
	FrameTypeDef frame = {0,0};   
	//检查当前描述符,是否属于ETHERNET DMA(设置的时候)/CPU(复位的时候)
	if((DMARxDescToGet->Status&ETH_DMARxDesc_OWN)!=(u32)RESET)
	{	
		frame.length=ETH_ERROR; 
		if ((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)  
		{ 
			ETH->DMASR = ETH_DMASR_RBUS;//清除ETH DMA的RBUS位 
			ETH->DMARPDR=0;//恢复DMA接收
		}
		return frame;//错误,OWN位被设置了
	}  
	if(((DMARxDescToGet->Status&ETH_DMARxDesc_ES)==(u32)RESET)&& 
	((DMARxDescToGet->Status & ETH_DMARxDesc_LS)!=(u32)RESET)&&  
	((DMARxDescToGet->Status & ETH_DMARxDesc_FS)!=(u32)RESET))  
	{       
		frame_len=((DMARxDescToGet->Status&ETH_DMARxDesc_FL)>>ETH_DMARxDesc_FrameLengthShift)-4;//得到接收包帧长度(不包含4字节CRC)
 		frame.buffer = DMARxDescToGet->Buffer1Addr;//得到包数据所在的位置
	}else frame_len=ETH_ERROR;//错误  
	frame.length=frame_len; 
	frame.descriptor=DMARxDescToGet;  
	//更新ETH DMA全局Rx描述符为下一个Rx描述符
	//为下一次buffer读取设置下一个DMA Rx描述符
	DMARxDescToGet=(ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
	return frame;  

}



u8 lan8720_txDMA_config(u16 frame_len)
{
	if((DMATxDescToSet->Status&ETH_DMATxDesc_OWN) != (u32)RESET) 
		return ETH_ERROR;//错误,OWN位被设置了 
	
	DMATxDescToSet->ControlBufferSize = (frame_len & ETH_DMATxDesc_TBS1);//设置帧长度,bits[12:0]
	DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;//设置最后一个和第一个位段置位(1个描述符传输一帧)
	DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;//设置Tx描述符的OWN位,buffer重归ETH DMA
		if((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)//当Tx Buffer不可用位(TBUS)被设置的时候,重置它.恢复传输
	{ 
		ETH->DMASR = ETH_DMASR_TBUS;//重置ETH DMA TBUS位 
		ETH->DMATPDR = 0;//恢复DMA发送
	}
	
	//更新ETH DMA全局Tx描述符为下一个Tx描述符
	//为下一次buffer发送设置下一个DMA Tx描述符 
	DMATxDescToSet = (ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);  
	
	return ETH_SUCCESS;  
}


//void lan8720_read_buff(u8 *p, u16 len)
//{
//	
//	memcpy(p, (u8*)DMARxDescToGet->Buffer1Addr, len);
//}


void lan8720_write_buff(u8 *p, u16 len)
{
	
	memcpy((u8*)DMATxDescToSet->Buffer1Addr, p, len);
}








