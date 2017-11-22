




#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include  <includes.h>




#define SETBIT(x, y)		x|=(1<<y)		//��x�ĵ�yλ��1
#define CLRBIT(x, y)		x&=!(1<<y)		//��x�ĵ�yλ��0	



#define DIR_UP_FLAG			(u8)(1<<0)




//����ͷβ��ʶ
#define TELEGRAM_HEAD		0x68		//
#define TELEGRAM_SYNC		0x68
#define TELEGRAM_END		0x16







//Э��������
#define  LH_CMD_LOGIN		0x0100	//��¼ָ��
#define  LH_CMD_HEART		0x0200	//����ָ��









#define CHANNEL_GPRS			((u8)0x0)
#define CHANNEL_ETH				((u8)0x1)
#define CHANNEL_RS485			((u8)0x2)





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











#endif
