

#ifndef __SYSTEM_H_
#define __SYSTEM_H_
#include "bsp.h"





typedef struct
{
	u8 OEM_code[5];
	u8 software_version[3];
	u8 hardware_version[3];
	u8 protocol_type;
	u8 protocol_version[3];
	
}dev_basic_info_t;





typedef struct dev
{
	
	u8 comm_addr[9];
	u8 server_apn[17];
	u8 server_ip[32];
	u32 server_port;
	
	u8 IMEI[8];					//GPRS的IMEI为BCD格式	
	u8 gprs_heart_timeout;		//GPRS心跳超时时间
	u8 gprs_comm_timeout;		//GPRS通信超时时间
	
	
}dev_info_t;






#endif 
