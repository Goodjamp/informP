/*
* processing_TIME_extern.h
 *
 *  Created on: May 5, 2017
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_TIME_EXTERN_H_
#define PROCESSING_TIME_EXTERN_H_

#include "stdint.h"

#include "processing_mem_map.h"

#define DEV_6            TIME
#define CLOCK_QUANTITY   4

typedef enum {
	TIME_STATUS_OK,
	TIME_STATUS_ALLARM,
	TIME_STATUS_ERROR
}TIME_STATUS;


#pragma pack(push,1)

typedef struct{
	uint16_t  date_year;    // date: year
	uint16_t  date_month;   // date: month
	uint16_t  date_day;     // date: day
	uint16_t  time_hour;  // time: honour
	uint16_t  time_minute;  // time: minute
	uint16_t  time_second;  // time: second
	uint16_t  DATE;
	uint16_t  TIME;
}S_Clock;



typedef struct{
	uint16_t  status_TIME;  // status register TIME module addtres
	S_Clock   clock[CLOCK_QUANTITY];
	uint16_t  serverYear;
	uint16_t  serverMonth;
	uint16_t  serverDay;
	uint16_t  serverHour;
	uint16_t  serverMinutes;
	uint16_t  serverSeconds;
} S_TIME_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Оперативные регистры процесса FRQmetter------------------------------------------------------------------------

typedef struct
{
	uint16_t Year;
	uint16_t Month;
	uint16_t Day;
	uint16_t Hour;
	uint16_t Minutes;
	uint16_t Seconds;
}serverSetTime;

typedef struct
{
	uint16_t  timeCorection:14;  // time correction in minutes
    uint16_t  isMinusCorrection:1;
    uint16_t  isDaylightSaving:1;
}clockIndConfigT;

typedef struct
{
	uint16_t      date_year;    // date: year
	uint16_t      date_month;   // date: month
	uint16_t      date_day;     // date: day
	uint16_t      time_hour;    // time: honour
	uint16_t      time_minute;  // time: minute
	uint16_t      time_second;  // time: second
	uint16_t      DATE;
	uint16_t      TIME;
}clockIndStateT;


typedef struct{
    uint16_t       status_TIME; // status register TIME module
    clockIndStateT clockState[CLOCK_QUANTITY];
    serverSetTime  serverTime;
} S_TIME_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Конфигурация процесса FRQmetter--------------------------------------------------------------------------------
typedef struct{
	uint16_t  state;                               // state of module ENABLE/DISABLE
	uint16_t  synchronizationSource;
	clockIndConfigT clockConfig[CLOCK_QUANTITY];
}S_TIME_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t TIME_calc_address_oper_reg(S_TIME_address *ps_TIME_address, uint16_t adres_start);
void t_processing_TIME(void *pvParameters);
uint8_t serverUpdatteTime(void);


#endif // PROCESSING_TC_SINAL

