





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




			

		






/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 LAN8720_Init(void)
{
	OS_ERR err;
	u8 rval=0;
	
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); //MAC和PHY之间使用RMII接口
	 
	ETH_RESET_L();
	timer_delay_1ms(20);
	ETH_RESET_H();
	
	BSP_IntVectSet(BSP_INT_ID_ETH, ETH_IRQHandler);
	
	rval=ETH_MACDMA_Config();		//配置MAC及DMA
	return !rval;	
	
}


/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
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




/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 LAN8720_Get_Speed(void)
{
	u8 speed = 0;
	
	speed=((ETH_ReadPHYRegister(0x00,31)&0x1C)>>2); //从LAN8720的31号寄存器中读取网络速度和双工模式
	USART_OUT(USART3, "speed=%d\r", speed);
	return speed;
}



extern void lwip_comm_pkt_handle(void);
//以太网DMA接收中断服务函数

/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void ETH_IRQHandler(void)
{
	while(ETH_GetRxPktSize(DMARxDescToGet)!=0) 	//检测是否收到数据包
	{ 
		lwip_comm_pkt_handle();		
	}
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R); 	//清除DMA中断标志位
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);	//清除DMA接收中断标志位
}  



//接收一个网卡数据包
//返回值:网络数据包帧结构体
/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
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
/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
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
/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
u32 ETH_GetCurrentTxBuffer(void)
{  
	return DMATxDescToSet->Buffer1Addr;//返回Tx buffer地址  
}


/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void lan8720_rx_packet(FrameTypeDef frame)
{
	
	frame.descriptor->Status=ETH_DMARxDesc_OWN;//设置Rx描述符OWN位,buffer重归ETH DMA 
	if((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)//当Rx Buffer不可用位(RBUS)被设置的时候,重置它.恢复传输
	{ 
		ETH->DMASR=ETH_DMASR_RBUS;//重置ETH DMA RBUS位 
		ETH->DMARPDR=0;//恢复DMA接收
	}
}
/*
*********************************************************************************************************
*                                          timer3_init()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
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


/*
*********************************************************************************************************
*                                          lan8720_txDMA_config()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
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

/*
*********************************************************************************************************
*                                          lan8720_write_buff()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count ?????? 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void lan8720_write_buff(u8 *p, u16 len)
{
	
	memcpy((u8*)DMATxDescToSet->Buffer1Addr, p, len);
}








