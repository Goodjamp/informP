/********************************************************************************
  * @file    BME280_user_interfase.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    27-Octobre-2017
  * @brief
  */

#include "stdint.h"

#ifndef BME280_USER_INTERFASE_H_
#define BME280_USER_INTERFASE_H_

#define BME280_address_low   (uint8_t)0x76
#define BME280_address_hight (uint8_t)0b77

// list of measurement value
typedef enum{
	MES_VALUE_TEMPERATURE,
	MES_VALUE_HUMIDITY,
	MES_VALUE_PRESURE
}MES_VALUE_DEF;

typedef enum{
	MES_STATE_ENABLE,
	MES_STATE_DISABLE,
}MES_STATE_DEF;

typedef enum{
	MES_MODE_CONTINUOUS,
	MES_MODE_SIMLE,
}MES_MODE_DEF;

typedef enum{
	OVERSEMPL_DISABLE,
	OVERSEMPL_2,
	OVERSEMPL_4,
	OVERSEMPL_8,
	OVERSEMPL_16
}OVERSEMPL_DEF;

typedef enum{
	FILTER_DISABLE,
	FILTER_ORDER_2,
	FILTER_ORDER_4,
	FILTER_ORDER_8,
	FILTER_ORDER_16
}FILTER_DEF;

typedef enum{
	MEASUREMENT_DELAY_0_5ms,
	MEASUREMENT_DELAY_10_0ms,
	MEASUREMENT_DELAY_20_0ms,
	MEASUREMENT_DELAY_65_5ms,
	MEASUREMENT_DELAY_125_0ms,
	MEASUREMENT_DELAY_250_0ms,
	MEASUREMENT_DELAY_500_0ms,
	MEASUREMENT_DELAY_1000_0ms,
}MEASUREMENT_DELAY_DEF;

typedef enum{
	BME280_OK,
	BME280_SENSOR_ERROR,
	BME280_COMUNICATION_ERROR,
	BME280_IN_PROCESS_MEASUREMENTS
}BME280_STATUS;

typedef struct{
	uint8_t selfAddress;
	uint8_t calibrationData[2];
	BME280_STATUS sensorStatus;
}BME280Handler;

typedef void(*bme280MesCallbackDef)(BME280_STATUS rezMesStatus,float rezMesTemperature,float rezMesPressure, float rezMesHumidity);

//BME280 configuration functions
BME280_STATUS BME280_initBME280      (BME280Handler *handler, uint8_t address);
BME280_STATUS BME280_setMesCallBack  (bme280MesCallbackDef mesCallbak);
BME280_STATUS BME280_setValueMesState(BME280Handler *handler, MES_VALUE_DEF mesValue, MES_STAT_DEF newMesState);
BME280_STATUS BME280_setOverSample   (BME280Handler *handler, MES_VALUE_DEF mesValue, OVERSEMPL_DEF overSample);
BME280_STATUS BME280_setFilterPar    (BME280Handler *handler, MES_VALUE_DEF mesValue, FILTER_DEF filterPar);
BME280_STATUS BME280_setMesDelay     (BME280Handler *handler, MEASUREMENT_DELAY_DEF mesDelay);
// Measurement control functions
BME280_STATUS BME280_Mes             (BME280Handler *handler, float *rezMesTemperature, float *rezMesPressure, float *rezMesHumidity);
BME280_STATUS BME280_startContiniousMes   (BME280Handler *handler, MES_STATE_DEF newMesState);
// Information functions
BME280_STATUS BME280_getStatus       (BME280Handler *handler);


#endif
