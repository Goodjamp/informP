/**
  ******************************************************************************
  * @file    processing_sensor_extern.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    25-July-2017
  * @brief
  */

#ifndef PROCESSING_SENSOR_EXTERN_H_
#define PROCESSING_SENSOR_EXTERN_H_

#include "stdint.h"

#include "processing_mem_map.h"

#define DEV_8                sensor
//#define DEV_1_BIT_STATUS_REG 1

// ����������� ������-��������� sensor
#define NUM_REG_STATUS_sensor       1
// ����� ���������� ��������� sensor (�������� � ���� ��������� ������ �����, ��� status ��������)
#define NUM_REG_sensor              3


#pragma pack(push,1)

//-----------------------------������ ����������� ��������� �������� sensor---------------------------------------------------------------
typedef struct{
	uint16_t  status_sensor;   // ������ �������� sensor
	uint16_t  rezTemperature;  // address in memory map rez measure temperature*10
	uint16_t  rezHumidity;     // address in memory map rez measure humidity *10
	uint16_t  rezPressure;     // address in memory map rez measure Pressure *10
} S_sensor_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ����������� �������� �������� sensor------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_sensor;  // status register
	S_proces_object_modbus  rezTemperature; // measurement result measure temperature*10
	S_proces_object_modbus  rezHumidity;    // measurement result measure humidity*10
	S_proces_object_modbus  rezPressure;    // measurement result measure Pressure*10
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

