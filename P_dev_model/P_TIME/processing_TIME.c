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

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "funct.h"
#include "processing_mem_map_extern.h"
#include "processing_USART.h"
#include "USART_fifo_operation.h"
#include "processing_TIME.h"
#include "processing_TIME_extern.h"
#include "clockProcessing.h"
#include "GPSprocessing.h"

#include "debugStuff.h"


//--------------USART---------------------------------------------
extern S_address_oper_data s_address_oper_data;
extern S_Task_parameters task_parameters[NUMBER_MY_PROCES];
//event grup
EventGroupHandle_t clockEventGroup;
EventBits_t        rezWaiteEvent;

static uint8_t usartReadBuff[USART_READ_BUFF_SIZE];
static uint32_t timeSetUTC;
static time_t timeGetUTC;
struct tm *timeGet;
struct tm timeUpdate;
static GPRMC_Def myGPRMC;
static uint16_t registerValue;
static S_TIME_user_config *configData;

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
	// pass mutex, swithc task

}

bool updateTime(void){
	uint8_t numRezRead = 0;
	while(1)
	{
		numRezRead = ReadUSART(task_parameters[gpsUSARTNum].RdUSART, (uint8_t*)usartReadBuff, USART_READ_BUFF_SIZE, USART_READ_BUFF_TIME_MS);
		if( numRezRead == 0)
			continue;
		if( GPSdata.cnt < GPSBuffSize )
		{
			memcpy(&GPSdata.data[GPSdata.cnt], usartReadBuff, numRezRead);
			GPSdata.cnt += numRezRead;
		}
		parsGPS(&myGPRMC, usartReadBuff, numRezRead);
		if(rxGPSStatus((void*)&myGPRMC) == GPS_STATUS_COMPLETE)
		{
			//set time
			timeUpdate.tm_year = myGPRMC.year + 100;
			timeUpdate.tm_mon = myGPRMC.mounth - 1;
			timeUpdate.tm_mday = myGPRMC.date;
			timeUpdate.tm_hour = myGPRMC.honour;
			timeUpdate.tm_min = myGPRMC.minutes;
			timeUpdate.tm_sec = myGPRMC.seconds;
			timeUpdate.tm_isdst = -1;
			timeSetUTC = mktime(&timeUpdate) + configData->timeCorection * SECONDS_PER_HOUR;
			clockSetTime(timeSetUTC);
			return true;
		}
	}
	return false;
}

void t_processing_TIME(void *p_task_par){
	configData = (S_TIME_user_config*)p_task_par;
	registerValue = TIME_STATUS_ERROR;
	processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.status_TIME);
	// create event group for processing clock event
	clockEventGroup= xEventGroupCreate();
	clockInit();
	clockSetCallback(secondClockCallBack, alarmClockCallBack);
	initUSARTGPS();
    addGPSPars(GPRMC, &myGPRMC);
    updateTime();
	while(1){
		rezWaiteEvent = xEventGroupWaitBits(
				clockEventGroup,
				SECOND_EVENT_BIT | ALARM_EVENT_BITS,
				pdTRUE,
				pdFALSE,
				portMAX_DELAY
				);

		if( rezWaiteEvent & SECOND_EVENT_BIT ) // processing second event
		{
			// get curent time value
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
			//Update status
			registerValue = TIME_STATUS_OK;
			processing_mem_map_write_s_proces_object_modbus(&registerValue, 1, s_address_oper_data.s_TIME_address.status_TIME);

		}
		else if( rezWaiteEvent & ALARM_EVENT_BITS ) // processing alarm event
		{
         // TODO how I can processing ALARM event without block of second event  ?? open question
		}
	}
}


