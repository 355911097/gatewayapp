





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
u32 DHCPfineTimer=0;	//DHCP��ϸ�����ʱ��
u32 DHCPcoarseTimer=0;	//DHCP�ֲڴ����ʱ��
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
  

#if LWIP_DHCP //���ʹ��DHCP�Ļ�
  //ÿ500ms����һ��dhcp_fine_tmr()
	if (lwip_localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
	{
		DHCPfineTimer =  lwip_localtime;
		dhcp_fine_tmr();
		if ((lwip_dev.dhcp_status != 2)&&(lwip_dev.dhcp_status != 0XFF))
		{ 
			lwip_dhcp_process_handle();  //DHCP����
		}
	}

  //ÿ60sִ��һ��DHCP�ֲڴ���
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
	
	OS_CRITICAL_ENTER();  //�����ٽ���
	Netif_Init_Flag = netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
//	Netif_Init_Flag = netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
	OS_CRITICAL_EXIT();  //�˳��ٽ���

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
	lwip_dev.dhcp_status = 0;	//DHCP���Ϊ0
	return 0;
}



#if LWIP_DHCP

void lwip_dhcp_process_handle(void)
{
	u32 ip=0, netmask=0, gw=0;
	struct dhcp *pdhcp;

	switch(lwip_dev.dhcp_status)
	{
		case 0: 	//����DHCP
			dhcp_start(&lwip_netif);
			lwip_dev.dhcp_status = 1;		//�ȴ�ͨ��DHCP��ȡ���ĵ�ַ
			USART_OUT(USART3, "���ڲ���DHCP������,���Ե�...........\r\n");  
		break;
		case 1:		//�ȴ���ȡ��IP��ַ
			ip=lwip_netif.ip_addr.addr;		//��ȡ��IP��ַ
			netmask=lwip_netif.netmask.addr;//��ȡ��������
			gw=lwip_netif.gw.addr;			//��ȡĬ������ 
			
			if(ip != 0)			//��ȷ��ȡ��IP��ַ��ʱ��
			{
				lwip_dev.dhcp_status = 2;	//DHCP�ɹ�
				USART_OUT(USART3, "����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwip_dev.mac[0],lwip_dev.mac[1],lwip_dev.mac[2],lwip_dev.mac[3],lwip_dev.mac[4],lwip_dev.mac[5]);
				//������ͨ��DHCP��ȡ����IP��ַ
				lwip_dev.ip[3]=(uint8_t)(ip>>24); 
				lwip_dev.ip[2]=(uint8_t)(ip>>16);
				lwip_dev.ip[1]=(uint8_t)(ip>>8);
				lwip_dev.ip[0]=(uint8_t)(ip);
				USART_OUT(USART3, "ͨ��DHCP��ȡ��IP��ַ..............%d.%d.%d.%d\r\n",lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
				//����ͨ��DHCP��ȡ�������������ַ
				lwip_dev.netmask[3]=(uint8_t)(netmask>>24);
				lwip_dev.netmask[2]=(uint8_t)(netmask>>16);
				lwip_dev.netmask[1]=(uint8_t)(netmask>>8);
				lwip_dev.netmask[0]=(uint8_t)(netmask);
				USART_OUT(USART3, "ͨ��DHCP��ȡ����������............%d.%d.%d.%d\r\n",lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
				//������ͨ��DHCP��ȡ����Ĭ������	
				lwip_dev.gateway[3]=(uint8_t)(gw>>24);
				lwip_dev.gateway[2]=(uint8_t)(gw>>16);
				lwip_dev.gateway[1]=(uint8_t)(gw>>8);
				lwip_dev.gateway[0]=(uint8_t)(gw);
				USART_OUT(USART3, "ͨ��DHCP��ȡ����Ĭ������..........%d.%d.%d.%d\r\n",lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);
			}
			else if(pdhcp->tries > LWIP_MAX_DHCP_TRIES) //ͨ��DHCP�����ȡIP��ַʧ��,�ҳ�������Դ���
			{
				lwip_dev.dhcp_status=0XFF;//DHCP��ʱʧ��.
				//ʹ�þ�̬IP��ַ
				IP4_ADDR(&(lwip_netif.ip_addr),lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
				IP4_ADDR(&(lwip_netif.netmask),lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
				IP4_ADDR(&(lwip_netif.gw),lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);
				USART_OUT(USART3, "DHCP����ʱ,ʹ�þ�̬IP��ַ!\r\n");
				USART_OUT(USART3, "����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwip_dev.mac[0],lwip_dev.mac[1],lwip_dev.mac[2],lwip_dev.mac[3],lwip_dev.mac[4],lwip_dev.mac[5]);
				USART_OUT(USART3, "��̬IP��ַ........................%d.%d.%d.%d\r\n",lwip_dev.ip[0],lwip_dev.ip[1],lwip_dev.ip[2],lwip_dev.ip[3]);
				USART_OUT(USART3, "��������..........................%d.%d.%d.%d\r\n",lwip_dev.netmask[0],lwip_dev.netmask[1],lwip_dev.netmask[2],lwip_dev.netmask[3]);
				USART_OUT(USART3, "Ĭ������..........................%d.%d.%d.%d\r\n",lwip_dev.gateway[0],lwip_dev.gateway[1],lwip_dev.gateway[2],lwip_dev.gateway[3]);
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
	
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); //MAC��PHY֮��ʹ��RMII�ӿ�
	 
	ETH_RESET_L();
	timer_delay_1ms(50);
	ETH_RESET_H();
	
	BSP_IntVectSet(BSP_INT_ID_ETH, ETH_IRQHandler);
	
	rval=ETH_MACDMA_Config();		//����MAC��DMA
	return !rval;	
	
}



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





u8 LAN8720_Get_Speed(void)
{
	u8 speed = 0;
	
	speed=((ETH_ReadPHYRegister(0x00,31)&0x1C)>>2); //��LAN8720��31�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ
	USART_OUT(USART3, "speed=%d\r", speed);
	return speed;
}



extern void lwip_pkt_handle(void);
//��̫��DMA�����жϷ�����
void ETH_IRQHandler(void)
{
	while(ETH_GetRxPktSize(DMARxDescToGet)!=0) 	//����Ƿ��յ����ݰ�
	{ 
		lwip_pkt_handle();		
	}
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R); 	//���DMA�жϱ�־λ
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);	//���DMA�����жϱ�־λ
}  



//����һ���������ݰ�
//����ֵ:�������ݰ�֡�ṹ��
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
u32 ETH_GetCurrentTxBuffer(void)
{  
	return DMATxDescToSet->Buffer1Addr;//����Tx buffer��ַ  
}



void lan8720_rx_packet(FrameTypeDef frame)
{
	
	frame.descriptor->Status=ETH_DMARxDesc_OWN;//����Rx������OWNλ,buffer�ع�ETH DMA 
	if((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)//��Rx Buffer������λ(RBUS)�����õ�ʱ��,������.�ָ�����
	{ 
		ETH->DMASR=ETH_DMASR_RBUS;//����ETH DMA RBUSλ 
		ETH->DMARPDR=0;//�ָ�DMA����
	}
}

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


void lan8720_write_buff(u8 *p, u16 len)
{
	
	memcpy((u8*)DMATxDescToSet->Buffer1Addr, p, len);
}








