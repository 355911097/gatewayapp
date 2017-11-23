




#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include  <includes.h>




#define SETBIT(x, y)		x|=(1<<y)		//将x的第y位置1
#define CLRBIT(x, y)		x&=!(1<<y)		//将x的第y位清0	



#define DIR_UP_FLAG			(u8)(1<<0)




//报文头尾标识
#define TELEGRAM_HEAD		0x68		//
#define TELEGRAM_SYNC		0x68
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



#define GPRS_HEAT_ERR_COUNT		5



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





u8 process_protocol(u8 *buff, u16 size, u8 channel);
u8 svr_to_ctu(u8 *buff, u16 size, u8 channel);

bool fatch_gprs_data(u8 *buff, u16 *size);
u8 sign_in(u8 channel);
u8 heart_beat(u8 channel);

#endif
