





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
	
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); //MAC��PHY֮��ʹ��RMII�ӿ�
	 
	ETH_RESET_L();
	timer_delay_1ms(20);
	ETH_RESET_H();
	
	BSP_IntVectSet(BSP_INT_ID_ETH, ETH_IRQHandler);
	
	rval=ETH_MACDMA_Config();		//����MAC��DMA
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
	
	//ʹ����̫��MAC�Լ�MAC���պͷ���ʱ��
                        
	ETH_DeInit();  								//AHB����������̫��
	ETH_SoftwareReset();  						//�����������
	while (ETH_GetSoftwareResetStatus() == SET);//�ȴ��������������� 
	ETH_StructInit(&ETH_InitStructure); 	 	//��ʼ������ΪĬ��ֵ  

	///����MAC�������� 
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;   			//������������Ӧ����
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;					//�رշ���
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		//�ر��ش�����
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 	//�ر��Զ�ȥ��PDA/CRC���� 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;						//�رս������е�֡
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;//����������й㲥֡
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			//�رջ��ģʽ�ĵ�ַ����  
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//�����鲥��ַʹ��������ַ����   
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;	//�Ե�����ַʹ��������ַ���� 
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			//����ipv4��TCP/UDP/ICMP��֡У���ж��   
#endif
	//������ʹ��֡У���ж�ع��ܵ�ʱ��һ��Ҫʹ�ܴ洢ת��ģʽ,�洢ת��ģʽ��Ҫ��֤����֡�洢��FIFO��,
	//����MAC�ܲ���/ʶ���֡У��ֵ,����У����ȷ��ʱ��DMA�Ϳ��Դ���֡,����Ͷ�������֡
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; //��������TCP/IP����֡
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;     //�����������ݵĴ洢ת��ģʽ    
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;   //�����������ݵĴ洢ת��ģʽ  

	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Disable;            //?????????
    ETH_InitStructure.ETH_TransmitThresholdControl = ETH_TransmitThresholdControl_64Bytes;    //?????64Bytes
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;                //??FIFO???????


	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;     	//��ֹת������֡  
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	//��ת����С�ĺ�֡ 
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;  		//�򿪴���ڶ�֡����
	
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;  	//����DMA����ĵ�ַ���빦��
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;            			//�����̶�ͻ������    
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;     		//DMA���͵����ͻ������Ϊ32������   
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;			//DMA���յ����ͻ������Ϊ32������
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
	rval=ETH_Init(&ETH_InitStructure,LAN8720_PHY_ADDRESS);		//����ETH
	if(rval==ETH_SUCCESS)//���óɹ�
	{
		ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);  	//ʹ����̫�������ж�	
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
	
	speed=((ETH_ReadPHYRegister(0x00,31)&0x1C)>>2); //��LAN8720��31�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ
	USART_OUT(USART3, "speed=%d\r", speed);
	return speed;
}



extern void lwip_comm_pkt_handle(void);
//��̫��DMA�����жϷ�����

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
	while(ETH_GetRxPktSize(DMARxDescToGet)!=0) 	//����Ƿ��յ����ݰ�
	{ 
		lwip_comm_pkt_handle();		
	}
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R); 	//���DMA�жϱ�־λ
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);	//���DMA�����жϱ�־λ
}  



//����һ���������ݰ�
//����ֵ:�������ݰ�֡�ṹ��
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
	//��鵱ǰ������,�Ƿ�����ETHERNET DMA(���õ�ʱ��)/CPU(��λ��ʱ��)
	if((DMARxDescToGet->Status&ETH_DMARxDesc_OWN)!=(u32)RESET)
	{	
		frame.length=ETH_ERROR; 
		if ((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)  
		{ 
			ETH->DMASR = ETH_DMASR_RBUS;//���ETH DMA��RBUSλ 
			ETH->DMARPDR=0;//�ָ�DMA����
		}
		return frame;//����,OWNλ��������
	}  
	if(((DMARxDescToGet->Status&ETH_DMARxDesc_ES)==(u32)RESET)&& 
	((DMARxDescToGet->Status & ETH_DMARxDesc_LS)!=(u32)RESET)&&  
	((DMARxDescToGet->Status & ETH_DMARxDesc_FS)!=(u32)RESET))  
	{       
		framelength=((DMARxDescToGet->Status&ETH_DMARxDesc_FL)>>ETH_DMARxDesc_FrameLengthShift)-4;//�õ����հ�֡����(������4�ֽ�CRC)
 		frame.buffer = DMARxDescToGet->Buffer1Addr;//�õ����������ڵ�λ��
	}else framelength=ETH_ERROR;//����  
	frame.length=framelength; 
	frame.descriptor=DMARxDescToGet;  
	//����ETH DMAȫ��Rx������Ϊ��һ��Rx������
	//Ϊ��һ��buffer��ȡ������һ��DMA Rx������
	DMARxDescToGet=(ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr);   
	return frame;  
}





//����һ���������ݰ�
//FrameLength:���ݰ�����
//����ֵ:ETH_ERROR,����ʧ��(0)
//		ETH_SUCCESS,���ͳɹ�(1)
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
	 
	//��鵱ǰ������,�Ƿ�����ETHERNET DMA(���õ�ʱ��)/CPU(��λ��ʱ��)
	if((DMATxDescToSet->Status&ETH_DMATxDesc_OWN) != (u32)RESET) 
		return ETH_ERROR;//����,OWNλ�������� 
 	DMATxDescToSet->ControlBufferSize = (FrameLength&ETH_DMATxDesc_TBS1);//����֡����,bits[12:0]
	DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;//�������һ���͵�һ��λ����λ(1������������һ֡)
  	DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;//����Tx��������OWNλ,buffer�ع�ETH DMA
	if((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)//��Tx Buffer������λ(TBUS)�����õ�ʱ��,������.�ָ�����
	{ 
		ETH->DMASR = ETH_DMASR_TBUS;//����ETH DMA TBUSλ 
		ETH->DMATPDR = 0;//�ָ�DMA����
	}

	//����ETH DMAȫ��Tx������Ϊ��һ��Tx������
	//Ϊ��һ��buffer����������һ��DMA Tx������ 
	DMATxDescToSet = (ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);  
	
	return ETH_SUCCESS;   
}



//�õ���ǰ��������Tx buffer��ַ
//����ֵ:Tx buffer��ַ
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
	return DMATxDescToSet->Buffer1Addr;//����Tx buffer��ַ  
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
	
	frame.descriptor->Status=ETH_DMARxDesc_OWN;//����Rx������OWNλ,buffer�ع�ETH DMA 
	if((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)//��Rx Buffer������λ(RBUS)�����õ�ʱ��,������.�ָ�����
	{ 
		ETH->DMASR=ETH_DMASR_RBUS;//����ETH DMA RBUSλ 
		ETH->DMARPDR=0;//�ָ�DMA����
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
	//��鵱ǰ������,�Ƿ�����ETHERNET DMA(���õ�ʱ��)/CPU(��λ��ʱ��)
	if((DMARxDescToGet->Status&ETH_DMARxDesc_OWN)!=(u32)RESET)
	{	
		frame.length=ETH_ERROR; 
		if ((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)  
		{ 
			ETH->DMASR = ETH_DMASR_RBUS;//���ETH DMA��RBUSλ 
			ETH->DMARPDR=0;//�ָ�DMA����
		}
		return frame;//����,OWNλ��������
	}  
	if(((DMARxDescToGet->Status&ETH_DMARxDesc_ES)==(u32)RESET)&& 
	((DMARxDescToGet->Status & ETH_DMARxDesc_LS)!=(u32)RESET)&&  
	((DMARxDescToGet->Status & ETH_DMARxDesc_FS)!=(u32)RESET))  
	{       
		frame_len=((DMARxDescToGet->Status&ETH_DMARxDesc_FL)>>ETH_DMARxDesc_FrameLengthShift)-4;//�õ����հ�֡����(������4�ֽ�CRC)
 		frame.buffer = DMARxDescToGet->Buffer1Addr;//�õ����������ڵ�λ��
	}else frame_len=ETH_ERROR;//����  
	frame.length=frame_len; 
	frame.descriptor=DMARxDescToGet;  
	//����ETH DMAȫ��Rx������Ϊ��һ��Rx������
	//Ϊ��һ��buffer��ȡ������һ��DMA Rx������
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
		return ETH_ERROR;//����,OWNλ�������� 
	
	DMATxDescToSet->ControlBufferSize = (frame_len & ETH_DMATxDesc_TBS1);//����֡����,bits[12:0]
	DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;//�������һ���͵�һ��λ����λ(1������������һ֡)
	DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;//����Tx��������OWNλ,buffer�ع�ETH DMA
		if((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)//��Tx Buffer������λ(TBUS)�����õ�ʱ��,������.�ָ�����
	{ 
		ETH->DMASR = ETH_DMASR_TBUS;//����ETH DMA TBUSλ 
		ETH->DMATPDR = 0;//�ָ�DMA����
	}
	
	//����ETH DMAȫ��Tx������Ϊ��һ��Tx������
	//Ϊ��һ��buffer����������һ��DMA Tx������ 
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








