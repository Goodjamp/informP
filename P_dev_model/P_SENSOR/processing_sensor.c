/*
* processing_sensor.c
 *
 *  Created on: July 29, 2017
 *      Author: Gerasimchuk
 *      Versin: 1
 */
#include "stdint.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"


#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#include "processing_sensor.h"
#include "processing_mem_map_extern.h"
#include "flash_operation.h"
#include "funct.h"

// All data address
extern S_address_oper_data s_address_oper_data;

xSemaphoreHandle semaphoreUpdateFRQ;



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
void t_processing_sensor(void *pvParameters){
	uint32_t totatalCNT;
	uint16_t frq;
	S_sensor_user_config *s_FRQConfig =(S_sensor_user_config*)pvParameters;

	while(1){

	}
}
