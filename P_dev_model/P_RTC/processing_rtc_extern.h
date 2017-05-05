/*
* processing_rtc_extern.h
 *
 *  Created on: May 5, 2017
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_RTC_EXTERN_H_
#define PROCESSING_RTC_EXTERN_H_

#include "stdint.h"

#include "processing_mem_map.h"

#define DEV_7                rtc
//#define DEV_1_BIT_STATUS_REG 1

// ����������� ������-��������� FRQmetter
#define NUM_REG_STATUS_TC       1
// ����� ���������� ��������� FRQmetter (�������� � ���� ��������� ������ �����, ��� status ��������)
#define NUM_REG_TC              1


#pragma pack(push,1)

//-----------------------------������ ����������� ��������� �������� FRQmetter---------------------------------------------------------------
typedef struct{
	uint16_t  status_rtc; // ������ �������� FRQmetter
	uint16_t  rez_rtc;  // ����� � ������ �� ��������� ��������� FRQmetter
} S_rtc_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ����������� �������� �������� FRQmetter------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_rtc; // ������ �������� FRQmetter
	S_proces_object_modbus  rez_rtc;    // measurement result
} S_rtc_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ������������ �������� FRQmetter--------------------------------------------------------------------------------
typedef struct{
	FunctionalState state;                           // ��������� ����������� ������: ENABLE/DISABLE
	uint16_t timeZone;                               // ������� ����
}S_rtc_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t rtc_calc_address_oper_reg(S_FRQmetter_address *ps_FRQmetter_address, uint16_t adres_start);
void t_processing_rtc(void *pvParameters);


#endif // PROCESSING_TC_SINAL

