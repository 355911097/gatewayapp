



#ifndef __LAN8720_H_
#define __LAN8720_H_



/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <stm32f2xx.h>
#include "stm32f2x7_eth.h"

















#define LAN8720_PHY_ADDRESS  	0x00				//LAN8720 PHY芯片地址.




extern ETH_DMADESCTypeDef  *DMATxDescToSet;			//DMA发送描述符追踪指针
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		//DMA接收描述符追踪指针 
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;	//DMA最后接收到的帧信息指针


extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB];
extern ETH_DMADESCTypeDef  DMATxDscrTab[ETH_TXBUFNB];
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];











/////////////////////////////////////////
u8 LAN8720_Init(void);
u8 LAN8720_Get_Speed(void);
u8 ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
u8 ETH_Tx_Packet(u16 FrameLength);
u32 ETH_GetCurrentTxBuffer(void);
u8 ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);
void ETH_IRQHandler(void);




void lan8720_rx_packet(FrameTypeDef frame);
FrameTypeDef lan8720_rxDMA_config(void);
u8 lan8720_txDMA_config(u16 frame_len);
void lan8720_read_buff(u8 *p, u16 len);
void lan8720_write_buff(u8 *p, u16 len);

#endif
