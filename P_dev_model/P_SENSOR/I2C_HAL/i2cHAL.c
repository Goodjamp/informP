/**
  ******************************************************************************
  * @file    i2cHAL.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    30-July-2017
  * @brief
  */
#include "stdint.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"

#include "i2c_source.h"



typedef enum{
	I2CStartGen,
	I2CTxAddress,
	I2CACKGen,
	I2CTxPayload,
	I2CRxPayload
}I2CState;

typedef struct{
	void *I2CTypeDef;
	uint16_t fI2C;
	I2CState State;
	uint8_t *TxBuf;
	uint8_t *RxBuf;
	uint16_t txSize;
	uint16_t rxSize;
}I2CHandler;


typedef struct{
	uint16_t fI2C;
}I2CConfigDef;

#define NUM_I2C   2

I2CHandler  *I2CInitlList[NUM_I2C];


void  initI2C_(I2CHandler *i2cInitStaruct, I2CHandler *configData ){
	GPIO_InitTypeDef GPIO_InitStructure;
	/*Camera I2C1 pins config*/
	RCC_APB1PeriphClockCmd(RCC_APB2ENR_IOPBEN, ENABLE);

	// config I2C CSCL GPIO
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*Config CSCL functions of I2C1*/
	GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStructure);

	// config I2C CSDA GPIO
	GPIO_InitStructure.GPIO_Pin = ( I2C1_SDA);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);
	// Enable Alternate function
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	//----CONFIG I2C1--------------------------------
	I2C_InitTypeDef I2C_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	I2C_InitStruct.I2C_ClockSpeed = configData->fI2C;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			  // I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	  // 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;			  // own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;		  // disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C1, &I2C_InitStruct);				  // init I2C1

	I2C_ITConfig(I2C1, ( I2C_IT_EVT ), ENABLE);
	NVIC_EnableIRQ(I2C1_EV_IRQn);

	// enable I2C1
	I2C_Cmd(I2C1, ENABLE);

};


void i2c_Reg_Interrupt_CallBack(void){

};

void I2C1_EV_IRQHandler(void){
	// I2C_GetLastEvent()

}
