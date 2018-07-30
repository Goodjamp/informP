/********************************************************************************
  * @file    processing_TIME.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    19 jun 2017
  * @brief
  */
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
#include "envlock.h"
#include "envz.h"


#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"

#include "funct.h"
#include "processing_mem_map_extern.h"
#include "processing_USART.h"
#include "USART_fifo_operation.h"
#include "processing_TIME.h"
#include "processing_TIME_extern.h"
#include "clockProcessing.h"
#include "GPSprocessing.h"
#include "processing_reset_control.h"

#include "debugStuff.h"

//--------------USART---------------------------------------------
extern S_address_oper_data s_address_oper_data;
extern S_Task_parameters task_parameters[NUMBER_MY_PROCES];
//event grup
EventGroupHandle_t clockEventGroup;
EventBits_t        rezWaiteEvent;

static uint8_t usartReadBuff[USART_READ_BUFF_SIZE];
static uint8_t numRezRead;
static GPRMC_Def myGPRMC;
static uint16_t registerValue;
static S_TIME_user_config *configData;

static struct{
	uint8_t initTIme:1;
	uint8_t fineTuneClock:1;
	uint8_t reserved:6;
}timeProcessingState = {
		.initTIme = 1,
		.fineTuneClock = 0
};

static struct{
	SEZON_TIME sezonTime; // = SEZON_TIME_NOT_SET;
}timeCorrection = {
		.sezonTime  = SEZON_TIME_WINTER
};

//---------Sattic function definition---------
static void initUSARTGPS(void);


uint16_t TIME_calc_address_oper_reg(S_TIME_address *ps_TIME_address, uint16_t adres_start){
	ps_TIME_address->status_TIME = adres_start;
	ps_TIME_address->date_year   = ps_TIME_address->status_TIME + structFieldRegSize(S_TIME_address,status_TIME);
	ps_TIME_address->date_month  = ps_TIME_address->date_year + structFieldRegSize(S_TIME_address,date_year);
	ps_TIME_address->date_day    = ps_TIME_address->date_month + structFieldRegSize(S_TIME_address,date_month);
	ps_TIME_address->time_honour = ps_TIME_address->date_day + structFieldRegSize(S_TIME_address,date_day);
	ps_TIME_address->time_minute = ps_TIME_address->time_honour + structFieldRegSize(S_TIME_address,time_honour);
	ps_TIME_address->time_second = ps_TIME_address->time_minute + structFieldRegSize(S_TIME_address,time_minute);
	ps_TIME_address->DATE        = ps_TIME_address->time_second + structFieldRegSize(S_TIME_address,time_second);
	ps_TIME_address->TIME        = ps_TIME_address->DATE + structFieldRegSize(S_TIME_address,DATE);
	adres_start                  = ps_TIME_address->TIME + structFieldRegSize(S_TIME_address,TIME);
	return adres_start;
}

// Config USART
static void initUSARTGPS(void){
    S_port_config portConfig={
            .baudrate     = gpsUSARTSpeed,
            .stopbits     = gpsUSARTStopBits,
            .parity       = gpsUSARTParity,
            .amountbyte   = gpsUSARTWordLengt,
            .controlpotok = 0
    };
    ConfigureUSART(&portConfig, gpsUSARTDef);
}

/*------------------------- SWITCH TIM WINTER/SUMMER RULES--------------------------
 *in Ukraine we live in +2 hour UTC
 *switch to SUMMER TIME: IN LAST SUNDAY OF MARCH   IN 3:00 +1
 *switch to WINTER TIME: IN LAST SUNDAY OF OCTOBER IN 4:00 -1 (back to +2 UTC )
 *
 * */


/*
 * tm_sec	int	seconds after the minute	0-61*
   tm_min	int	minutes after the hour	    0-59
   tm_hour	int	hours since midnight	    0-23
   tm_mday	int	day of the month	        1-31
   tm_mon	int	months since January	    0-11
   tm_year	int	years since 1900
   tm_wday	int	days since Sunday	        0-6
   tm_yday	int	days since January 1	    0-365
   tm_isdst	int	Daylight Saving Time flag
 */
#define GPSBuffSize 512
struct{
	uint8_t data[GPSBuffSize + 20];
	uint16_t cnt;
}GPSdata;

void secondClockCallBack(uint32_t seconCnt){

	BaseType_t pxHigherPriorityTaskWoken;

	xEventGroupSetBitsFromISR(
			clockEventGroup,
			SECOND_EVENT_BIT,
			&pxHigherPriorityTaskWoken
    );
}


void alarmClockCallBack(uint32_t seconCnt){
	BaseType_t pxHigherPriorityTaskWoken;

	xEventGroupSetBitsFromISR(
			clockEventGroup,
			ALARM_EVENT_BITS,
			&pxHigherPriorityTaskWoken
    );

}

/*@brief getLastWeekDate  - return date of last Sunday in month
 *@[in]  date        -  day of month, 1-31
 *@[in]  dayOfWeek   -  day of week,  1-7
 *@[in]  dayInMounth -  number of day in month 1-31
 *@[in]
 *@[out] date of last Sunday
 *
 * */
static inline uint8_t getLastWeekDate(uint8_t date, uint8_t dayOfWeek, uint8_t dayInMounth){
	uint8_t nearestSanday = date + ( 7 - dayOfWeek);
	uint8_t weekToEnd = ( dayInMounth -  nearestSanday) / 7;

	return ( ( (weekToEnd * 7) + nearestSanday) <=  dayInMounth ) ? ((weekToEnd * 7) + nearestSanday) : (nearestSanday - 7);
}


static inline SEZON_TIME getSezonTime(struct tm *nowTime){
    uint8_t tm_mon  = nowTime->tm_mon + 1;
    uint8_t tm_wday = nowTime->tm_wday + 1;
	// chec is we in summer time now. If not winter - winter
	if(! ( (tm_mon >= DAYLIGHT_START_MOUNTH ) && (tm_mon <= DAYLIGHT_STOP_MOUNTH) ) )
	{
		return SEZON_TIME_WINTER;
	}

	if (DAYLIGHT_START_MOUNTH == tm_mon )
	{

		uint8_t lastSundayToStart = getLastWeekDate(nowTime->tm_mday, tm_wday, DAYLIGHT_START_MOUNTH_DAYS);
	    if( nowTime->tm_mday > lastSundayToStart )
		{
			return SEZON_TIME_SUMMER;
		}
		if ( ( nowTime->tm_mday == lastSundayToStart ) &&
	    	 ( nowTime->tm_hour >= DAYLIGHT_SWITCH_HOUR_UTC ) )
	    {
	    	return SEZON_TIME_SUMMER;
	    }
	    return SEZON_TIME_WINTER;
	}
	else if(DAYLIGHT_STOP_MOUNTH == tm_mon )
	{
		uint8_t lastSundaysToStop = getLastWeekDate(nowTime->tm_mday, tm_wday, DAYLIGHT_STOP_MOUNTH_DAYS);
	    if( nowTime->tm_mday < lastSundaysToStop )
		{
			return SEZON_TIME_SUMMER;
		}
		if( ( nowTime->tm_mday == lastSundaysToStop ) &&
		    ( nowTime->tm_hour <  DAYLIGHT_SWITCH_HOUR_UTC ) )
		{
		   	return SEZON_TIME_SUMMER;
		}
		return SEZON_TIME_WINTER;

	}

	return SEZON_TIME_WINTER;
}


static void initTimeProcessing(void){
	clockInit();
	clockSetCallback(secondClockCallBack, alarmClockCallBack);
	initUSARTGPS();
    addGPSPars(GPRMC, &myGPRMC);
}


static bool updateRTC(uint8_t *gpsData, uint8_t numberOfData ) {
	struct tm timeUpdate;
	time_t timeSetUTC, alarmSetUTC;
	struct tm *timeGet;
	parsGPS(&myGPRMC, usartReadBuff, numRezRead);
	if (rxGPSStatus((void*) &myGPRMC) == GPS_STATUS_COMPLETE) {
		//set time
		timeUpdate.tm_year = myGPRMC.year + 100;
		timeUpdate.tm_mon  = myGPRMC.mounth - 1; // -1 according time.h
		timeUpdate.tm_mday = myGPRMC.date;
		timeUpdate.tm_hour = myGPRMC.honour;
		timeUpdate.tm_min  = myGPRMC.minutes;
		timeUpdate.tm_sec  = myGPRMC.seconds;
		timeUpdate.tm_isdst = -1;
		// get UTC time for RTC
		timeSetUTC = mktime(&timeUpdate);
		// I need day of the week for detect current period of dayLight
		timeGet = gmtime(&timeSetUTC);
		uint8_t correction = ( SEZON_TIME_SUMMER == (timeCorrection.sezonTime = getSezonTime(timeGet)) ) ?
						(configData->timeCorection + 1) :
						(configData->timeCorection);
        // add correction in seconds for current time
		timeSetUTC += correction * SECONDS_PER_HOUR;
		 // ALARM USE FOR UPDATE DAYLYGHT!!!!!!!!!!!!!   VERY IMPORTANT     !!!!!!!!!!!!!
		//timeUpdate.tm_min  = 0;
		//timeUpdate.tm_sec  = 0;
		// calculate time for update: current time UTC  + one hour in seconds  + correction in seconds
		alarmSetUTC =  mktime(&timeUpdate);
		alarmSetUTC =  alarmSetUTC + 3 + correction * SECONDS_PER_HOUR;
		// set alarm
		clockSetAlarmTime( alarmSetUTC );
		// set RTC
		clockSetTime(timeSetUTC);
		return true;
	}
	return false;
}

void t_processing_TIME(void *p_task_par){
	uint8_t temp = 0;
	time_t timeGetUTC;
	struct tm *timeGet;
	TickType_t errorIndTimeThresHold_ms;

	configData = (S_TIME_user_config*)p_task_par;
    // stop task if it disable on configuration
	if(configData->state == DISABLE)
	{
		vTaskDelete(NULL);
	}
	// create event group for processing clock event
	clockEventGroup= xEventGroupCreate();
	// initialization module
	initTimeProcessing();
	//------------set clock first time-------------------
	registerValue = TIME_STATUS_ERROR;
	processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.status_TIME);

	Clrinbuf_without_time(&task_parameters[gpsUSARTNum]);
	// calculate error indication time threshold
	errorIndTimeThresHold_ms = xTaskGetTickCount() + ERROR_IND_TIME_THRESHOLD;
	while(1)
	{
		numRezRead = ReadUSART(task_parameters[gpsUSARTNum].RdUSART, (uint8_t*)usartReadBuff, USART_READ_BUFF_SIZE, USART_READ_BUFF_TIME_MS);
		if( 0 != numRezRead)
		{
		    if (updateRTC(usartReadBuff, numRezRead))
		    {
		    	RESET_GLOBAL_STATUS(DEV_6);
			    break;
		    }
		}
		if( errorIndTimeThresHold_ms <= xTaskGetTickCount() )
		{
			// set global error status end error indication
			SET_GLOBAL_STATUS(DEV_6);
		}
	}

	registerValue = TIME_STATUS_OK;
	processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.status_TIME);

	while(1){

		if( timeProcessingState.fineTuneClock )
		{
			// update RTC
			numRezRead = ReadUSART(task_parameters[gpsUSARTNum].RdUSART, (uint8_t*)usartReadBuff, USART_READ_BUFF_SIZE, USART_READ_BUFF_TIME_MS);
			if( 0 != numRezRead)
			{
				if ( updateRTC(usartReadBuff, numRezRead) )
				{
					 timeProcessingState.fineTuneClock = 0;
					 registerValue = TIME_STATUS_OK;
					 processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.status_TIME);
				}
			}
			// UPDATE TIME TIMOUT!!!!  Alarm indication
			if( errorIndTimeThresHold_ms <= xTaskGetTickCount() )
			{
				// set alarm status and indication
				registerValue = TIME_STATUS_ALLARM;
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.status_TIME);
			}
			// Take a last events without timeout
		    rezWaiteEvent = xEventGroupWaitBits(
			    	clockEventGroup,
				    ( SECOND_EVENT_BIT | ALARM_EVENT_BITS  ),
				    pdTRUE,
				    pdFALSE,
				    0
				    );
		}
		else
		{
			// infinity wait for any time processing event
		    rezWaiteEvent = xEventGroupWaitBits(
			    	clockEventGroup,
				    ( SECOND_EVENT_BIT | ALARM_EVENT_BITS ),
				    pdTRUE,
				    pdFALSE,
				    portMAX_DELAY
				    );
		}
        // processing event
		if( rezWaiteEvent & SECOND_EVENT_BIT ) // processing second event
		{

			// get current date/time value
			timeGetUTC = clockGetTime();
			timeGet = gmtime(&timeGetUTC);

			// update YEAR
			registerValue = (uint16_t)( timeGet->tm_year - 100 +2000);
			processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.date_year);
			// update MONTH
			registerValue = (uint16_t)( timeGet->tm_mon+1);
			processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.date_month);
			// update DAY
			registerValue = (uint16_t)( timeGet->tm_mday);
			processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.date_day);
			// update HOUR
			registerValue = (uint16_t)( timeGet->tm_hour);
			processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.time_honour);
			// update MINUTE
			registerValue = (uint16_t)( timeGet->tm_min);
			processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.time_minute);
			// update SECOND
			registerValue = (uint16_t)( timeGet->tm_sec);
			processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.time_second);

			// update DATE
			registerValue = (uint16_t)( ((timeGet->tm_mon + 1) << 8) | (uint8_t)(timeGet->tm_mday) );
			processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.DATE);
			// update TIME
			registerValue = (uint16_t)( ((timeGet->tm_hour) << 8) | (uint8_t)(timeGet->tm_min));
			processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.TIME);

		}
		else if( rezWaiteEvent & ALARM_EVENT_BITS ) // processing alarm event, dayLight update
		{
			timeProcessingState.fineTuneClock = 1;
			// clear USART in buff
			Clrinbuf_without_time(&task_parameters[gpsUSARTNum]);
			// calculate error indication time threshold
			errorIndTimeThresHold_ms = xTaskGetTickCount() + ERROR_IND_TIME_THRESHOLD;
			if( (temp++) & 0x1)
			{
				debugPin_2_Clear;
			}
			else
			{
				debugPin_2_Set;
			}

		}
	}
}


