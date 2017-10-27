/**
  ******************************************************************************
  * @file    i2c_sourse.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    30-July-2017
  * @brief
  */

#include "stdint.h"
#include "stdbool.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"

#include "i2c_source.h"
// After sending the START condition (I2C_GenerateSTART() function) the master
static volatile I2CProcessingDef I2CProcessing;
// static functions prototype
static void I2CfillProcessingBuff(I2C_TypeDef* I2C_SEL, uint8_t address_dev, uint8_t address_reg, uint8_t num_read, uint8_t *buff,I2C_TRANSACTION_TYPE  typeTransaction);
static inline void I2CProcessingInterrupRx(void);
static inline void I2CProcessingInterrupTx(void);
static void (*fI2CInterruptProcessing[2])(void) = {
		[I2C_TRANSACTION_RX] = I2CProcessingInterrupRx,
		[I2C_TRANSACTION_TX] = I2CProcessingInterrupTx
};


#define debugPin1  GPIO_Pin_2
#define debugPin2  GPIO_Pin_3
#define debugPort1 GPIOA
#define debugPort2 GPIOA
#define debugPin_1_Set     GPIO_SetBits(debugPort1, debugPin1)
#define debugPin_1_Clear   GPIO_ResetBits(debugPort1, debugPin1)
#define debugPin_2_Set     GPIO_SetBits(debugPort2, debugPin2)
#define debugPin_2_Clear   GPIO_ResetBits(debugPort2, debugPin2)

void debugPinConfig(void){
	GPIO_InitTypeDef initGpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	initGpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	initGpio.GPIO_Speed = GPIO_Speed_10MHz;
	initGpio.GPIO_Pin   = debugPin1;
	GPIO_Init(debugPort1, &initGpio);

	initGpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	initGpio.GPIO_Speed = GPIO_Speed_10MHz;
	initGpio.GPIO_Pin   = debugPin2;
	GPIO_Init(debugPort2, &initGpio);
}

I2C_STATUS I2CTxData(I2C_TypeDef* I2C_SEL, uint8_t address_dev, uint8_t address_reg, uint8_t num_read, uint8_t *buff){
	if(I2CProcessing.transactionStatus != I2C_OK){
		return I2CProcessing.transactionStatus;
	}
	I2CfillProcessingBuff(I2C_SEL, address_dev, address_reg, num_read, buff, I2C_TRANSACTION_TX);
	while( (I2CProcessing.transactionStatus == I2C_TRANSACTION_PROCESSING) || (I2C_ReadRegister(I2CProcessing.I2C_SEL, I2C_Register_CR1) & I2C_CR1_STOP)){}
	I2C_Cmd(I2CProcessing.I2C_SEL, DISABLE);
	return I2CProcessing.transactionStatus;
}

I2C_STATUS I2CRxData(I2C_TypeDef* I2C_SEL, uint8_t address_dev, uint8_t address_reg, uint8_t num_read, uint8_t *buff){
	if(I2CProcessing.transactionStatus != I2C_OK){
		return I2CProcessing.transactionStatus;
	}
	debugPin_1_Clear;
	I2CfillProcessingBuff(I2C_SEL, address_dev, address_reg, num_read, buff, I2C_TRANSACTION_RX);
	I2C_AcknowledgeConfig(I2C_SEL, ENABLE);
	while( I2CProcessing.transactionStatus == I2C_TRANSACTION_PROCESSING ){};
	debugPin_1_Set;
	I2C_Cmd(I2CProcessing.I2C_SEL, DISABLE);
	return I2CProcessing.transactionStatus;
}

I2C_STATUS I2CGetStatus(void){
	return I2CProcessing.transactionStatus;
}

void  i2c_config(I2C_TypeDef *Sel_I2C ,I2CConfigDef *configData){
	debugPinConfig();

	GPIO_InitTypeDef GPIO_InitStructure;
	/*Camera I2C1 pins config*/
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

	//----CONFIG I2C1--------------------------------
	I2C_InitTypeDef I2C_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	I2C_InitStruct.I2C_ClockSpeed = configData->fI2C;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			  // I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	  // 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;			  // own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;		  // disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(Sel_I2C, &I2C_InitStruct);				  // init I2C1
	// Enable interrupt
	if(Sel_I2C == I2C1)
	{
		NVIC_EnableIRQ(I2C1_EV_IRQn);
	}
	else if(Sel_I2C == I2C2)
	{
		NVIC_EnableIRQ(I2C2_EV_IRQn);
	}
};


static void I2CfillProcessingBuff(I2C_TypeDef* I2C_SEL, uint8_t address_dev, uint8_t address_reg,
		                          uint8_t num_read, uint8_t *buff, I2C_TRANSACTION_TYPE typeTransaction){
	// Enable All Event interupts

	I2CProcessing.transactionStatus = I2C_TRANSACTION_PROCESSING;
	I2CProcessing.transactionType = typeTransaction;
	I2CProcessing.I2C_SEL = I2C_SEL;
	I2CProcessing.addressDev = address_dev;
	I2CProcessing.addressReg = address_reg;
	I2CProcessing.buffData = buff;
	I2CProcessing.numData = num_read;
	I2CProcessing.cnt = 0;
	// Start Tx data
	I2C_Cmd(I2CProcessing.I2C_SEL, ENABLE);
	I2C_ITConfig(I2C_SEL, I2C_IT_EVT, ENABLE);
	I2C_GenerateSTART(I2CProcessing.I2C_SEL, ENABLE);
}

static inline bool clearI2CComunication(uint16_t I2C_SR1Field){
	if( (I2C_ReadRegister(I2CProcessing.I2C_SEL, I2C_Register_SR1)) & I2C_SR1Field )
	{
		return true;
	}
	I2CProcessing.transactionStatus = I2C_TRANSACTION_ERROR;
	I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_BUF, DISABLE);
	I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_EVT, DISABLE);
	I2CProcessing.stateCnt = 0;
	return false;
}


void I2C1_EV_IRQHandler(void){
	fI2CInterruptProcessing[I2CProcessing.transactionType]();
}

//Function: I2CProcessingInterrupRx
//Description:State machine for TRANSMITE data to I2C device. Called from I2C interrupt
static inline void I2CProcessingInterrupTx(void){
	uint32_t status;
	switch(I2CProcessing.stateCnt){
	case I2C_TX_SB:
		// first interrupt should be completed SB generation
		if( !clearI2CComunication(I2C_SR1_SB))
		{
			return;
		};
		I2C_GenerateSTART(I2CProcessing.I2C_SEL, DISABLE);
		I2C_Send7bitAddress(I2CProcessing.I2C_SEL, (uint8_t)(I2CProcessing.addressDev<<1), I2C_Direction_Transmitter);
		I2CProcessing.stateCnt++;
		break;
	case I2C_TX_ADDRESS:
		if( !clearI2CComunication(I2C_SR1_ADDR))
		{
			return;
		};
		I2C_ReadRegister(I2CProcessing.I2C_SEL, I2C_Register_SR2);
		// Enable All buffer interupts
		I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_BUF, ENABLE);
		// Send address register
		I2C_SendData(I2CProcessing.I2C_SEL, I2CProcessing.addressReg);
		I2CProcessing.stateCnt++;
		break;
	case I2C_TX_TXE:
		if( !clearI2CComunication(I2C_SR1_TXE))
		{
			return;
		};
		// Send data
		if(I2CProcessing.cnt >= I2CProcessing.numData)
		{
			I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_BUF, DISABLE);
			I2CProcessing.stateCnt++;
			break;
		}
		I2C_SendData(I2CProcessing.I2C_SEL, I2CProcessing.buffData[I2CProcessing.cnt]);

		I2CProcessing.cnt++;
		break;
	case I2C_TX_BTF:
		if( !clearI2CComunication(I2C_SR1_BTF))
		{
			return;
		};
		I2C_GenerateSTOP(I2CProcessing.I2C_SEL, ENABLE);
		I2CProcessing.transactionStatus = I2C_OK;
		I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_BUF, DISABLE);
		I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_EVT, DISABLE);
		I2CProcessing.stateCnt = 0;
		break;
	}
}


//Function: I2CProcessingInterrupRx
//Description:State machine for receive data from I2C device. Called from I2C interrupt
static inline void I2CProcessingInterrupRx(void){
	uint32_t status;// = I2C1->SR1;
	switch(I2CProcessing.stateCnt){
	case I2C_RX_SB_F1:
		// first interrupt should be completed SB generation
		if( !clearI2CComunication(I2C_SR1_SB))
		{
			return;
		};
		I2C_Send7bitAddress(I2CProcessing.I2C_SEL, (uint8_t)(I2CProcessing.addressDev<<1), I2C_Direction_Transmitter);
		I2CProcessing.stateCnt++;
		break;
	case I2C_RX_ADDRESS_F1:
		if( !clearI2CComunication(I2C_SR1_ADDR))
		{
			return;
		};
		I2C_ReadRegister(I2CProcessing.I2C_SEL, I2C_Register_SR2);
		// Send start address register
		I2C_SendData(I2CProcessing.I2C_SEL, I2CProcessing.addressReg);
		I2CProcessing.stateCnt++;
		break;
	case I2C_RX_BTF_F1:
		if( !clearI2CComunication(I2C_SR1_BTF))
		{
			return;
		};
		I2C_ReceiveData(I2CProcessing.I2C_SEL);
		I2C_GenerateSTART(I2CProcessing.I2C_SEL, ENABLE);
		I2CProcessing.stateCnt++;
		break;
	case I2C_RX_SB_F2:
		status = I2C1->SR1;
		if( !clearI2CComunication(I2C_SR1_SB))
		{
			return;
		};
		I2C_Send7bitAddress(I2CProcessing.I2C_SEL, (uint8_t)(I2CProcessing.addressDev<<1), I2C_Direction_Receiver);
		I2CProcessing.stateCnt++;
		break;
	case I2C_RX_ADDRESS_F2:
		if( !clearI2CComunication(I2C_SR1_ADDR))
		{
			return;
		};
		// SR1 was read in clearI2CComunication function
		I2C_ReadRegister(I2CProcessing.I2C_SEL, I2C_Register_SR2);
		// Enable buffer interrupt
		I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_BUF, ENABLE);
		if(I2CProcessing.numData == 1)
		{
			I2C_AcknowledgeConfig(I2CProcessing.I2C_SEL, DISABLE);
		}
		I2CProcessing.stateCnt++;
		break;
	case I2C_RX_RXE_F2:
		if( !clearI2CComunication(I2C_SR1_RXNE))
		{
			return;
		};
		I2CProcessing.buffData[I2CProcessing.cnt] = I2C_ReceiveData(I2CProcessing.I2C_SEL);
		I2CProcessing.cnt++;
		if( I2CProcessing.cnt == (I2CProcessing.numData - 3 ))
		{
			I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_BUF, DISABLE);
			I2CProcessing.stateCnt++;
		}
		break;
	case I2C_RX_BTF_F2:
		if( !clearI2CComunication(I2C_SR1_BTF))
		{
			return;
		};
		debugPin_1_Set;
		I2C_AcknowledgeConfig(I2CProcessing.I2C_SEL, DISABLE);
		// Receive data from DR register and shift register
		I2CProcessing.buffData[I2CProcessing.cnt++] = I2C_ReceiveData(I2CProcessing.I2C_SEL);
		I2C_GenerateSTOP(I2CProcessing.I2C_SEL, ENABLE);
		I2CProcessing.buffData[I2CProcessing.cnt++] = I2C_ReceiveData(I2CProcessing.I2C_SEL);
		I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_BUF, ENABLE);
		debugPin_1_Clear;
		I2CProcessing.stateCnt++;
		break;
	case I2C_RX_RXE_F3:
		if( !clearI2CComunication(I2C_SR1_RXNE))
		{
			return;
		};
		debugPin_2_Set;
		I2CProcessing.buffData[I2CProcessing.cnt] = I2C_ReceiveData(I2CProcessing.I2C_SEL);
		I2CProcessing.transactionStatus = I2C_OK;
		I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_BUF, DISABLE);
		I2C_ITConfig(I2CProcessing.I2C_SEL, I2C_IT_EVT, DISABLE);
		I2CProcessing.stateCnt = 0;
		debugPin_2_Clear;
		break;
	}
}


I2C_STATUS  i2c_read_data(I2C_TypeDef* I2C_SEL_, uint8_t address_dev, uint8_t address_reg, uint8_t num_read,  uint8_t *buff){};
I2C_STATUS i2c_write_data(I2C_TypeDef* I2C_SEL_, uint8_t address_dev, uint8_t address_reg, uint8_t num_write, uint8_t *buff){};
