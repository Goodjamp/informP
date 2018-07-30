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

#include "processing_sensor.h"
#include "i2c_user_interface.h"
#include "DS1621_source.h"
#include "BME280_user_interface.h"
#include "funct.h"
#include "processing_sensor_extern.h"
#include "processing_sensor.h"
#include "processing_mem_map_extern.h"
#include "processing_reset_control.h"

#include "debugStuff.h"

extern S_address_oper_data s_address_oper_data;
DS1621MessStatus mesStatus;
xSemaphoreHandle semaphoreUpdateFRQ;
static BME280_STATUS bmeStatus;

//---------------------------------I2C user implementation functions-----------------------
void i2cInitGpio(uint8_t step){

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPBEN, ENABLE);


	// config I2C CSCL GPIO
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL;
	GPIO_InitStructure.GPIO_Mode = (step) ? (GPIO_Mode_AF_OD) : (GPIO_Mode_IPU);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStructure);

	// config I2C CSDA GPIO
	GPIO_InitStructure.GPIO_Pin = ( I2C1_SDA);
	GPIO_InitStructure.GPIO_Mode =  (step) ? (GPIO_Mode_AF_OD) : ( GPIO_Mode_IPU);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);

	// Enable Alternate function
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void delay_us(uint32_t timeout)
{
	uint32_t steps = timeout/10;
    volatile uint32_t cnt = 0;
	while( cnt++ < steps){};
}


#include "debugStuff.h"

void i2cRecover(uint32_t i2cFRQ)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPBEN, ENABLE);

	if(GPIO_ReadInputDataBit(I2C1_SCL_PORT, I2C1_SDA) == Bit_SET)
	{
		return;
	}

	debugPin_1_Set;
	GPIO_SetBits(I2C1_SCL_PORT, I2C1_SCL);
	// config I2C CSCL GPIO
	GPIO_InitStructure.GPIO_Pin   = I2C1_SCL;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	/*Config CSL functions of I2C1*/
	GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStructure);


	GPIO_SetBits(I2C1_SDA_PORT, I2C1_SDA);
	// config I2C CSDA GPIO
	GPIO_InitStructure.GPIO_Pin   = I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);


	//gen 9 pulses
	uint32_t halfPeriodUs = SystemCoreClock/(i2cFRQ * 2);
    volatile uint8_t cnt = 0;
	#define NUMBER_PULSES    9
	while(cnt++ < NUMBER_PULSES)
	{
		GPIO_ResetBits(I2C1_SCL_PORT, I2C1_SCL);
		delay_us(halfPeriodUs);
		GPIO_SetBits(I2C1_SCL_PORT, I2C1_SCL);
		delay_us(halfPeriodUs);
	}

	// generate stop order
	GPIO_ResetBits(I2C1_SCL_PORT, I2C1_SCL);
	GPIO_ResetBits(I2C1_SCL_PORT, I2C1_SDA);
	delay_us(halfPeriodUs);
	GPIO_SetBits(I2C1_SCL_PORT, I2C1_SCL);
	delay_us(halfPeriodUs);
	GPIO_SetBits(I2C1_SCL_PORT, I2C1_SDA);

	debugPin_1_Clear;
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
	ps_sensor_address->status_sensor     = adres_start;
	ps_sensor_address->rezTemperature    = ps_sensor_address->status_sensor + structFieldRegSize(S_sensor_oper_data,status_sensor);
	ps_sensor_address->rezHumidity       = ps_sensor_address->rezTemperature + structFieldRegSize(S_sensor_oper_data,rezTemperature);
	ps_sensor_address->rezPressure_mmHg  = ps_sensor_address->rezHumidity + structFieldRegSize(S_sensor_oper_data,rezHumidity);
	ps_sensor_address->rezPressure_GPasc = ps_sensor_address->rezPressure_mmHg + structFieldRegSize(S_sensor_oper_data,rezPressure_mmHg);
	adres_start = ps_sensor_address->rezPressure_GPasc + structFieldRegSize(S_sensor_oper_data,rezPressure_GPasc);
	return adres_start;
}


void i2c_init(void){
	I2C_configDef i2c_configParamiters = {
			.frequencyI2C = I2C_SENSOR_FRQ_HZ
	};
	i2cConfig(I2C_SENSOR, &i2c_configParamiters);
}


BME280Handler sensorHandler;


BME280_STATUS initI2C_Sensor(void){
	BME280_STATUS bmeStatus;
	bool sensorIsOnLine = false;

	i2c_init();

	BME280_setI2CAddress(&sensorHandler, BME280_ADDRESS_HIGHT);
    // Is sensor online ?
	if(BME280_STATUS_OK  != (bmeStatus = BME280_isOnLine(&sensorHandler, &sensorIsOnLine) ) ){
        return bmeStatus;
	}

	if( !sensorIsOnLine ){
		return BME280_STATUS_SENSOR_ERROR;
	}

	if(BME280_STATUS_OK  != (bmeStatus = BME280_init(&sensorHandler) ) )
	{
		return bmeStatus;
	}
	// Enable measurement all value with 16 oversemple
	if(BME280_STATUS_OK  != (bmeStatus = BME280_setValueMesState(&sensorHandler, MES_VALUE_TEMPERATURE, MES_STATE_ENABLE) ) )
	{
		return bmeStatus;
	}
	if(BME280_STATUS_OK  != (bmeStatus = BME280_setOverSample(&sensorHandler, MES_VALUE_HUMIDITY, OVERSEMPLE_16) ) )
	{
		return bmeStatus;
	}
	if(BME280_STATUS_OK  != (bmeStatus = BME280_setValueMesState(&sensorHandler, MES_VALUE_PRESSURE, MES_STATE_ENABLE) ) )
	{
		return bmeStatus;
	}
	if(BME280_STATUS_OK  != (bmeStatus = BME280_setOverSample(&sensorHandler, MES_VALUE_HUMIDITY, OVERSEMPLE_16) ) )
	{
		return bmeStatus;
	}
	if(BME280_STATUS_OK  != (bmeStatus = BME280_setValueMesState(&sensorHandler, MES_VALUE_HUMIDITY, MES_STATE_ENABLE) ) )
	{
		return bmeStatus;
	}
	if(BME280_STATUS_OK  != (bmeStatus = BME280_setOverSample(&sensorHandler, MES_VALUE_HUMIDITY, OVERSEMPLE_16) ) )
	{
		return bmeStatus;
	}
    // set delay between measurement equal 65 ms
	if(BME280_STATUS_OK  != (bmeStatus = BME280_setMesDelay(&sensorHandler, MEASUREMENT_DELAY_65_5ms) ) )
	{
		return bmeStatus;
	}
	return BME280_STATUS_OK;
}


static void updateSensorStatus(SENSOR_STATUS newStatus){
	static uint16_t currentSensorStatus = SENSOR_STATUS_NOT_SET;
	if(currentSensorStatus == newStatus)
	{
		return;
	}
	currentSensorStatus = newStatus;
	uint16_t regValue = (uint16_t)newStatus;
	processing_mem_map_write_s_proces_object_modbus((uint16_t*)&regValue, 1, s_address_oper_data.s_sensor_address.status_sensor);
    switch(regValue){
    case SENSOR_STATUS_OK:
		// set global error status end error indication
		RESET_GLOBAL_STATUS(DEV_7);
		break;
    case SENSOR_STATUS_ERROR:
		// set global error status end error indication
		SET_GLOBAL_STATUS(DEV_7);
		break;
    default:break;
    }
}

void t_processing_sensor(void *pvParameters){
	S_sensor_user_config *s_sensorUserConfig =(S_sensor_user_config*)pvParameters;
	uint16_t rezMes = 0;
	float rezMesHumidity;
	float rezMesTemperature;
	float rezMesPressure;

    // stop task if it disable on configuration
	if(s_sensorUserConfig->state == DISABLE)
	{
		vTaskDelete(NULL);
	}

	bmeStatus = initI2C_Sensor();

	//status = (bmeStatus == BME280_STATUS_OK) ? SENSOR_STATUS_OK : SENSOR_STATUS_ERROR;
	updateSensorStatus(  (bmeStatus == BME280_STATUS_OK) ? SENSOR_STATUS_OK : SENSOR_STATUS_ERROR  );
	//processing_mem_map_write_s_proces_object_modbus((uint16_t*)&status, 1, s_address_oper_data.s_sensor_address.status_sensor);

	while(1){

		if( BME280_STATUS_OK  != bmeStatus )
		{
			if( BME280_STATUS_OK  == (bmeStatus = initI2C_Sensor()) )
			{
				//status = SENSOR_STATUS_OK;
				updateSensorStatus(SENSOR_STATUS_OK);
				//processing_mem_map_write_s_proces_object_modbus((uint16_t*)&status, 1, s_address_oper_data.s_sensor_address.status_sensor);
			    continue;
			}
			vTaskDelay(10);
		}
		else
		{
			if(BME280_STATUS_OK  != (bmeStatus = BME280_forcedMes(&sensorHandler, &rezMesTemperature,
					                                                              &rezMesPressure,
					                                                              &rezMesHumidity) ) )
			{
				//status = SENSOR_STATUS_ERROR;
				updateSensorStatus(SENSOR_STATUS_ERROR);
				//processing_mem_map_write_s_proces_object_modbus((uint16_t*)&status, 1, s_address_oper_data.s_sensor_address.status_sensor);
				vTaskDelay(10);
				continue;
			}

		    rezMes = rezMesTemperature*10;
		    processing_mem_map_write_s_proces_object_modbus(&rezMes, 1, s_address_oper_data.s_sensor_address.rezTemperature);
		    rezMes = rezMesHumidity*10;
		    processing_mem_map_write_s_proces_object_modbus(&rezMes, 1, s_address_oper_data.s_sensor_address.rezHumidity);
		    rezMes = rezMesPressure/100;
		    processing_mem_map_write_s_proces_object_modbus(&rezMes, 1, s_address_oper_data.s_sensor_address.rezPressure_GPasc);
		    rezMes = (rezMesPressure * PASCAL_TO_MMHG_COEF);
		    processing_mem_map_write_s_proces_object_modbus(&rezMes, 1, s_address_oper_data.s_sensor_address.rezPressure_mmHg);
		    vTaskDelay(50);
		}
	}
}
