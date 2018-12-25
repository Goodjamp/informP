/**
  ******************************************************************************
  * @file    processing_sensor.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    30-July-2017
  * @brief
  */

#ifndef PROCESSING_FEQMETTER_H_
#define PROCESSING_FEQMETTER_H_

#include "i2c_user_interface.h"
#include "stdbool.h"

#define I2C_SENSOR        I2C_1
#define I2C_SENSOR_FRQ_HZ 20000

#define I2C1_SCL            GPIO_Pin_6       //PB6 ch1
#define I2C1_SCL_AF_GPIO    GPIO_PinSource6  //PB8 ch1
#define I2C1_SCL_PORT       GPIOB            // CSCL PORT

#define I2C1_SDA            GPIO_Pin_7       //PB7 ch1
#define I2C1_SDA_AF_GPIO    GPIO_PinSource7  //PB7 ch1
#define I2C1_SDA_PORT       GPIOB            // CSDA PORT
// Transform atmospheric pressure from Pascal to mmHg
#define PASCAL_TO_MMHG_COEF 0.00750062


/**********NRF24L01 default paramiters****************/
#define NRF_ADDRESS             (uint8_t*)"METEO"
#define NRF_CHANEL              10

#define MESSUREMT_PERIOD_MS         1000
/**Hysteresis settings for receive result from remote meteo post****/
#define TIMEOUT_GIST_WIN_SIZE       128
#define TIMEOUT_GIST_SUCSSESS_SIZE  1
/***************************************************************/
#define SENSOR_STATUS_OK       (uint16_t)0x0
#define SENSOR_STATUS_NOT_INIT (uint16_t)0xFFFF

// bit position of remote meteo post status remote
typedef enum{
	REM_METEO_STATUS_ERROR_SENSOR       = 0,
    REM_METEO_STATUS_RAINE              = 1,
    REM_METEO_STATUS_ERROR_RAINE_SENSOR = 2,
	REM_METEO_STATUS_ERROR_BATARY       = 3
}REM_METEO_STATUS;

//source of meteo data
typedef enum{
	METEO_SOURCE_LOCAL  = 0,
	METEO_SOURCE_REMOTE = 1
}METEO_SOURCE;


#pragma pack(push, 1)
typedef struct
{
	uint8_t status;
    int16_t temperature;
    uint16_t humifity;
    uint16_t atmPressure;
    uint16_t atmPressurehPa;
}transactionT;
#pragma pack(pop)

typedef union
{
	uint8_t      *buf;
	transactionT *data;
}transactionBuffT;

typedef struct
{
    bool ringBuffRez[TIMEOUT_GIST_WIN_SIZE];
    uint16_t posWrite;
    uint16_t cntTrue;
}gistT;

#endif // PROCESSING_TC_SINAL
