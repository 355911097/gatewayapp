

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "bsp.h"
#include "usart.h"
#include "rtc.h"





/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void rtc_init(void)
{
	RTC_InitTypeDef rtc_init_structure;
	rtc_calendar_t ca;

	
	ca.year = 17;
	ca.month = 2;
	ca.date = 23;
	ca.hours = 2;
	ca.minutes = 4;
	ca.seconds = 0;
	
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问 

	RCC_LSEConfig(RCC_LSE_ON);//LSE 开启    
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);			
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
	RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 
	RTC_WaitForSynchro();
	
	
	if (RTC_ReadBackupRegister(RTC_BKP_DR0)!=0xA5A5)
	{
				
		rtc_init_structure.RTC_AsynchPrediv = 0x7F;
		rtc_init_structure.RTC_SynchPrediv = 0xFF;
		rtc_init_structure.RTC_HourFormat = RTC_HourFormat_24;//RTC设置为,24小时格式		
		RTC_Init(&rtc_init_structure);
		
		rtc_set_calendar(&ca);
		
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0xA5A5);	//标记已经初始化过了
	}

	
	BSP_IntVectSet(BSP_INT_ID_RTC_ALARM, RTC_Alarm_IRQHandler);	
	BSP_IntVectSet(BSP_INT_ID_RTC_WKUP, RTC_WKUP_IRQHandler);	
	BSP_IntVectSet(BSP_INT_ID_RTC, RTC_WKUP_IRQHandler);
	
	
}






/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
ErrorStatus rtc_set_time(u8 hour, u8 min, u8 sec, u8 ampm)
{
	RTC_TimeTypeDef rtc_time_structure;
	
	rtc_time_structure.RTC_Hours = hour;
	rtc_time_structure.RTC_Minutes = min;
	rtc_time_structure.RTC_Seconds = sec;
	rtc_time_structure.RTC_H12 = ampm;
	
	return RTC_SetTime(RTC_Format_BCD, &rtc_time_structure);
	
}


/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
ErrorStatus rtc_set_date(u8 year, u8 month, u8 date, u8 weekday)
{
	RTC_DateTypeDef rtc_date_structure;
	
	rtc_date_structure.RTC_Year = year;
	rtc_date_structure.RTC_Month = month;
	rtc_date_structure.RTC_Date = date;
	rtc_date_structure.RTC_WeekDay = weekday;
	
	return RTC_SetDate(RTC_Format_BCD, &rtc_date_structure);
}

/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void rtc_get_calendar(rtc_calendar_t *calendar)
{
	RTC_TimeTypeDef rtc_time_structure;
	RTC_DateTypeDef rtc_date_structure;
	
	RTC_GetTime(RTC_Format_BCD, &rtc_time_structure);
	RTC_GetDate(RTC_Format_BCD, &rtc_date_structure);
	
//	USART_OUT(USART3, "date4=%d-%d-%d %d:%d:%d\r", rtc_date_structure.RTC_Year, rtc_date_structure.RTC_Month, rtc_date_structure.RTC_Date, rtc_time_structure.RTC_Hours, rtc_time_structure.RTC_Minutes, rtc_time_structure.RTC_Seconds);
	
	calendar->year = rtc_date_structure.RTC_Year;
	calendar->month = rtc_date_structure.RTC_Month;
	calendar->date = rtc_date_structure.RTC_Date;
	calendar->weekday = rtc_bcd2_to_byte(rtc_date_structure.RTC_WeekDay);
	calendar->hours = rtc_bcd2_to_byte(rtc_time_structure.RTC_Hours);
	calendar->minutes = rtc_bcd2_to_byte(rtc_time_structure.RTC_Minutes);
	calendar->seconds = rtc_bcd2_to_byte(rtc_time_structure.RTC_Seconds);
	
	USART_OUT(USART3, "date1=%d-%d-%d %d:%d:%d\r", calendar->year, calendar->month, calendar->date, calendar->hours, calendar->minutes, calendar->seconds);
}

/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void rtc_set_calendar(rtc_calendar_t *calendar)
{
	RTC_TimeTypeDef rtc_time_structure;
	RTC_DateTypeDef rtc_date_structure;
	
	USART_OUT(USART3, "time2=%d-%d-%d %d:%d:%d\r", calendar->year, calendar->month, calendar->date, calendar->hours, calendar->minutes, calendar->seconds);
	
	rtc_date_structure.RTC_Year = calendar->year;
	rtc_date_structure.RTC_Month = calendar->month;
	rtc_date_structure.RTC_Date = calendar->date;
	rtc_date_structure.RTC_WeekDay = calendar->weekday;
	rtc_time_structure.RTC_Hours = rtc_bcd2_to_byte(calendar->hours);
	rtc_time_structure.RTC_Minutes = rtc_bcd2_to_byte(calendar->minutes);
	rtc_time_structure.RTC_Seconds = rtc_bcd2_to_byte(calendar->seconds);	
	
	USART_OUT(USART3, "time3=%d-%d-%d %d:%d:%d\r", calendar->year, calendar->month, calendar->date, calendar->hours, calendar->minutes, calendar->seconds);
	
	
	
	rtc_set_time(rtc_time_structure.RTC_Hours, rtc_time_structure.RTC_Minutes, rtc_time_structure.RTC_Seconds, RTC_H12_AM);
	rtc_set_date(rtc_date_structure.RTC_Year, rtc_date_structure.RTC_Month, rtc_date_structure.RTC_Date, RTC_Weekday_Wednesday);
	

}






/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A中断
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除中断标志

	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	//清除中断线17的中断标志 	

}	


/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP中断
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	//清除中断标志
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);//清除中断线22的中断标志 								
}



/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void RCC_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_TSF)==SET)//WK_UP中断
	{ 
		RTC_ClearFlag(RTC_FLAG_TSF);	//清除中断标志
	}   
	EXTI_ClearITPendingBit(EXTI_Line21);//清除中断线21的中断标志 								
}


/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 rtc_byte_to_bcd2(u8 value)
{
  uint8_t bcdhigh = 0;
  
  while (value >= 10)
  {
    bcdhigh++;
    value -= 10;
  }
  
  return  ((uint8_t)(bcdhigh << 4) | value);
}



/*
*********************************************************************************************************
*                                           spi1_exchange_byte()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8 rtc_bcd2_to_byte(u8 value)
{

	uint8_t tmp = 0;
	tmp = ((uint8_t)(value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
	return (tmp + (value & (uint8_t)0x0F));
}



