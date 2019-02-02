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
EventGroupHandle_t  clockEventGroup;
EventBits_t        rezWaiteEvent;


static uint8_t            usartReadBuff[USART_READ_BUFF_SIZE];
static uint8_t            numRezRead;
static GPRMC_Def          myGPRMC;
static uint16_t           registerValue;
static S_TIME_user_config *configData;
static serverSetTime      timeSetStruct;

union
{
	uint16_t      *timeBuf;
	serverSetTime *timeData;
}timeBuffManager =
{
    .timeData = &timeSetStruct
};

static struct{
	SEZON_TIME sezonTime; // = SEZON_TIME_NOT_SET;
}timeCorrection = {
		.sezonTime  = SEZON_TIME_NOT_SET
};

//---------Sattic function definition---------
static void initUSARTGPS(void);


uint16_t TIME_calc_address_oper_reg(S_TIME_address *ps_TIME_address, uint16_t adres_start){
	ps_TIME_address->status_TIME   = adres_start + offsetof(S_TIME_address, status_TIME) / 2;

	for(uint8_t k = 0; k < CLOCK_QUANTITY; k++)
	{
		ps_TIME_address->clock[k].date_year    = adres_start + offsetof(S_TIME_address, clock[k]) / 2 + offsetof(S_Clock, date_year) / 2 ;
		ps_TIME_address->clock[k].date_month   = adres_start + offsetof(S_TIME_address, clock[k]) / 2 + offsetof(S_Clock, date_month) / 2;
		ps_TIME_address->clock[k].date_day     = adres_start + offsetof(S_TIME_address, clock[k]) / 2 + offsetof(S_Clock, date_day) / 2;
		ps_TIME_address->clock[k].time_hour    = adres_start + offsetof(S_TIME_address, clock[k]) / 2 + offsetof(S_Clock, time_hour) / 2;
		ps_TIME_address->clock[k].time_minute  = adres_start + offsetof(S_TIME_address, clock[k]) / 2 + offsetof(S_Clock, time_minute) / 2;
		ps_TIME_address->clock[k].time_second  = adres_start + offsetof(S_TIME_address, clock[k]) / 2 + offsetof(S_Clock, time_second) / 2;
		ps_TIME_address->clock[k].DATE         = adres_start + offsetof(S_TIME_address, clock[k]) / 2 + offsetof(S_Clock, DATE) / 2;
		ps_TIME_address->clock[k].TIME         = adres_start + offsetof(S_TIME_address, clock[k]) / 2 + offsetof(S_Clock, TIME) / 2;
	}

	ps_TIME_address->serverYear    = adres_start + offsetof(S_TIME_address, serverYear) / 2;
	ps_TIME_address->serverMonth   = adres_start + offsetof(S_TIME_address, serverMonth) / 2;
	ps_TIME_address->serverDay     = adres_start + offsetof(S_TIME_address, serverDay) / 2;
	ps_TIME_address->serverHour    = adres_start + offsetof(S_TIME_address, serverHour) / 2;
	ps_TIME_address->serverMinutes = adres_start + offsetof(S_TIME_address, serverMinutes) / 2;
	ps_TIME_address->serverSeconds = adres_start + offsetof(S_TIME_address, serverSeconds) / 2;
	return adres_start + sizeof(S_TIME_address)/2;
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
    uint8_t tm_wday = nowTime->tm_wday;
	// chec is we in summer time now. If not winter - summer
	if( !((tm_mon >= DAYLIGHT_START_MOUNTH ) && (tm_mon <= DAYLIGHT_STOP_MOUNTH)))
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
	return SEZON_TIME_SUMMER;
}


static void initTimeProcessing(bool isServer){
	clockInit();
	clockSetCallback(secondClockCallBack, alarmClockCallBack);
	if(isServer)
	{
		return;
	}
	initUSARTGPS();
    addGPSPars(GPRMC, &myGPRMC);
}


void setRCTTime(const serverSetTime *newTime, bool setAllarm)
{
	struct tm timeUpdate;
	time_t timeSetUTC, alarmSetUTC;
	//set time
	timeUpdate.tm_year = newTime->Year + 100;
	timeUpdate.tm_mon  = newTime->Month - 1; // -1 according time.h
	timeUpdate.tm_mday = newTime->Day;
	timeUpdate.tm_hour = newTime->Hour;
	timeUpdate.tm_min  = newTime->Minutes;
	timeUpdate.tm_sec  = newTime->Seconds;
	timeUpdate.tm_isdst = -1;
	// get UTC time for RTC
	timeSetUTC = mktime(&timeUpdate);
	// calculate time for update: current time UTC  + one hour in seconds  + correction in seconds
	if(setAllarm)
	{
		alarmSetUTC  =  mktime(&timeUpdate);
		alarmSetUTC +=  TIME_UPDATE_PERIOD_MINUTES * SECONDS_PER_MINUTES;
		clockSetAlarmTime( alarmSetUTC );
	}
	clockSetTime(timeSetUTC);
}


uint8_t serverUpdatteTime(void)
{
	if(configData->synchronizationSource == SYNCRONISATION_SOURCE_GPS)
	{
		return 0;
	}
	xEventGroupSetBits(
			clockEventGroup,
			SERVER_UPDATE_TIME
    );
	return 1;
}


void t_processing_TIME(void *p_task_par){
	time_t timeGetUTC;
	struct tm *timeGet;
	bool fineTuneClock = false;
	TickType_t allarmErrorIndTime_ms;

	configData = (S_TIME_user_config*)p_task_par;
    // stop task if it disable on configuration
	if(configData->state == DISABLE)
	{
		registerValue = TIME_STATUS_OK;
		processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.status_TIME);
		vTaskDelete(NULL);
	}

	clockEventGroup= xEventGroupCreate();

	if(configData->synchronizationSource == SYNCRONISATION_SOURCE_GPS)
	{
		/************************************CONFIGURATION TIME PROCESSING FOR WORK WITH GPS*******************/
		initTimeProcessing(false);
		xEventGroupSetBits(clockEventGroup,  ALARM_EVENT_BITS);
	}
	else
	{
		/************************************CONFIGURATION TIME PROCESSING FOR WORK WITH SERVER*******************/
		initTimeProcessing(true);
	}


	while(1){

		if( fineTuneClock )
		{
			numRezRead = ReadUSART(task_parameters[gpsUSARTNum].RdUSART, (uint8_t*)usartReadBuff, USART_READ_BUFF_SIZE, USART_READ_BUFF_TIME_MS);
			if( 0 != numRezRead)
			{
				parsGPS(&myGPRMC, usartReadBuff, numRezRead);
				if (rxGPSStatus((void*) &myGPRMC) == GPS_STATUS_COMPLETE)  // time was read
				{
					fineTuneClock = false;
					registerValue = TIME_STATUS_OK;
					processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.status_TIME);
					timeBuffManager.timeData->Year    = myGPRMC.year ;
					timeBuffManager.timeData->Month   = myGPRMC.mounth;
					timeBuffManager.timeData->Day     = myGPRMC.date;
					timeBuffManager.timeData->Hour    = myGPRMC.honour;
					timeBuffManager.timeData->Minutes = myGPRMC.minutes;
					timeBuffManager.timeData->Seconds = myGPRMC.seconds;
					setRCTTime(timeBuffManager.timeData, true);
					continue;
				}
		    }
			// UPDATE TIME TIMEOUT!!!!  Alarm indication
			if( allarmErrorIndTime_ms <= xTaskGetTickCount() )
			{
				// set alarm status and indication
				registerValue = TIME_STATUS_ALLARM;
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.status_TIME);
			}
		}
        // Wait for any event
	    rezWaiteEvent = xEventGroupWaitBits(
		    	clockEventGroup,
			    ( SECOND_EVENT_BIT | ALARM_EVENT_BITS | SERVER_UPDATE_TIME  ),
			    pdTRUE,
			    pdFALSE,
			    (fineTuneClock) ? (0) : (portMAX_DELAY) // if we still in fine tune clock process - continue
			    );
        // processing event
		if( rezWaiteEvent & SECOND_EVENT_BIT ) // processing second event
		{
			for(uint8_t k = 0; k < CLOCK_QUANTITY; k++)
			{
			    // get current date/time value
			    timeGetUTC = clockGetTime() + ((configData->clockConfig[k].isMinusCorrection) ? (-60 * configData->clockConfig[k].timeCorection ) :
			    		                                                                        ( 60 * configData->clockConfig[k].timeCorection));
				if(configData->clockConfig[k].isDaylightSaving)
				{
					timeGet = gmtime(&timeGetUTC);
					timeGetUTC  += ( SEZON_TIME_SUMMER == (timeCorrection.sezonTime = getSezonTime(timeGet)) ) ? (60 * SECONDS_PER_MINUTES) : (0);
				}
				timeGet = gmtime(&timeGetUTC);

				// update YEAR
				registerValue = (uint16_t)( timeGet->tm_year - 100 +2000);
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.clock[k].date_year);
				// update MONTH
				registerValue = (uint16_t)( timeGet->tm_mon+1);
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.clock[k].date_month);
				// update DAY
				registerValue = (uint16_t)( timeGet->tm_mday);
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.clock[k].date_day);
				// update HOUR
				registerValue = (uint16_t)( timeGet->tm_hour);
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.clock[k].time_hour);
				// update MINUTE
				registerValue = (uint16_t)( timeGet->tm_min);
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.clock[k].time_minute);
				// update SECOND
				registerValue = (uint16_t)( timeGet->tm_sec);
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.clock[k].time_second);
				// update DATE
				registerValue = (uint16_t)( ((timeGet->tm_mon + 1) << 8) | (uint8_t)(timeGet->tm_mday) );
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.clock[k].DATE);
				// update TIME
				registerValue = (uint16_t)( ((timeGet->tm_hour) << 8) | (uint8_t)(timeGet->tm_min));
				processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.clock[k].TIME);
			}
		}
		else if( rezWaiteEvent & SERVER_UPDATE_TIME ) // new time was received from server
		{
            // serverSetTime
			processing_mem_map_read_s_proces_object_modbus(timeBuffManager.timeBuf, sizeof(serverSetTime) / 2, s_address_oper_data.s_TIME_address.serverYear);
			setRCTTime(timeBuffManager.timeData, false);
		}
		else if( rezWaiteEvent & ALARM_EVENT_BITS ) // fine tune
		{
			fineTuneClock = true;
			// clear USART input buff
			Clrinbuf_without_time(&task_parameters[gpsUSARTNum]);
			// calculate error indication time threshold
			allarmErrorIndTime_ms = xTaskGetTickCount() + ERROR_IND_TIME_THRESHOLD;
		}
	}
}


