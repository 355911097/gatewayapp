
#ifndef __RTC_H_
#define __RTC_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <stm32f2xx.h>
#include "stm32f2xx_rtc.h"



typedef struct
{
	uint8_t hours;			//Specifies the RTC Time Hour.
	uint8_t minutes;		//Specifies the RTC Time Minutes.
	uint8_t seconds;		// Specifies the RTC Time Seconds.
	uint8_t weekday;
	uint8_t date;
	uint8_t month;
	uint16_t year;
	
}rtc_calendar_t;




void rtc_init(void);
ErrorStatus rtc_set_time(u8 hour, u8 min, u8 sec, u8 ampm);
ErrorStatus rtc_set_date(u8 year, u8 month, u8 date, u8 weekday);
void rtc_get_calendar(rtc_calendar_t *calendar);
void rtc_set_calendar(rtc_calendar_t *calendar);
void RTC_Alarm_IRQHandler(void);
void RTC_WKUP_IRQHandler(void);
void RCC_IRQHandler(void);
u8 rtc_byte_to_bcd2(u8 value);
u8 rtc_bcd2_to_byte(u8 value);
#endif
