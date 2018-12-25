/**
  ******************************************************************************
  * @file    processing_sensor.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    29-July-2017
  * @brief
  */
#include "stdint.h"
#include "stdbool.h"

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

#include "NRF24L01user.h"
#include "NRF24L01.h"
#include "moduleHWInit.h"

#include "debugStuff.h"

extern S_address_oper_data s_address_oper_data;
DS1621MessStatus mesStatus;
xSemaphoreHandle semaphoreUpdateFRQ;
static BME280_STATUS bmeStatus;
STATUS statusRx;
nrfHeader nrfRx;
transactionT rxData;
transactionBuffT rxBuff =
{
	.data = &rxData
};


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
	ps_sensor_address->rezRain           = ps_sensor_address->rezPressure_GPasc + structFieldRegSize(S_sensor_oper_data,rezPressure_GPasc);
	adres_start                          = ps_sensor_address->rezRain + structFieldRegSize(S_sensor_oper_data,rezRain);
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


static void initNRF(void)
{
	nrfRx = NRF24L01_init(NRF_INTERFACE_N01);

	NRF24L01_power_switch(nrfRx, NRF_SET);
	NRF24L01_FLUSH_RX(nrfRx);
	NRF24L01_FLUSH_TX(nrfRx);
	NRF24L01_set_RX_address(nrfRx, PIPE0, NRF_ADDRESS);
	NRF24L01_set_TX_addres(nrfRx, NRF_ADDRESS);
	NRF24L01_set_crco(nrfRx, CRCO_2_BYTES);
	NRF24L01_set_TX_PayloadSize(nrfRx, PIPE0, sizeof(transactionT));
	NRF24L01_set_rf_dr(nrfRx, DATA_SPEED_250K);
	NRF24L01_set_rf_chanel(nrfRx, NRF_CHANEL);
	NRF24L01_clear_interrupt(nrfRx, STATUS_RX_DR); // Clear status NRF
	NRF24L01_set_rx_mode(nrfRx);
}


static inline void updateSensorStatus(uint16_t newStatus){
	volatile static uint16_t currentSensorStatus = SENSOR_STATUS_NOT_INIT;
	if(currentSensorStatus == newStatus)
	{
		return;
	}
	currentSensorStatus = newStatus;
	processing_mem_map_write_s_proces_object_modbus((uint16_t*)&newStatus, 1, s_address_oper_data.s_sensor_address.status_sensor);
	if(currentSensorStatus != 0)
	{
		// set global error status end error indication
		SET_GLOBAL_STATUS(DEV_7);
	}
	else
	{
		// reset global error status end error indication
		RESET_GLOBAL_STATUS(DEV_7);
	}
}


static bool gistAddData(gistT *inGist, bool rez)
{
	inGist->ringBuffRez[inGist->posWrite] = rez;
	if(rez)
	{
		inGist->cntTrue++;
	}
	uint16_t lastPointer = (inGist->posWrite + 1) & (sizeof(inGist->ringBuffRez) - 1);
	if(inGist->ringBuffRez[lastPointer])
	{
		inGist->cntTrue--;
	}
	inGist->posWrite++;
	inGist->posWrite &= (sizeof(inGist->ringBuffRez) - 1);

	return (inGist->cntTrue >= TIMEOUT_GIST_SUCSSESS_SIZE) ? (true) : (false);
}


static void processingLocalSensor(void)
{
	uint16_t rezMes = 0;
	float rezMesHumidity;
	float rezMesTemperature;
	float rezMesPressure;

	bmeStatus = initI2C_Sensor();
	updateSensorStatus(  (bmeStatus == BME280_STATUS_OK) ? ( SENSOR_STATUS_OK ) : ((uint16_t)(1 << SENSOR_STATUS_ERROR_LOCAL)));
	while(1){

		if( BME280_STATUS_OK  != bmeStatus )
		{
			if( BME280_STATUS_OK  == (bmeStatus = initI2C_Sensor()) )
			{
				updateSensorStatus(SENSOR_STATUS_OK);
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
				updateSensorStatus(((uint16_t)(1 << SENSOR_STATUS_ERROR_LOCAL)));
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


void processingRemoteSensor(void)
{
	STATUS status_reg;
	uint16_t moduleStatus = SENSOR_STATUS_OK;
	gistT inGist =
	{
		.ringBuffRez = {[0 ... (sizeof(inGist.ringBuffRez) - 1)] = false},
		.posWrite = 0,
		.cntTrue  = 0
	};

	initNRF();
	while(1)
    {
		vTaskDelay(MESSUREMT_PERIOD_MS);
        updateSensorStatus(moduleStatus);
    	NRF24L01_get_status_tx_rx(nrfRx, &status_reg);

    	if(status_reg.RX_DR == 0)//wait interrupt
    	{
    		if(!gistAddData(&inGist, false))
    		{
    			moduleStatus = SENSOR_STATUS_OK;
    			moduleStatus |= (uint16_t)(1 << SENSOR_STATUS_ERROR_REM_RX_TIMEOUT);
    		}
        	continue;
    	}
    	else
    	{
    		if(gistAddData(&inGist, true))
    		{
    			moduleStatus &= ~(uint16_t)(1 << SENSOR_STATUS_ERROR_REM_RX_TIMEOUT);
    		}
    	}

    	if(*(uint8_t*)(&status_reg) == 0xff)
    	{
    		moduleStatus  = SENSOR_STATUS_OK;
    		moduleStatus |= (uint16_t)(1 << SENSOR_STATUS_ERROR_RECEIVER);
    		continue; // there are no sense processing next steps
    	}
    	else
    	{
    		moduleStatus &= ~(uint16_t)(1 << SENSOR_STATUS_ERROR_RECEIVER);
    	}

    	// new data was received
    	NRF24L01_read_rx_data(nrfRx, sizeof(transactionT), rxBuff.buf);
    	NRF24L01_FLUSH_RX(nrfRx);
    	NRF24L01_clear_interrupt(nrfRx,STATUS_RX_DR);
    	// update status register
    	if( rxData.status & (uint8_t)(1 << REM_METEO_STATUS_ERROR_SENSOR) )
    	{
    		moduleStatus |= (uint16_t)(1 << SENSOR_STATUS_ERROR_REM_SENSOR);
            continue;
    	}
    	else
    	{
    		moduleStatus &= ~(uint16_t)(1 << SENSOR_STATUS_ERROR_REM_SENSOR);
    	}

    	if( rxData.status & (uint8_t)(1 << REM_METEO_STATUS_ERROR_BATARY) )
    	{
    		moduleStatus |= (uint16_t)(1 << SENSOR_STATUS_ERROR_REM_BATARY);
    	}
    	else
    	{
    		moduleStatus &= ~(uint16_t)(1 << SENSOR_STATUS_ERROR_REM_BATARY);
    	}


    	if( rxData.status & (uint8_t)(1 << REM_METEO_STATUS_ERROR_RAINE_SENSOR) )
    	{
    		moduleStatus |= (uint16_t)(1 << SENSOR_STATUS_ERROR_REM_RAINE_SENSOR);
    	}
    	else
    	{
    		moduleStatus &= ~(uint16_t)(1 << SENSOR_STATUS_ERROR_REM_RAINE_SENSOR);
    		uint16_t rezRain = (rxData.status & (uint8_t)(1 << REM_METEO_STATUS_RAINE)) ? 1 : 0;
        	processing_mem_map_write_s_proces_object_modbus(&rezRain,
        			                                        1,
        			                                        s_address_oper_data.s_sensor_address.rezRain);
    	}

    	processing_mem_map_write_s_proces_object_modbus((uint16_t*)&rxData.temperature,    1, s_address_oper_data.s_sensor_address.rezTemperature);
    	processing_mem_map_write_s_proces_object_modbus((uint16_t*)&rxData.humifity,       1, s_address_oper_data.s_sensor_address.rezHumidity);
    	processing_mem_map_write_s_proces_object_modbus((uint16_t*)&rxData.atmPressure,    1, s_address_oper_data.s_sensor_address.rezPressure_mmHg);
    	processing_mem_map_write_s_proces_object_modbus((uint16_t*)&rxData.atmPressurehPa, 1, s_address_oper_data.s_sensor_address.rezPressure_GPasc);

    }
}


void t_processing_sensor(void *pvParameters){
	S_sensor_user_config *s_sensorUserConfig =(S_sensor_user_config*)pvParameters;
	updateSensorStatus(SENSOR_STATUS_OK);
    // stop task if it disable on configuration
	if(s_sensorUserConfig->state == DISABLE)
	{
		vTaskDelete(NULL);
	}
	if(s_sensorUserConfig->source == METEO_SOURCE_LOCAL)
	{
		processingLocalSensor();
	}
	else
	{
		processingRemoteSensor();
	}
}
