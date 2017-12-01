



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
#include "timer.h"





extern uint8_t gprs_rx_flag;						//GPRS接收数据标志位
extern usart_buff_t *gprs_rx_buff;					//GPRS接收数据缓冲区

extern u8 eth_rx_flag;								//以太网接收标志位
extern u8 rawudp_recv_buff[RAWUDP_RX_LENGHT];		//UDP接收数据缓冲区 
extern u32 rawudp_recv_buff_len;				//UDP接收数据的长度
	
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

u32 heart_time_cnt = 0;		//心跳计数器




u8 protocol_buff[PROTOCOL_BUFF_LENGHT] = {0};
u32 protocol_buff_len = 0;
 



static void protocol_task_fun(void *p_arg);

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
void protocol_task_create(void)
{
	OS_ERR os_err;
	CPU_SR_ALLOC();

	
	CPU_CRITICAL_ENTER();
	USART_OUT(USART3, "\r protocol_task_create......\r");
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
		USART_OUT(USART3, "\r rawudp_task fail\r");			 
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
static void protocol_task_fun(void *p_arg)
{
	OS_ERR err;
	u8 cmd = 0;
	u8 protocol_err = 0;
	u8 protocol_status = 0;
	u32 err_cnt = 0;
	u8 login_cnt = 0;
	
	USART_OUT(USART3, "\r protocol_task_fun......\r");
	while(DEF_TRUE)
	{

/*		
		if (heart_time_cnt == 0)
		{	
			if(protocol_status == STATE_LOGIN)
			{
				sign_in(CHANNEL_ETH);
				memset(gprs_rx_buff, 0, sizeof(usart_buff_t));	
				heart_time_cnt = timer_get_heart_ms();
				
			}
			else if(protocol_status == STATE_HEART)
			{
				heart_beat(CHANNEL_ETH);
				memset(gprs_rx_buff, 0, sizeof(usart_buff_t));	
				heart_time_cnt = timer_get_heart_ms();
			}
		}
		else
		{
			if((timer_get_heart_ms()-heart_time_cnt) >= GPRS_HEART_TIME) //心跳阶段检测心跳时间
			{
				err_cnt++; //错误计数器增加
				if(err_cnt >= GPRS_HEART_ERR_COUNT)	//超过最大尝试次数，重启GPRS
				{
					
					heart_time_cnt = 0;
					err_cnt = 0;	
				}
				else	//重新发送心跳指令
				{
					heart_time_cnt = 0;
				}
			}
			else
			{
				
				if(gprs_rx_flag == TRUE)	//收到数据标志
				{
					
					u8 buffer[USART_BUFF_LENGHT];
					u16 size;
					gprs_rx_flag = FALSE;
								
					
					if (fatch_gprs_data(buffer, &size))		//取得消息报文
					{
						protocol_err = process_protocol(buffer, size, CHANNEL_ETH);	//处理报文
						
						
						if (protocol_err == TRUE)
						{
							
							switch (protocol_status)
							{
								case STATE_LOGIN:
									
								
								break;
								
								
								case STATE_HEART:
									
								
								break;
								
								case STATE_RPT:
									
								
								break;
								
								
								default:
								break;
										
								
							}
							
						}
					}
						
				}
		
					
			}
			
		}
		
*/		
		

		
	//////////////////////////////////////////////////////////////////////////

	
		if(eth_rx_flag == 1)												//收到数据标志
		{
			
			u8 buffer[USART_BUFF_LENGHT];
			u16 size;
			gprs_rx_flag = FALSE;
						
			eth_rx_flag = 0;

			if (fatch_gprs_data(buffer, &size))								//取得消息报文
			{
				protocol_err = process_protocol(buffer, size, CHANNEL_ETH);	//处理报文
			
				if (protocol_err == TRUE)
				{
					
					switch (protocol_status)
					{
						case STATE_LOGIN:	//登录成功
								
							protocol_status = STATE_HEART;		
						break;
						
						
						case STATE_HEART:
							
						
							protocol_status = STATE_RPT;					
						break;
						
						
						case STATE_RPT:
							
							protocol_status = STATE_PTR;
						break;
						
						
						case STATE_PTR:		//接收模式
							
						
						break;
						
						default:
						break;
								
						
					}
					
				}
					
			}
			
		}
		else
		{
			switch(protocol_status)
			{
				case STATE_LOGIN:
					
					sign_in(CHANNEL_ETH);
					memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  //数据接收缓冲区清零
					protocol_buff_len = 0;							//数据接收计数清零	
					heart_time_cnt = timer_get_heart_ms();
				
					login_cnt++;
					if(login_cnt > 3)
					{
						//重启设备
					}
				
				break;
				
				
				case STATE_HEART:
					heart_beat(CHANNEL_ETH);
					memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  //数据接收缓冲区清零
					protocol_buff_len = 0;							//数据接收计数清零
					heart_time_cnt = timer_get_heart_ms();
					protocol_status = STATE_HEART;
				break;
				
				
				case STATE_RPT:
									
				
				break;
				
				
				case STATE_PTR:
					
					protocol_status = STATE_HEART;
				break;
				
				default:
				break;

			}
		
		
		}
		
		
		
		
		
		
	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
	

		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}

}




/*
*********************************************************************************************************
*                                          crc16_xmodem()
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
u16 crc16_xmodem(const u8 *data, u32 len)//len
{
	u16 crc = 0;
	while (len--)
		
	crc = crc_table[(crc >> 8 ^ *(data++)) & 0xFFU] ^ (crc << 8);
	
	return crc;
}



/*
*********************************************************************************************************
*                                          crc16_modbus()
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
u16 crc16_modbus(u8 *data, u32 len)
{
	u16 reg_crc = 0xFFFF;

	u16 crc = 0;
	u8 res = 0;
	u8 i = 0;
	
	while(len--)
	{
	
		reg_crc ^= *data++;
		for(i=0; i<8; i++)
		{
			if(reg_crc & 0x0001)
			{
				reg_crc = reg_crc>>1^0xA001;
			}
			else
			{
				reg_crc >>= 1;
			}
		}
	}
	
//	crc = ((reg_crc&0x00FF)<<8) | ((reg_crc&0xFF00)>>8);
	
	return reg_crc;
}

/*
*********************************************************************************************************
*                                          clear_rx_buff()
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
void clear_rx_buff(void)
{
	memset(gprs_rx_buff, 0, sizeof(usart_buff_t));	
}



/*
*********************************************************************************************************
*                                          process_protocol()
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
u8 process_protocol(u8 *buff, u16 size, u8 channel)
{
	
	buff += 4;					 //指针指向msgID
	
	size -=	7;					//去掉开始字符、长度、同步、crc和结束字符 总共6个字节
	
	return svr_to_ctu(buff, size, channel);
	
}






/*
*********************************************************************************************************
*                                          svr_to_ctu()
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
u8 svr_to_ctu(u8 *buff, u16 size, u8 channel)
{
	
	u16 cmd = 0;
	u16 ctr_unit = 0;
	
	//取出消息ID
	cmd = *buff++;
	cmd += (*buff++)<<8;  
	
	ctr_unit = *buff++;
	ctr_unit += (*buff++)<<8; 
	
	
	if ((ctr_unit&DIR_UP_FLAG) == DIR_UP_FLAG)	//判断消息是上行还是下行
	{
		USART_OUT(USART3, "GPRS ctr_unit UD= %d\r", ctr_unit);
		return 2;
	}
	
	
//	if((dev_info.dev_is_login == 0) && (channel == CHANNEL_ETH))
//	{
//		USART_OUT(USART3, "GPRS  without Login");
//		return 1;
//	}
	
	
	switch (cmd)
	{
		case 0x0100:
			
			
			return sign_in_ack(buff, size, channel);
		break;
	
		
		case 0x0200:
			
			return heart_beat_ack(buff, size, channel);
		break;
		
		
		default:
		break;
	}
	
	
	
	
}



/*
*********************************************************************************************************
*                                          ctu_to_srv()
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
bool ctu_to_srv(u8 *buff, u16 size, u8 channel)
{

	u16 i = 0, crc = 0, tmp_len = 0;
	u16 telegram_lenth = 0;
	u8 *tmp;
	
	tmp = (u8 *)malloc(256);
	usart_printf(USART3, size, buff);
	tmp[0] = TELEGRAM_HEAD;
	tmp[1] = (u8)size;
	tmp[2] = (u8)(size>>8);
	tmp[3] = TELEGRAM_SYNC;
	
	
	for (i=0; i<size; i++)
	{
		tmp[i+4] = *buff++;
	}
	
	crc = crc16_modbus(tmp+4, size);
	
	tmp[size+4] = (u8)crc;
	tmp[size+5] = (u8)(crc>>8);
	tmp[size+6] = TELEGRAM_END;
	
	tmp_len = size+7;
	
	if(channel == CHANNEL_GPRS)
	{
		USART_OUT(USART2, tmp);
	}
	else if(channel == CHANNEL_ETH)
	{
		usart_printf(USART3, tmp_len, tmp);
		rawudp_send_data(udppcb, tmp, tmp_len);
	
	}
	free(tmp);
	
	return TRUE;
}














/*
*********************************************************************************************************
*                                          sign_in()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : type ?????  enum timer3
*				count 
*
* Return(s)   : 0 ??????  1???????
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 sign_in(u8 channel)
{

	u8 buff[100] = {0};	
	u32 i = 0, buff_cnt = 0;
	u16 cmd = 0x0100;
	u16 ctr_unit = 0;
	u16 dev_id = 0x0001;
	u16 node_id = 0;
	u16 mes = 0;
	u8 date[8] = {0};
	u8 gw_id[6] = {0};
	u16 gw_software_version = 0x0102;
	u16 gw_hardware_version = 0x0103;
	
	
	SETBIT(ctr_unit, 1);	//控制单元
	CLRBIT(ctr_unit, 2);
	

	
	/************消息头******************/	
	//消息ID
	buff[buff_cnt++] = cmd&0xFF;
	buff[buff_cnt++] = cmd>>8;
	
	//控制单元
	buff[buff_cnt++] = ctr_unit&0xFF;
	buff[buff_cnt++] = ctr_unit>>8;
	
	//通信地址
	for(i=0; i<4; i++)	
	{
		buff[buff_cnt++] = 00;
	}
	//设备id
	buff[buff_cnt++] = dev_id &0xFF;
	buff[buff_cnt++] = dev_id>>8;
	//终端id
	buff[buff_cnt++] = node_id &0xF;
	buff[buff_cnt++] = node_id>>8;
	//消息流水号
	buff[buff_cnt++] = mes &0xF;
	buff[buff_cnt++] = mes>>8;
	
	for(i=0; i<8; i++)	//时间
	{
		buff[buff_cnt++] = 0xFF;
	}

	/*********消息体**********************/
	//网关id
	for(i=0; i<6; i++)		
	{
		buff[buff_cnt++] = gw_id[i];
	}
	
	//软件版本号
	buff[buff_cnt++] = gw_software_version&0xFF;
	buff[buff_cnt++] = gw_software_version>>8;
	//硬件版本号
	buff[buff_cnt++] = gw_hardware_version&0xFF;
	buff[buff_cnt++] = gw_hardware_version>>8;
	
	//硬件编号
	for(i=0; i<6; i++)	
	{
		buff[buff_cnt++] = 00;
	}
	//MAC地址
	for(i=0; i<6; i++)
	{
		buff[buff_cnt++] = 00;
	}
	//GSM模块的IMEI
	for(i=0; i<8; i++)
	{
		buff[buff_cnt++] = 0x33;
	}	

	return ctu_to_srv(buff, buff_cnt, channel);

}





bool sign_in_ack(u8 *buff, u16 size, u8 channel)
{

	u8 ack_status = 0;	
	u8 data1 = 0;
	
	
	data1 = buff[18];
	
	
	
	switch (data1)
	{
		case 0:
			
			//设备状态 为登录状态
			dev_info.dev_is_login = 1;
		break;
		
		case 1:
			
		break;
		
		case 2:

		
		break;
		
		
		case 3:
			
		
		break;
		
		
		case 4:
			
		break;
		
		default:
			return FALSE;
		break;
		
	}
	
	return TRUE;

}







/*
*********************************************************************************************************
*                                          heart_beat()
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
u8 heart_beat(u8 channel)
{
	u8 buff[100] = {0};
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
	
	/************消息头******************/	
	//消息ID
	buff[buff_cnt++] = cmd&0xFF;
	buff[buff_cnt++] = cmd>>8;
	
	//控制单元
	buff[buff_cnt++] = ctr_unit&0xFF;
	buff[buff_cnt++] = ctr_unit>>8;
	
	//通信地址
	for(i=0; i<4; i++)	
	{
		buff[buff_cnt++] = 00;
	}
	//设备id
	buff[buff_cnt++] = dev_id &0xFF;
	buff[buff_cnt++] = dev_id>>8;
	//终端id
	buff[buff_cnt++] = node_id &0xF;
	buff[buff_cnt++] = node_id>>8;
	//消息流水号
	buff[buff_cnt++] = mes &0xF;
	buff[buff_cnt++] = mes>>8;
	
	for(i=0; i<8; i++)	//时间
	{
		buff[buff_cnt++] = 0xFF;
	}

	/*********消息体**********************/
	//网关id
	for(i=0; i<6; i++)		
	{
		buff[buff_cnt++] = i;
	}
		
	return ctu_to_srv(buff, buff_cnt, channel);
}


bool heart_beat_ack(u8 *buff, u16 size, u8 channel)
{
	u8 ack_status = 0;	
		
	return TRUE;

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
bool fatch_gprs_data(u8 *buff, u16 *size)
{
	u16 i = 0;
	u16 data_len = 0;
	u16 crc = 0, rx_crc = 0;
	
	
	
	if(protocol_buff_len < 4)		//数据包太短
	{	
		memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  //数据接收缓冲区清零
		protocol_buff_len = 0;						//数据接收计数清零
		
		USART_OUT(USART3, "\r fatch_gprs_data too small\r\n");
		return FALSE;		
	}
	
	for (i=0; i<protocol_buff_len; i++)			//找协议开始标志
	{
		if(protocol_buff[i] == TELEGRAM_HEAD)	//找到协议开始标志
		{
			break;
		}
	}

	
	if(i >= PROTOCOL_BUFF_LENGHT)	//一直到结束没有找到报文头， 
	{
		memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  //数据接收缓冲区清零
		protocol_buff_len = 0;								//数据接收计数清零
		USART_OUT(USART3, "\r fatch_gprs_data no TELEGRAM_HEAD\r\n");
		return FALSE;
	}
	

	
	memcpy(protocol_buff, protocol_buff+i, protocol_buff_len-i);	//移除至报文开始标志	
	protocol_buff_len -= i;	
	
	if (protocol_buff_len < 5)				//数据包太短了
	{
		memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  //数据接收缓冲区清零
		protocol_buff_len = 0;							//数据接收计数清零
		USART_OUT(USART3, "\r fatch_gprs_data too short\r\n");
		return FALSE;
	}
	
	if (protocol_buff[3] != TELEGRAM_SYNC)		//找到报文中同步字段
	{
		memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  	//数据接收缓冲区清零
		protocol_buff_len = 0;								//数据接收计数清零
		USART_OUT(USART3, "\r fatch_gprs_data no TELEGRAM_SYNC\r\n");
		return FALSE;
		
	}
	
	//获取报文数据的长度（长度域）
	data_len = protocol_buff[2];
	data_len <<= 8;
	data_len |=  protocol_buff[1];
	

	if (data_len > PROTOCOL_BUFF_LENGHT)					// 防止接收的数据不对
	{
		memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  	//数据接收缓冲区清零
		protocol_buff_len = 0;								//数据接收计数清零
		USART_OUT(USART3, "\r fatch_gprs_data buffer overflow\r\n");	
		return FALSE;
	}
	
	
	if (protocol_buff_len >= data_len)			//分析协议
	{
		if (protocol_buff[data_len+6] == TELEGRAM_END)
		{
			crc = crc16_modbus(&protocol_buff[4], data_len);
			rx_crc = protocol_buff[data_len+6-1];
			rx_crc <<= 8;
			rx_crc += protocol_buff[data_len+6-2];
			
			if(crc == rx_crc)
			{
				*size = data_len+7;		//一个完整数据包的长度
				memcpy(buff, protocol_buff, *size);				//拷贝一个完整的数据包
				protocol_buff_len -= data_len+7;				
				
				memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  //数据接收缓冲区清零
				protocol_buff_len = 0;							//数据接收计数清零
				return TRUE;
			}
			else	//crc错误
			{
				memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  //数据接收缓冲区清零
				protocol_buff_len = 0;							//数据接收计数清零
				return FALSE;
			}
		}
		else	//报文的结束字符错误
		{
			memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  //数据接收缓冲区清零
			protocol_buff_len = 0;							//数据接收计数清零
			return FALSE;
		
		}
	}
	else
	{
		memset(protocol_buff, 0, PROTOCOL_BUFF_LENGHT);  //数据接收缓冲区清零
		protocol_buff_len = 0;							//数据接收计数清零
		return FALSE;
	}
	
	
	
}




















