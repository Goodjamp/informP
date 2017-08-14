/*
*  i2c_sourse.h
 *
 *  Created on: July 30, 2017
 *      Author: Gerasimchuk
 *      Versin: 1
 */
#ifndef I2C_SOURCE_H_
#define I2C_SOURCE_H_

#include "stdint.h"

#include "stm32f10x.h"

#define I2C1_SCL            GPIO_Pin_6       //PB6 ch1
#define I2C1_SCL_AF_GPIO    GPIO_PinSource6  //PB8 ch1
#define I2C1_SCL_PORT       GPIOB            // CSCL PORT

#define I2C1_SDA            GPIO_Pin_7       //PB7 ch1
#define I2C1_SDA_AF_GPIO    GPIO_PinSource7  //PB7 ch1
#define I2C1_SDA_PORT       GPIOB            // CSDA PORT

typedef enum{
	I2C_OK=0,
	I2C_START_GEN_ERROR=1,
	I2C_SEND_ADDRESS_ERROR=2,
	I2C_SEND_BYTE_ERROR=3,
	I2C_RECEIVE_BYTE_ERROR=4,
	I2C_READ_ERROR=5,
	I2C_WRITE_ERROR=6
}I2C_ERROR;


typedef struct{
	uint8_t  pid;
	uint8_t  ver;
}S_DidVer;

typedef struct{
	uint16_t fI2C;
}I2CConfig;


void 	  i2c_config(I2CConfig *configData);
I2C_ERROR i2c_read_byte(I2C_TypeDef * I2C_SEL, u8 *read_byte);
I2C_ERROR i2c_send_byte(I2C_TypeDef * I2C_SEL, u8 send_byte);
I2C_ERROR i2c_gen_start_send_adres(I2C_TypeDef * I2C_SEL, u8 adres_send, u8 I2C_Direction);
uint8_t  i2c_wait_set_flag(__IO uint16_t *reg, u32 bit, FlagStatus value_waite,u32 period_ms );
I2C_ERROR i2c_read_data(void *I2C_SEL,uint8_t num_read, uint8_t address_dev, uint8_t address_reg, uint8_t *buff);
I2C_ERROR i2c_write_data(void *I2C_SEL, uint8_t address_dev, uint8_t address_reg,uint8_t num_write, uint8_t *buff);

#endif /* HP03SA_SOURCE_H_ */
