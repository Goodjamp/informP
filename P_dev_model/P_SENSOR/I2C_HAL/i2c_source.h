/**
  ******************************************************************************
  * @file    i2c_sourse.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    30-July-2017
  * @brief
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
	I2C_TRANSACTION_TX,
	I2C_TRANSACTION_RX,
}I2C_TRANSACTION_TYPE;


typedef enum{
	I2C_OK,
	I2C_TRANSACTION_PROCESSING,
	I2C_TRANSACTION_ERROR,
}I2C_STATUS;

typedef enum{
	I2C_TX_SB      = 0,
	I2C_TX_ADDRESS = 1,
	I2C_TX_TXE     = 2,
	I2C_TX_BTF     = 3
}I2C_TX_STATE;

typedef enum{
	I2C_RX_SB_F1      = 0,
	I2C_RX_ADDRESS_F1 = 1,
	I2C_RX_BTF_F1     = 2,
	I2C_RX_SB_F2      = 3,
	I2C_RX_ADDRESS_F2 = 4,
	I2C_RX_RXE_F2     = 5,
	I2C_RX_BTF_F2     = 6,
	I2C_RX_RXE_F3     = 7,
}I2C_RX_STATE;

//
typedef struct{

}I2CInterruptStatusDef;


typedef struct{
	I2C_STATUS           transactionStatus; // Transaction status
	I2C_TRANSACTION_TYPE transactionType;   // Type of transaction (Tx/Rx)
	uint8_t              stateCnt;          // counter state of I2C transaction, point on current state (tx/rx - I2C_TX_STATE/I2C_RX_STATE)
	I2C_TypeDef*         I2C_SEL;      //Selectrd I2C
	uint8_t              addressDev;   //Slave address
	uint8_t              addressReg;   //start register address for Tx/Rx data
	uint8_t              numData;      //number of Tx/Rx data
	uint8_t              *buffData;    //buffer for Tx/Rx data
	uint16_t             cnt;          //counter Tx/Rx dataer
}I2CProcessingDef;

typedef struct{
	uint32_t fI2C;
}I2CConfigDef;


I2C_STATUS I2CTxData(I2C_TypeDef* I2C_SEL, uint8_t address_dev, uint8_t address_reg, uint8_t num_read, uint8_t *buff);
I2C_STATUS I2CRxData(I2C_TypeDef* I2C_SEL, uint8_t address_dev, uint8_t address_reg, uint8_t num_read, uint8_t *buff);


I2C_STATUS  i2c_read_data(I2C_TypeDef* I2C_SEL_, uint8_t address_dev, uint8_t address_reg, uint8_t num_read,  uint8_t *buff);
I2C_STATUS i2c_write_data(I2C_TypeDef* I2C_SEL_, uint8_t address_dev, uint8_t address_reg, uint8_t num_write, uint8_t *buff);




#endif /* HP03SA_SOURCE_H_ */
