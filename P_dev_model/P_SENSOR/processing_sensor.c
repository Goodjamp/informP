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
#include "stm32f10x_gpio.h"

#include "i2c_user_interface.h"
#include "DS1621_source.h"
#include "BME280_user_interfase.h"
#include "funct.h"
#include "processing_sensor_extern.h"
#include "processing_sensor.h"
#include "processing_mem_map_extern.h"
#include "debugStuff.h"

extern S_address_oper_data s_address_oper_data;
DS1621MessStatus mesStatus;
xSemaphoreHandle semaphoreUpdateFRQ;

//---------------------------------I2C user implementation functions-----------------------
void i2cInitGpio(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	/*I2C1 pins config*/


	RCC_APB2PeriphResetCmd(RCC_APB2ENR_IOPBEN, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2ENR_IOPBEN, DISABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPBEN, ENABLE);


	// config I2C CSCL GPIO
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*Config CSL functions of I2C1*/
	GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStructure);

	// config I2C CSDA GPIO
	GPIO_InitStructure.GPIO_Pin = ( I2C1_SDA);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);
	// Enable Alternate function

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

uint32_t i2cgetTimeMs(void)
{
	//return 0;
	return xTaskGetTickCount();
}

//-----------------------------------sensor user implementftion functions------------------------------
TRANSACION_STATUS BMEReadData (uint8_t sensorAddress, uint8_t sensorReagister, uint8_t *data, uint8_t numData){
	if(i2cRxData(I2C_SENSOR, sensorAddress, sensorReagister, numData, data) != I2C_STATUS_OK)
	{
		return TRANSACION_STATUS_ERROR;
	};
	return TRANSACION_STATUS_OK;
}


TRANSACION_STATUS BMEWriteData(uint8_t sensorAddress, uint8_t sensorReagister, uint8_t *data, uint8_t numData){
	if( i2cTxData(I2C_SENSOR, sensorAddress, sensorReagister, numData, data) != I2C_STATUS_OK)
	{
		return TRANSACION_STATUS_ERROR;
	};
	return TRANSACION_STATUS_OK;
}


uint32_t  sensorGetTime(void)
{
	//return 0;
	return xTaskGetTickCount();
}


/* @brief
 *
 */
u16 sensor_calc_address_oper_reg(S_sensor_address *ps_sensor_address, u16 adres_start){
	ps_sensor_address->status_sensor = adres_start;
	ps_sensor_address->rez_sensor = ps_sensor_address->status_sensor + structFieldRegSize(S_sensor_oper_data,status_sensor);
	return adres_start;
}

uint8_t dataArrayTx[] = {60};
uint8_t addressRegTx = 0XF2;
uint8_t dataArrayRx[1] = {0};
uint8_t addressRegRx = 0X88;


void i2c_init(void){
	I2C_configDef i2c_configParamiters = {
			.frequencyI2C = I2C_SENSOR_FRQ_HZ
	};
	i2cConfig(I2C_SENSOR, &i2c_configParamiters);
}


BME280Handler sensorHandler;

void t_processing_sensor(void *pvParameters){
	//S_sensor_user_config *s_FRQConfig =(S_sensor_user_config*)pvParameters;
	float rezMesHumidity;
	float rezMesTemperature;
	float rezMesPressure;


	i2c_init();
	BME280_init(&sensorHandler, BME280_ADDRESS_HIGHT);
	BME280_setValueMesState(&sensorHandler, MES_VALUE_TEMPERATURE, MES_STATE_ENABLE);
	BME280_setValueMesState(&sensorHandler, MES_VALUE_PRESSURE, MES_STATE_ENABLE);
	BME280_setValueMesState(&sensorHandler, MES_VALUE_HUMIDITY, MES_STATE_ENABLE);
	BME280_setOverSample(&sensorHandler,    MES_VALUE_TEMPERATURE, OVERSEMPLE_8);



	//i2c_write_data(DS1321Interface, devAddress<<1, regAddress, sizeof(TXBuff), TXBuff);
	//ds16211SHOT(&ds1621Sensor);
	//ds1621StartMess(&ds1621Sensor);

	while(1){

		if (BME280_forcedMes(&sensorHandler, &rezMesTemperature, &rezMesPressure, &rezMesHumidity) == BME280_STATUS_COMUNICATION_ERROR)
		{
			vTaskDelay(30);
			i2c_init();
		}
		//vTaskDelay(10);

		/*
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
		*/
	}
	//xTaskGetTickCount();
}
