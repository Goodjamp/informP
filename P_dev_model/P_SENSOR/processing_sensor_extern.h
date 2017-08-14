/*
* processing_sensor_extern.h
 *
 *  Created on: July 29, 2017
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_SENSOR_EXTERN_H_
#define PROCESSING_SENSOR_EXTERN_H_

#include "stdint.h"

#include "processing_mem_map.h"

#define DEV_8                sensor
//#define DEV_1_BIT_STATUS_REG 1

// ����������� ������-��������� sensor
#define NUM_REG_STATUS_TC       1
// ����� ���������� ��������� sensor (�������� � ���� ��������� ������ �����, ��� status ��������)
#define NUM_REG_TC              1


#pragma pack(push,1)

//-----------------------------������ ����������� ��������� �������� sensor---------------------------------------------------------------
typedef struct{
	uint16_t  status_sensor; // ������ �������� sensor
	uint16_t  rez_sensor;  // ����� � ������ �� ��������� ��������� sensor
} S_sensor_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ����������� �������� �������� sensor------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_sensor; // ������ �������� sensor
	S_proces_object_modbus  rez_sensor;    // measurement result
} S_sensor_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ������������ �������� sensor--------------------------------------------------------------------------------
typedef struct{
	FunctionalState state;                           // ��������� ����������� ������: ENABLE/DISABLE
	uint16_t time_gist;                                    // ����� ������������ ������� sensor
}S_sensor_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t sensor_calc_address_oper_reg(S_sensor_address *ps_sensor_address, uint16_t adres_start);
void t_processing_sensor(void *pvParameters);


#endif // PROCESSING_SENSOR_EXTERN_H_

