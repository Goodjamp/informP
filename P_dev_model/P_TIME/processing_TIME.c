#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "time.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "processing_USART.h"
#include "USART_fifo_operation.h"
#include "processing_TIME.h"
#include "processing_TIME_extern.h"
#include "RTCProcessing.h"
#include "GPSprocessing.h"

uint8_t usartReadBuff[USART_READ_BUFF_SIZE];
uint32_t timeUTC;
struct tm timeUpdate;
GPRMC_Def myGPRMC;
extern S_Task_parameters task_parameters[NUMBER_MY_PROCES];

//---------Sattic function definition---------
static void initUSARTGPS(void);


uint16_t TIME_calc_address_oper_reg(S_TIME_address *ps_TIME_address, uint16_t adres_start){

	return 0;
}

// Config USART
static void initUSARTGPS(void){
    S_port_config portConfig={
            .baudrate = gpsUSARTSpeed,
            .stopbits = gpsUSARTStopBits,
            .parity = gpsUSARTParity,
            .amountbyte = gpsUSARTWordLengt,
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

void secondRTCCallBack(void){
	// pass mutex, swithc task

}

/*
 * 1 - set current date/time with block all tasks
 * 2 - update time without block task.
 *
 *
 */

void updateTime(void){
	uint8_t numRezRead = 0;
	char *timeStr;
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
			timeUTC = mktime(&timeUpdate);
			setTime(timeUTC);
			//break;
		}
	}
}

void t_processing_TIME(void *p_task_par){
	initRTC();
	initUSARTGPS();
    addGPSPars(GPRMC, &myGPRMC);
    updateTime();
	while(1){

	}
}


