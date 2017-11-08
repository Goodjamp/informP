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
#define I2C_SENSOR_FRQ_HZ 400000

#define I2C1_SCL            GPIO_Pin_6       //PB6 ch1
#define I2C1_SCL_AF_GPIO    GPIO_PinSource6  //PB8 ch1
#define I2C1_SCL_PORT       GPIOB            // CSCL PORT

#define I2C1_SDA            GPIO_Pin_7       //PB7 ch1
#define I2C1_SDA_AF_GPIO    GPIO_PinSource7  //PB7 ch1
#define I2C1_SDA_PORT       GPIOB            // CSDA PORT

#endif // PROCESSING_TC_SINAL
