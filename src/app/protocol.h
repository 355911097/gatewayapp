




#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include  <includes.h>




#define SETBIT(x, y)		x|=(1<<y)		//将x的第y位置1
#define CLRBIT(x, y)		x&=~(1<<y)		//将x的第y位清0	



#define CTR_UNIT_BIT0			(u8)(1<<0)
#define CTR_UNIT_BIT1			(u8)(1<<1)
#define CTR_UNIT_BIT2			(u8)(1<<2)



//报文头尾标识
#define TELEGRAM_HEAD		0x3A		//
#define TELEGRAM_SYNC		0x3A
#define TELEGRAM_END		0x16







//协议命令字
#define  LH_CMD_LOGIN		0x0100	//登录指令
#define  LH_CMD_HEART		0x0200	//心跳指令









#define CHANNEL_GPRS			((u8)0x0)
#define CHANNEL_ETH				((u8)0x1)
#define CHANNEL_RS485			((u8)0x2)





#define STATE_LOGIN				0
#define STATE_HEART				1
#define	STATE_RPT				2
#define STATE_PTR				3




#define PROTOCOL_BUFF_LENGHT 			512



#define GPRS_HEART_ERR_COUNT		5
#define GPRS_HEART_TIME				1000*60*2			//心跳包时间


typedef struct
{
	u8 head;
	u16 len;
	u32 addr;
	u8 ctr_uint;
	u16 dev_type;
	u16 cmd;
	u8 *data;
	u8 time[6];
	u16 crc16;
	u8 end;
}telegram_t;


typedef  enum
{
	PROTOCOL_ERR_NONE 				= 0,
	PROTOCOL_ERR_LEN_SMALL,
	PROTOCOL_ERR_HEAD,
	PROTOCOL_ERR_SYNC,
	PROTOCOL_ERR_OVERFLOW,
	PROTOCOL_ERR_CRC,
	PROTOCOL_ERR_END
	
}PROTOCOL_ERR_E;




void protocol_task_create(void);

u16 process_protocol(u8 *buff, u16 size, u8 channel);
u16 svr_to_ctu(u8 *buff, u16 size, u8 channel, u16 *cmd);
bool ctu_to_srv(u8 *buff, u16 size, u8 channel, u16 cmd);
bool fatch_gprs_data(u8 *buff, u16 *size);

u8 sign_in_0001(u8 channel);
bool sign_in_0001_ack(u8 *buff, u16 size, u8 channel);
u8 heart_beat_0002(u8 channel);
bool heart_beat_0002_ack(u8 *buff, u16 size, u8 channel);
bool dev_restart_0003(u8 *buff, u16 size, u8 channel);
u8 dev_restart_0003_ack(u8 channel);
u8 fire_alarm_0101(u8 channel);
bool fire_alarm_0101_ack(u8 *buff, u16 size, u8 channel);
u16 crc16_modbus(u8 *data, u32 len);


#endif
