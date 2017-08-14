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

#define DEV_6                FRQmetter
//#define DEV_1_BIT_STATUS_REG 1

// ����������� ������-��������� FRQmetter
#define NUM_REG_STATUS_TC       1
// ����� ���������� ��������� FRQmetter (�������� � ���� ��������� ������ �����, ��� status ��������)
#define NUM_REG_TC              1


#pragma pack(push,1)

//-----------------------------������ ����������� ��������� �������� FRQmetter---------------------------------------------------------------
typedef struct{
	uint16_t  status_FRQmetter; // ������ �������� FRQmetter
	uint16_t  rez_FRQmetter;  // ����� � ������ �� ��������� ��������� FRQmetter
} S_FRQmetter_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ����������� �������� �������� FRQmetter------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_FRQmetter; // ������ �������� FRQmetter
	S_proces_object_modbus  rez_FRQmetter;    // measurement result
} S_FRQmetter_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ������������ �������� FRQmetter--------------------------------------------------------------------------------
typedef struct{
	FunctionalState state;                           // ��������� ����������� ������: ENABLE/DISABLE
	uint16_t time_gist;                                    // ����� ������������ ������� FRQmetter
}S_FRQmetter_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t FRQmetter_calc_address_oper_reg(S_FRQmetter_address *ps_FRQmetter_address, uint16_t adres_start);
void t_processing_FRQmetter(void *pvParameters);


#endif // PROCESSING_TC_SINAL

