/********************************************************************************
  * @file    BME280_digital_interface_HAL.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    27-Octobre-2017
  * @brief
  */

#ifndef BME280_DIGITAL_INTERFACE_HAL_H_
#define BME280_DIGITAL_INTERFACE_HAL_H_

typedef enum{
	TRANSACION_OK,
	TRANSACION_ERROR
}TRANSACION_STATUS;


TRANSACION_STATUS BMEReadData(uint8_t sensorAddress, uint8_t sensorReagister, uint8_t numData);
TRANSACION_STATUS BMEWriteData(uint8_t sensorAddress, uint8_t sensorReagister, uint8_t numData);

#endif
