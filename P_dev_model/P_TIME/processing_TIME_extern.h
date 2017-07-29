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

// ����������� ������-��������� FRQmetter
#define NUM_REG_STATUS_TC       1
// ����� ���������� ��������� FRQmetter (�������� � ���� ��������� ������ �����, ��� status ��������)
#define NUM_REG_TC              1


#pragma pack(push,1)

//-----------------------------������ ����������� ��������� �������� FRQmetter---------------------------------------------------------------
typedef struct{
	uint16_t  status_TIME;  // status register TIME module addtres
	uint16_t  date_year;    // date: year
	uint16_t  date_month;   // date: month
	uint16_t  date_day;     // date: day
	uint16_t  time_honour;  // time: honour
	uint16_t  time_minute;  // time: minute
	uint16_t  time_second;  // time: second
} S_TIME_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ����������� �������� �������� FRQmetter------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_TIME; // status register TIME module
	S_proces_object_modbus  date_year;    // date: year
	S_proces_object_modbus  date_month;   // date: month
	S_proces_object_modbus  date_day;     // date: day
	S_proces_object_modbus  time_honour;  // time: honour
	S_proces_object_modbus  time_minute;  // time: minute
	S_proces_object_modbus  time_second;  // time: second
} S_TIME_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ������������ �������� FRQmetter--------------------------------------------------------------------------------
typedef struct{
	FunctionalState state;                           // ��������� ����������� ������: ENABLE/DISABLE
	uint16_t timeZone;                               // ������� ����
}S_TIME_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t TIME_calc_address_oper_reg(S_TIME_address *ps_TIME_address, uint16_t adres_start);
void t_processing_TIME(void *pvParameters);


#endif // PROCESSING_TC_SINAL

