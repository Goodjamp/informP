/**
  ******************************************************************************
  * @file    processing_sensor.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    29-July-2017
  * @brief
  */
#include "stdint.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"


#include "stm32f10x.h"

#include "i2c_source.h"
#include "DS1621_source.h"
#include "funct.h"
#include "processing_sensor_extern.h"
#include "processing_sensor.h"
#include "processing_mem_map_extern.h"


/* @brief
 *
 */
extern S_address_oper_data s_address_oper_data;

DS1621MessStatus mesStatus;
xSemaphoreHandle semaphoreUpdateFRQ;
uint16_t mesTemperature;
//
interfaceAddressDef ds1621Sensor = {
		.I2Cdef 	 = (void*)DS1321Interface,
		.addressMS4B = DS1621Address
};


void initI2C(void){
	I2CConfig i2cInit = {
			.fI2C = frqI2C
	};
	i2c_config(&i2cInit );
}

/* @brief
 *
 */
u16 sensor_calc_address_oper_reg(S_sensor_address *ps_sensor_address, u16 adres_start){
	ps_sensor_address->status_sensor = adres_start;
	ps_sensor_address->rez_sensor = ps_sensor_address->status_sensor + structFieldRegSize(S_sensor_oper_data,status_sensor);
	return adres_start;
}


/* @brief
 *
 */
#define tempBuffSize   20
struct{
	uint16_t temperature[tempBuffSize];
	uint8_t cnt;
}rezMes;

void t_processing_sensor(void *pvParameters){
	//S_sensor_user_config *s_FRQConfig =(S_sensor_user_config*)pvParameters;
	initI2C();

	ds16211SHOT(&ds1621Sensor);
	ds1621StartMess(&ds1621Sensor);

	while(1){

		if(ds1621MesDone(&ds1621Sensor, &mesStatus) == DS1621_STATUS_BUSS_ERROR){
			vTaskDelay(20);
			continue;
		}
		if(mesStatus != DS1621_MESS_REDY){
			vTaskDelay(20);
			continue;
		}

		ds1621ReadTemp(&ds1621Sensor, &mesTemperature);
		if(rezMes.cnt >= tempBuffSize){
			rezMes.cnt  = 0;
		}
		rezMes.temperature[rezMes.cnt ] = mesTemperature;
		rezMes.cnt++;
		ds1621ClearDONE(&ds1621Sensor);
		ds1621StartMess(&ds1621Sensor);
		vTaskDelay(20);
	}
}
