/**
  ******************************************************************************
  * @file    processing_FRQmetter_extern.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    23-April-2017
  * @brief
  */
#ifndef PROCESSING_FRQMETTER_EXTERN_H_
#define PROCESSING_FRQMETTER_EXTERN_H_

#include "stdint.h"

#include "processing_mem_map.h"

#define DEV_5                FRQmetter
//#define DEV_1_BIT_STATUS_REG 1

// Количекство статус-регистров FRQmetter
#define NUM_REG_STATUS_TC       1
// Общее количество регистров FRQmetter (имееться в виду регистров разных типов, без status регистра)
#define NUM_REG_TC              1

typedef enum{
	FRQ_STATUS_OK,
	FRQ_STATUS_ALLARM,
	FRQ_STATUS_ERROR
}FRQ_STATUS;


#pragma pack(push,1)

//-----------------------------Адреса оперативных регистров процесса FRQmetter---------------------------------------------------------------
typedef struct{
	uint16_t  status_FRQmetter;  // статус регистры FRQmetter
	uint16_t  rez_FRQmetter;     // адрес в памяти МК регистров состояния FRQmetter
} S_FRQmetter_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Оперативные регистры процесса FRQmetter------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_FRQmetter; // статус регистры FRQmetter
	S_proces_object_modbus  rez_FRQmetter;    // measurement result
} S_FRQmetter_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Конфигурация процесса FRQmetter--------------------------------------------------------------------------------
typedef struct{
	uint16_t state;           // state of module ENABLE/DISABLE
	int16_t  frqCorrection;   // correction of measured frequency: 1 = 1/100 Hz
}S_FRQmetter_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t FRQmetter_calc_address_oper_reg(S_FRQmetter_address *ps_FRQmetter_address, uint16_t adres_start);
void t_processing_FRQmetter(void *pvParameters);


#endif // PROCESSING_TC_SINAL

