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

// Количекство статус-регистров sensor
#define NUM_REG_STATUS_TC       1
// Общее количество регистров sensor (имееться в виду регистров разных типов, без status регистра)
#define NUM_REG_TC              1


#pragma pack(push,1)

//-----------------------------Адреса оперативных регистров процесса sensor---------------------------------------------------------------
typedef struct{
	uint16_t  status_sensor; // статус регистры sensor
	uint16_t  rez_sensor;  // адрес в памяти МК регистров состояния sensor
} S_sensor_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Оперативные регистры процесса sensor------------------------------------------------------------------------
typedef struct{
	S_proces_object_modbus  status_sensor; // статус регистры sensor
	S_proces_object_modbus  rez_sensor;    // measurement result
} S_sensor_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Конфигурация процесса sensor--------------------------------------------------------------------------------
typedef struct{
	FunctionalState state;                           // состояние програмного модуля: ENABLE/DISABLE
	uint16_t time_gist;                                    // время гистерезисса прийома sensor
}S_sensor_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t sensor_calc_address_oper_reg(S_sensor_address *ps_sensor_address, uint16_t adres_start);
void t_processing_sensor(void *pvParameters);


#endif // PROCESSING_SENSOR_EXTERN_H_

