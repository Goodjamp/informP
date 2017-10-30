/********************************************************************************
  * @file    BME280_source.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    19-Octobre-2017
  * @brief
  */

#include "stdint.h"

#include "BME280_source.h"
#include "BME280_user_interfase.h"

// TRANSACION_STATUS BMEReadData (uint8_t sensorAddress, uint8_t sensorReagister, uint8_t *data, uint8_t numData);
BME280_STATUS BME280_init(BME280Handler *handler, uint8_t address){
	handler->selfAddress = address;
	//if(  BMEReadData( address, BME280_REG_CALIB00_25, ) ){
//
	//}
}




