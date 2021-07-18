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

#define DEV_7                sensor
//#define DEV_1_BIT_STATUS_REG 1

// ����������� ������-��������� sensor
#define NUM_REG_STATUS_sensor       1
// ����� ���������� ��������� sensor (�������� � ���� ��������� ������ �����, ��� status ��������)
#define NUM_REG_sensor              3

// bit position of status local/receiver
typedef enum{
	SENSOR_STATUS_ALLARM_LOCAL           = 0,
	SENSOR_STATUS_ERROR_LOCAL            = 1,
	SENSOR_STATUS_ERROR_RECEIVER         = 2,
	SENSOR_STATUS_ERROR_REM_RX_TIMEOUT   = 3,
	SENSOR_STATUS_ERROR_REM_SENSOR       = 4,
	SENSOR_STATUS_ERROR_REM_RAINE_SENSOR = 5,
	SENSOR_STATUS_ERROR_REM_BATARY       = 6,
}SENSOR_STATUS;

#pragma pack(push,1)

//-----------------------------������ ����������� ��������� �������� sensor---------------------------------------------------------------
typedef struct{
	uint16_t  status_sensor;     // ������ �������� sensor
	uint16_t  rezTemperature;    // address in memory map rez measure temperature*10
	uint16_t  rezHumidity;       // address in memory map rez measure humidity *10
	uint16_t  rezPressure_mmHg;  // address in memory map rez Pressure mmHg
	uint16_t  rezPressure_GPasc; // address in memory map rez Pressure GPasc
	uint16_t  rezRain;           // address in memory map rez rain
} S_sensor_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ����������� �������� �������� sensor------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_sensor;     // status register
	S_proces_object_modbus  rezTemperature;    // measurement result temperature*10
	S_proces_object_modbus  rezHumidity;       // measurement result humidity*10
	S_proces_object_modbus  rezPressure_mmHg;  // measurement result Pressure mmHg
	S_proces_object_modbus  rezPressure_GPasc; // measurement result Pressure GPasc
	S_proces_object_modbus  rezRain; // raine state result: true false
} S_sensor_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ������������ �������� sensor--------------------------------------------------------------------------------
typedef struct{
	uint8_t state;   // state of module ENABLE/DISABLE
	uint16_t source;  // source of data: local/remote
}S_sensor_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t sensor_calc_address_oper_reg(S_sensor_address *ps_sensor_address, uint16_t adres_start);
void t_processing_sensor(void *pvParameters);

#endif // PROCESSING_SENSOR_EXTERN_H_

