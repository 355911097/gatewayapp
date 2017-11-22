



/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <string.h>
#include "bsp.h"
#include "protocol.h"
#include "usart.h"
#include "rawudp.h"
#include "app.h"



extern uint8_t gprs_rx_flag;
extern usart_buff_t *gprs_rx_buff;

extern struct udp_pcb *udppcb;  	//定义一个UDP服务器控制块
extern OS_TCB 	protocol_task_TCB;
extern CPU_STK	protocol_task_stk[PROTOCOL_TASK_STK_SIZE];


const u16 crc_table[256] = {
0x0000U, 0x1021U, 0x2042U, 0x3063U, 0x4084U, 0x50a5U, 0x60c6U, 0x70e7U,
0x8108U, 0x9129U, 0xa14aU, 0xb16bU, 0xc18cU, 0xd1adU, 0xe1ceU, 0xf1efU,
0x1231U, 0x0210U, 0x3273U, 0x2252U, 0x52b5U, 0x4294U, 0x72f7U, 0x62d6U,
0x9339U, 0x8318U, 0xb37bU, 0xa35aU, 0xd3bdU, 0xc39cU, 0xf3ffU, 0xe3deU,
0x2462U, 0x3443U, 0x0420U, 0x1401U, 0x64e6U, 0x74c7U, 0x44a4U, 0x5485U,
0xa56aU, 0xb54bU, 0x8528U, 0x9509U, 0xe5eeU, 0xf5cfU, 0xc5acU, 0xd58dU,
0x3653U, 0x2672U, 0x1611U, 0x0630U, 0x76d7U, 0x66f6U, 0x5695U, 0x46b4U,
0xb75bU, 0xa77aU, 0x9719U, 0x8738U, 0xf7dfU, 0xe7feU, 0xd79dU, 0xc7bcU,
0x48c4U, 0x58e5U, 0x6886U, 0x78a7U, 0x0840U, 0x1861U, 0x2802U, 0x3823U,
0xc9ccU, 0xd9edU, 0xe98eU, 0xf9afU, 0x8948U, 0x9969U, 0xa90aU, 0xb92bU,
0x5af5U, 0x4ad4U, 0x7ab7U, 0x6a96U, 0x1a71U, 0x0a50U, 0x3a33U, 0x2a12U,
0xdbfdU, 0xcbdcU, 0xfbbfU, 0xeb9eU, 0x9b79U, 0x8b58U, 0xbb3bU, 0xab1aU,
0x6ca6U, 0x7c87U, 0x4ce4U, 0x5cc5U, 0x2c22U, 0x3c03U, 0x0c60U, 0x1c41U,
0xedaeU, 0xfd8fU, 0xcdecU, 0xddcdU, 0xad2aU, 0xbd0bU, 0x8d68U, 0x9d49U,
0x7e97U, 0x6eb6U, 0x5ed5U, 0x4ef4U, 0x3e13U, 0x2e32U, 0x1e51U, 0x0e70U,
0xff9fU, 0xefbeU, 0xdfddU, 0xcffcU, 0xbf1bU, 0xaf3aU, 0x9f59U, 0x8f78U,
0x9188U, 0x81a9U, 0xb1caU, 0xa1ebU, 0xd10cU, 0xc12dU, 0xf14eU, 0xe16fU,
0x1080U, 0x00a1U, 0x30c2U, 0x20e3U, 0x5004U, 0x4025U, 0x7046U, 0x6067U,
0x83b9U, 0x9398U, 0xa3fbU, 0xb3daU, 0xc33dU, 0xd31cU, 0xe37fU, 0xf35eU,
0x02b1U, 0x1290U, 0x22f3U, 0x32d2U, 0x4235U, 0x5214U, 0x6277U, 0x7256U,
0xb5eaU, 0xa5cbU, 0x95a8U, 0x8589U, 0xf56eU, 0xe54fU, 0xd52cU, 0xc50dU,
0x34e2U, 0x24c3U, 0x14a0U, 0x0481U, 0x7466U, 0x6447U, 0x5424U, 0x4405U,
0xa7dbU, 0xb7faU, 0x8799U, 0x97b8U, 0xe75fU, 0xf77eU, 0xc71dU, 0xd73cU,
0x26d3U, 0x36f2U, 0x0691U, 0x16b0U, 0x6657U, 0x7676U, 0x4615U, 0x5634U,
0xd94cU, 0xc96dU, 0xf90eU, 0xe92fU, 0x99c8U, 0x89e9U, 0xb98aU, 0xa9abU,
0x5844U, 0x4865U, 0x7806U, 0x6827U, 0x18c0U, 0x08e1U, 0x3882U, 0x28a3U,
0xcb7dU, 0xdb5cU, 0xeb3fU, 0xfb1eU, 0x8bf9U, 0x9bd8U, 0xabbbU, 0xbb9aU,
0x4a75U, 0x5a54U, 0x6a37U, 0x7a16U, 0x0af1U, 0x1ad0U, 0x2ab3U, 0x3a92U,
0xfd2eU, 0xed0fU, 0xdd6cU, 0xcd4dU, 0xbdaaU, 0xad8bU, 0x9de8U, 0x8dc9U,
0x7c26U, 0x6c07U, 0x5c64U, 0x4c45U, 0x3ca2U, 0x2c83U, 0x1ce0U, 0x0cc1U,
0xef1fU, 0xff3eU, 0xcf5dU, 0xdf7cU, 0xaf9bU, 0xbfbaU, 0x8fd9U, 0x9ff8U,
0x6e17U, 0x7e36U, 0x4e55U, 0x5e74U, 0x2e93U, 0x3eb2U, 0x0ed1U, 0x1ef0U	
};




dev_info_t dev_info;



static void protocol_task_fun(void *p_arg);


void protocol_task_create(void)
{
	OS_ERR os_err;
	CPU_SR_ALLOC();

	
	CPU_CRITICAL_ENTER();
	OSTaskCreate((OS_TCB 	* )&protocol_task_TCB,		
				 (CPU_CHAR	* )"protocol_task", 		
                 (OS_TASK_PTR )protocol_task_fun, 			
                 (void		* )0,					
                 (OS_PRIO	  )PROTOCOL_TASK_PRIO,     
                 (CPU_STK   * )&protocol_task_stk[0],	
                 (CPU_STK_SIZE)PROTOCOL_TASK_STK_SIZE/10,	
                 (CPU_STK_SIZE)PROTOCOL_TASK_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&os_err); 
				 
	CPU_CRITICAL_EXIT();
				 
	OSTimeDly(10, OS_OPT_TIME_DLY, &os_err);
	
	if(os_err != OS_ERR_NONE)
	{
		USART_OUT(USART3, "\rrawudp_task fail\r");			 
	}			 

}


static void protocol_task_fun(void *p_arg)
{
	OS_ERR err;
	u8 nn = 0;

	while(DEF_TRUE)
	{

		if(gprs_rx_flag == TRUE)
		{
			gprs_rx_flag = FALSE;
			
			
				
		}
		

		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}

}





u16 crc16(u16 crc, const u8 *data, u32 len )//len
{
	while (len--)
		
	crc = crc_table[(crc >> 8 ^ *(data++)) & 0xFFU] ^ (crc << 8);
	
	return crc;
}





u16 crc16_modbus(u8 *data, int len)
{
	u16 cin = 0xFFFF;
	u16 poly = 0x8005;
	u8 res = 0;
	u8 i = 0;
	
	while(len--)
	{
		res = *(data++);
		cin ^= res << 8;
		for(i=0; i<8; i++)
		{
			if(cin & 0x8000)
			{
				cin = (cin<<1) ^ poly;
			}
			else
			{
				cin = cin<<1;
			}
		}
	}
	
	return cin;
}





u8 svr_to_ctu(u8 *buff, u16 size, u8 channel)
{
	
	u16 cmd = 0;
	u16 ctr_unit = 0;
	
	
	cmd = *buff++;
	cmd += (*buff++)<<8;  
	
	ctr_unit = *buff++;
	ctr_unit += (*buff++)<<8; 
	
	
	if ((ctr_unit&DIR_UP_FLAG) == DIR_UP_FLAG)
	{
		return 2;
	}
	
	
	if(dev_info.dev_is_login == FALSE)
	{
		
		return 1;
	}
	
	
	switch (cmd)
	{
		case 0001:
			
		
		break;
	
		
		case 0002:
			
		break;
		
		
		default:
			break;
	}
	
	
	
	
}




bool ctu_to_srv(u8 *buff, u16 size, u8 channel)
{

	u16 i = 0, crc = 0;
	u16 telegram_lenth = 0;
	u8 tele[512] = {0};
	
	tele[0] = TELEGRAM_HEAD;
	tele[1] = (u8)size;
	tele[3] = (u8)(size>>8);
	tele[4] = TELEGRAM_SYNC;
	
	
	for (i=0; i<size-7; i++)
	{
		tele[i+7] = *buff++;
	}
	
	crc = crc16_modbus(tele+6, size);
	
	tele[size+5] = (u8)crc;
	tele[size+6] = (u8)crc>>8;
	tele[size+7] = TELEGRAM_END;
	

	
	if(channel == CHANNEL_GPRS)
	{
		USART_OUT(USART2, tele);
	}
	else if(channel == CHANNEL_ETH)
	{
		rawudp_send_data(udppcb, tele);
	}
	
	return TRUE;
}















void sign_in(u8 *buff, u32 size)
{

	u32 i = 0, buff_cnt = 0;
	u16 cmd = 0x0100;
	u16 ctr_unit = 0;
	u16 dev_id = 0x0001;
	u16 node_id = 0;
	u16 mes = 0;
	u8 date[8] = {0};
	u8 gw_id[6] = {0};
	u16 gw_software_version = 0;
	u16 gw_hardware_version = 0;
	
	
	SETBIT(ctr_unit, 1);
	CLRBIT(ctr_unit, 2);
	
	buff[buff_cnt++] = cmd&0xFF;
	buff[buff_cnt++] = cmd>>8;
	
	buff[buff_cnt++] = ctr_unit&0xFF;
	buff[buff_cnt++] = ctr_unit>>8;
	
	
	for(i=0; i<4; i++)
	{
		buff[buff_cnt+i] = 00;
	}
	
	buff[buff_cnt++] = dev_id &0xFF;
	buff[buff_cnt++] = dev_id>>8;
	
	buff[buff_cnt++] = node_id &0xF;
	buff[buff_cnt++] = node_id>>8;
	
	buff[buff_cnt++] = mes &0xF;
	buff[buff_cnt++] = mes>>8;
	
	for(i=0; i<8; i++)
	{
		buff[buff_cnt+i] = 00;
	}
	
	
	for(i=0; i<6; i++)
	{
		buff[buff_cnt+i] = gw_id[i];
	}
	
	buff[buff_cnt++] = gw_software_version&0xFF;
	buff[buff_cnt++] = gw_software_version>>8;
	
	buff[buff_cnt++] = gw_hardware_version&0xFF;
	buff[buff_cnt++] = gw_hardware_version>>8;
	
	for(i=0; i<6; i++)	//硬件编号
	{
		buff[buff_cnt+i] = 00;
	}
	
	for(i=0; i<6; i++)
	{
		buff[buff_cnt+i] = 00;
	}
	
	for(i=0; i<8; i++)
	{
		buff[buff_cnt+i] = 00;
	}
	
	size = buff_cnt;
}





void heart_beat(u8 *buff, u32 size)
{

	u32 i = 0, buff_cnt = 0;
	u16 cmd = 0x0200;
	u16 ctr_unit = 0;
	u16 dev_id = 0x0001;
	u16 node_id = 0;
	u16 mes = 0;
	u8 date[8] = {0};
	u8 gw_id[6] = {0};

	
	
	SETBIT(ctr_unit, 1);
	CLRBIT(ctr_unit, 2);
	
	buff[buff_cnt++] = cmd&0xFF;
	buff[buff_cnt++] = cmd>>8;
	
	buff[buff_cnt++] = ctr_unit&0xFF;
	buff[buff_cnt++] = ctr_unit>>8;
	
	
	for(i=0; i<4; i++)
	{
		buff[buff_cnt+i] = 00;
	}
	
	buff[buff_cnt++] = dev_id &0xFF;
	buff[buff_cnt++] = dev_id>>8;
	
	buff[buff_cnt++] = node_id &0xF;
	buff[buff_cnt++] = node_id>>8;
	
	buff[buff_cnt++] = mes &0xF;
	buff[buff_cnt++] = mes>>8;
	
	for(i=0; i<8; i++)
	{
		buff[buff_cnt+i] = 00;
	}
	
	for(i=0; i<6; i++)
	{
		buff[buff_cnt+i] = gw_id[i];
	}

	
	size = buff_cnt;
}












void fatch_gprs_data(u8 *buff, u16 size)
{
	u16 i = 0;
	
	
	if (gprs_rx_buff->index < 4)
	{
		gprs_rx_buff->index = 0;
		memset(gprs_rx_buff, 0, sizeof(usart_buff_t));	
		USART_OUT(USART3, "\r fatch_gprs_data too small\r \n");
	}
	
	
	for (i=0; i<gprs_rx_buff->index; i++)
	{
	
	
	}
	

}




















