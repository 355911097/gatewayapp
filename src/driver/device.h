
#ifndef __DEVIDE_H
#define __DEVIDE_H
#include <stm32f10x.h>



//日志打印
#define ADDR_BASE_DBG                                                   0x00D0
#define ADDR_DBG_SYS                                                    (ADDR_BASE_DBG+0)
#define ADDR_DBG_GPS                                                    (ADDR_BASE_DBG+1)
#define ADDR_DBG_GPRS                                                   (ADDR_BASE_DBG+2)
#define ADDR_DBG_LUX                                                    (ADDR_BASE_DBG+3)
#define ADDR_DBG_FLS                                                    (ADDR_BASE_DBG+4)
#define ADDR_DBG_EEP                                                    (ADDR_BASE_DBG+5)
#define ADDR_DBG_SUB                                                    (ADDR_BASE_DBG+6)
#define ADDR_DBG_RTC                                                    (ADDR_BASE_DBG+7)
#define ADDR_DBG_CHG                                                    (ADDR_BASE_DBG+8)



//设备信息
typedef struct
{
	uint8_t OME_code[10];			//设备制造商
	uint8_t hardware_version[3];	//硬件版本
	uint8_t software_version[3];	//软件版本
	uint8_t protect_version[3];		//协议版本
	uint8_t protect_type;			//协议类型
}dev_basic_info_t;

typedef struct
{
	uint8_t dev_type;
	uint8_t comm_addr[10];
	uint8_t server_ip[17];
	uint8_t server_apn[17];
	uint32_t server_port;
	
	uint8_t gprs_debug_level;
	uint8_t rtc_debug_level;
	

}dev_info_t;





#endif
