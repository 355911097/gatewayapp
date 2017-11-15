




#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include  <includes.h>



//报文头尾标识
#define TELEGRAM_HEAD		0x68//
#define TELEGRAM_END		0x16


#define DIR_UP_FLAG			(u8)(1<<0)




//协议命令字
#define  LH_CMD_LOGIN		0x0100	//登录指令
#define  LH_CMD_HEART		0x0200	//心跳指令


















#endif
