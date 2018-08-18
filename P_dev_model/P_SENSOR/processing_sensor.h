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
#define REMOTE_DATA_TIMEOUT_MS  (uint32_t)3000
#define NRF_CHANEL              10
/*****************************************************/
#define MESSUREMT_PERIOD            600
#define TIMEOUT_GIST_WIN_SIZE       8
#define TIMEOUT_GIST_SUCSSESS_SIZE  4
#define TIMEOUT_GIST_ERROR_SIZE     6


#define SENSOR_STATUS_OK       (uint16_t)0x0
#define SENSOR_STATUS_NOT_INIT (uint16_t)0xFFFF
// bit position of status local/receiver
typedef enum{
	SENSOR_STATUS_ALLARM_LOCAL         = 0,
	SENSOR_STATUS_ERROR_LOCAL          = 1,
	SENSOR_STATUS_ERROR_RECEIVER       = 2,
	SENSOR_STATUS_ERROR_REM_RX_TIMEOUT = 3,
	SENSOR_STATUS_ERROR_REM_SENSOR     = 4,
	SENSOR_STATUS_ERROR_REM_MES        = 5,
	SENSOR_STATUS_ERROR_REM_BATARY     = 6,
}SENSOR_STATUS;

// bit position of remote meteo post status remote
typedef enum{
	REM_METEO_STATUS_ERROR_SENSOR = 0,
	REM_METEO_STATUS_ERROR_MES    = 1,
	REM_METEO_STATUS_ERROR_BATARY = 2
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
    uint16_t thresholdWindowSize;
    uint16_t thresholdSucssesSize;
    uint16_t thresholdErrorSize;

    uint16_t thresholdSucssesCnt;
    uint16_t thresholdErrorCnt;

    bool     state;
}gistT;

#endif // PROCESSING_TC_SINAL
