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

// Количекство статус-регистров FRQmetter
#define NUM_REG_STATUS_TC       1
// Общее количество регистров FRQmetter (имееться в виду регистров разных типов, без status регистра)
#define NUM_REG_TC              1


#pragma pack(push,1)

//-----------------------------Адреса оперативных регистров процесса FRQmetter---------------------------------------------------------------
typedef struct{
	uint16_t  status_TIME; // статус регистры FRQmetter
	uint16_t  rez_TIME;  // адрес в памяти МК регистров состояния FRQmetter
} S_TIME_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Оперативные регистры процесса FRQmetter------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_TIME; // статус регистры FRQmetter
	S_proces_object_modbus  rez_TIME;    // measurement result
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

