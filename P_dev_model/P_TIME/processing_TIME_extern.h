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

#define DEV_7                TIME
//#define DEV_1_BIT_STATUS_REG 1

// Number of status registers TIME
#define NUM_REG_STATUS_TIME       1
// Total number of registers (without status registers)
#define NUM_REG_TIME              8

typedef enum {
	TIME_STATUS_OK,
	TIME_STATUS_ERROR
}TIME_STATUS;


#pragma pack(push,1)

//-----------------------------Адреса оперативных регистров процесса FRQmetter---------------------------------------------------------------
typedef struct{
	uint16_t  status_TIME;  // status register TIME module addtres
	uint16_t  date_year;    // date: year
	uint16_t  date_month;   // date: month
	uint16_t  date_day;     // date: day
	uint16_t  time_honour;  // time: honour
	uint16_t  time_minute;  // time: minute
	uint16_t  time_second;  // time: second
	uint16_t  DATE;
	uint16_t  TIME;
} S_TIME_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Оперативные регистры процесса FRQmetter------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_TIME; // status register TIME module
	S_proces_object_modbus  date_year;    // date: year
	S_proces_object_modbus  date_month;   // date: month
	S_proces_object_modbus  date_day;     // date: day
	S_proces_object_modbus  time_honour;  // time: honour
	S_proces_object_modbus  time_minute;  // time: minute
	S_proces_object_modbus  time_second;  // time: second
	S_proces_object_modbus  DATE;
	S_proces_object_modbus  TIME;
} S_TIME_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Конфигурация процесса FRQmetter--------------------------------------------------------------------------------
typedef struct{
	FunctionalState state;                           // состояние програмного модуля: ENABLE/DISABLE
	uint16_t timeZone;                               // часовой пояс
}S_TIME_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t TIME_calc_address_oper_reg(S_TIME_address *ps_TIME_address, uint16_t adres_start);
void t_processing_TIME(void *pvParameters);


#endif // PROCESSING_TC_SINAL

