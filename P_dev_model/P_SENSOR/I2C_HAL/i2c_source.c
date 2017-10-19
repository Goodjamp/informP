/**
  ******************************************************************************
  * @file    i2c_sourse.c
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


void  i2c_config(I2CConfig *configData){
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

	I2C_GenerateSTART(I2C1, DISABLE);
	I2C1->CR1 = 0;

	I2C_InitStruct.I2C_ClockSpeed = configData->fI2C;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			  // I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	  // 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;			  // own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;		  // disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C1, &I2C_InitStruct);				  // init I2C1

	// enable I2C1
	I2C_Cmd(I2C1, ENABLE);

};


//---------function i2c_read_byte-------------------------
// read one byte from remote i2c device
// input arg:
// read_byte - byte to read BYTE
// output:
// I2C_ERROR - result operation:
//                              I2C_OK - read successful
//                              I2C_RECEIVE_BYTE_ERROR - I2C_SR1_RXNE flag not set
I2C_ERROR i2c_read_byte(I2C_TypeDef * I2C_SEL, uint8_t *read_byte){
	if(!i2c_wait_set_flag( &I2C_SEL->SR1, I2C_SR1_RXNE, SET, 10))
	{
		return I2C_RECEIVE_BYTE_ERROR;
	}
	//while(I2C_GetFlagStatus(I2C1,I2C_FLAG_RXNE)==RESET){};
	(*read_byte)=I2C_ReceiveData(I2C1);
	return I2C_OK;
}


//---------function i2c_send_byte-------------------------
// write one byte to remote i2c device
// input arg:
// send_byte - write to BYTE
// output:
// I2C_ERROR - result operation:
//                              I2C_OK - write successful
//                              I2C_SEND_BYTE_ERROR - I2C_SR1_RXNE flag not set
I2C_ERROR i2c_send_byte(I2C_TypeDef * I2C_SEL, u8 send_byte){
	I2C_SendData(I2C1,send_byte);
	//while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BTF)==RESET){};
	if(!i2c_wait_set_flag( &I2C_SEL->SR1, I2C_FLAG_BTF, SET, 10))
	{
		return I2C_SEND_BYTE_ERROR;
	}
	I2C_ReadRegister( I2C_SEL,I2C_Register_SR1);
	return I2C_OK;
}


//---------function i2c_gen_start_send_adres-------------------------
// generate start, send address device
// input arg:
// -
// output:
// I2C_ERROR - result operation:
//                              I2C_OK - write successful
//                              I2C_START_GEN_ERROR
//                              I2C_SEND_ADDRESS_ERROR -
I2C_ERROR i2c_gen_start_send_adres(I2C_TypeDef * I2C_SEL, u8 adres_send, u8 I2C_Direction){

	I2C_GenerateSTART(I2C_SEL,ENABLE);

	if(!i2c_wait_set_flag(&I2C_SEL->SR1, I2C_SR1_SB, SET, 10)){
		return I2C_START_GEN_ERROR;
	}


	I2C_Send7bitAddress(I2C_SEL,adres_send,I2C_Direction);

	if(!i2c_wait_set_flag(&I2C_SEL->SR1, I2C_SR1_ADDR, SET, 10)){
		return I2C_SEND_ADDRESS_ERROR;
	}

	I2C_ReadRegister( I2C_SEL,I2C_Register_SR1);
	I2C_ReadRegister( I2C_SEL,I2C_Register_SR2);
	return I2C_OK;
}


//---------function i2c_wait_set_flag-------------------------
// waite timeout before bit set.
// input arg:
// -
// output:
// uint8_t - result operation:
//                              SET - if first bit set
//                              RESET - if first timeout
uint8_t i2c_wait_set_flag(__IO uint16_t *reg, u32 bit, FlagStatus value_waite,u32 period_ms ){
	u32 cnt=0;
	period_ms=period_ms*15259;
	while(1){
		if(*reg & bit){
			if(value_waite==SET){
				return 1;
			}
		}
		else
		{
			if(value_waite==RESET){
				return 1;
			}
		}
		if(cnt>=period_ms){
			return 0;
		}
		cnt++;
	}
}


//---------function i2c_read_data-------------------------
// read
I2C_ERROR i2c_read_data(void *I2C_SEL_, uint8_t address_dev, uint8_t address_reg, uint8_t num_read, uint8_t *buff){
	u8 cnt; //counter rx bytes
	I2C_TypeDef* I2C_SEL =  (I2C_TypeDef*)I2C_SEL_;
	I2C_GenerateSTOP(I2C_SEL,DISABLE);
	I2C_AcknowledgeConfig(I2C_SEL,ENABLE);

	if(i2c_gen_start_send_adres(I2C_SEL,address_dev, I2C_Direction_Transmitter)){return I2C_READ_ERROR;};
	if(i2c_send_byte(I2C_SEL, address_reg)){return I2C_READ_ERROR;};
	I2C_GenerateSTOP(I2C_SEL,ENABLE);
	if(num_read == 0) // simple command
	{
		return I2C_OK;
	}
	if(i2c_gen_start_send_adres(I2C_SEL,address_dev, I2C_Direction_Receiver)){return 0;};
	for(cnt=0;cnt<num_read;cnt++){
		if(cnt>=(num_read-1))
		{
			I2C_AcknowledgeConfig(I2C_SEL,DISABLE);
			I2C_GenerateSTOP(I2C_SEL,ENABLE);
		}
		if(i2c_read_byte(I2C_SEL,buff))
		{
			return I2C_READ_ERROR;
		};
		buff++;
	}
	return I2C_OK;
};


//---------function i2c_write_data-------------------------
// write
I2C_ERROR i2c_write_data(void *I2C_SEL_, uint8_t address_dev, uint8_t address_reg,uint8_t num_write, uint8_t *buff){
	u8 cnt; //counter rx bytes
	I2C_TypeDef* I2C_SEL =  (I2C_TypeDef*)I2C_SEL_;
	I2C_GenerateSTOP(I2C_SEL,DISABLE);
	I2C_AcknowledgeConfig(I2C_SEL,ENABLE);

	if(i2c_gen_start_send_adres(I2C_SEL,address_dev, I2C_Direction_Transmitter)){return I2C_READ_ERROR;};
	if(i2c_send_byte(I2C_SEL, address_reg)){return I2C_READ_ERROR;};
	if(num_write == 0) // simple command
	{
		I2C_GenerateSTOP(I2C_SEL,ENABLE);
		return I2C_OK;
	}

	// write payload
	for(cnt=0;cnt<num_write;cnt++){
		if(i2c_send_byte(I2C_SEL,(*buff)))
		{
			return I2C_WRITE_ERROR;
		};
		buff++;
	}
	I2C_GenerateSTOP(I2C_SEL,ENABLE);
	return I2C_OK;
};























