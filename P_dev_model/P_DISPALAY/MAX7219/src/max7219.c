/**
  ******************************************************************************
  * @file    max7219.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    22-DEcembefr-2016
  * @brief   This file contains interface for access to max7219 chip
  */

#include "stdio.h"
#include "string.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

#include "stmMaxHardwareInit.h"
#include "max7219.h"
#include "sumbolDesc.h"
#include "fifo_.h"

// static functions description


static typedef struct {
	uint8_t command;
	uint8_t data;
}setPar;

static _max7219Interface  max7219InterfaceGlob = {
								.spiCounter = 0,
							  };
videoUnit videoMem[SIZE_VIDEO_FREIM];

static void max7219SPICallback(SPI_TypeDef *SPIx)
{
	uint8_t cnt = 0;
	max7219Handler *tempMaxHendler;
	// find SPI according input argument in list of activated max7219 interfaces
	for(;cnt<max7219InterfaceGlob.spiCounter;cnt++)
	{
		if(max7219InterfaceGlob.max7219InterfaceHandler[cnt]->selSPI == SPIx)
		{
			break;
		}
	}
	//if not find - return
	if(cnt >= max7219InterfaceGlob.spiCounter)
	{
		SPI_IT_TX_DISABLE(SPIx);
		//max7219InterfaceGlob.max7219InterfaceHandler[cnt]->status = MAX7219_OK;
		return;

	}
	tempMaxHendler = max7219InterfaceGlob.max7219InterfaceHandler[cnt];
	// next data to transmit
	tempMaxHendler->txHandler.txCounter++;
	// if FINISH transmit all data
	if(tempMaxHendler->txHandler.txCounter < tempMaxHendler->txHandler.txSize)
	{
		// continue transmit  data
			SPI_I2S_SendData(tempMaxHendler->selSPI, \
					tempMaxHendler->txHandler.txData[tempMaxHendler->txHandler.txCounter]);
			return;
	}

		//-----Generate LD Pulse
	if(tempMaxHendler->txHandler.txAddress == TX_ADDRESS_ONE){ // If generate on one LD pin
		stmMaxLdPuls(tempMaxHendler->txHandler.portLd,\
					 tempMaxHendler->txHandler.pinLd);
	}
	else                                                      // If generate on one ALL LD pin
	{
		for(cnt = 0;cnt < tempMaxHendler->ldCounter;cnt++)
		{
			stmMaxLdPuls(((max7219LdHandler*)(tempMaxHendler->ldHandler[cnt]))->portLd ,\
					     ((max7219LdHandler*)(tempMaxHendler->ldHandler[cnt]))->pinLd);
		}
	}
	tempMaxHendler->status = MAX7219_OK;
	return;

}


MAX7219_STATUS max7219Tx(max7219LdHandler *max7219Ld, uint8_t numData, uint16_t *pData, TX_ADDRESS txAddress)
{
	if(max7219Ld->interface->status == MAX7219_BUSY)
	{
		return MAX7219_BUSY;
	}
	if(numData == 0)
	{
		return MAX7219_ERROR_TX_NUM;
	}

	max7219Ld->interface->txHandler.pinLd  = max7219Ld->pinLd;
	max7219Ld->interface->txHandler.portLd = max7219Ld->portLd;
	max7219Ld->interface->txHandler.txCounter = 0;
	max7219Ld->interface->txHandler.txSize = numData;
	max7219Ld->interface->txHandler.txData = pData;
	max7219Ld->interface->txHandler.txAddress = txAddress;
	max7219Ld->interface->status = MAX7219_BUSY;


	SPI_I2S_SendData(max7219Ld->interface->selSPI, \
			         (max7219Ld->interface->txHandler.txData[0]));

	return MAX7219_OK;
}


MAX7219_STATUS max7219InterfaceInit(max7219Handler *max7219Interface)
{

	if(stmMaxSPIinit(max7219Interface->selSPI, &max7219SPICallback) != HARDWARE_INIT_STATUS_OK)
	{
		return MAX7219_ERROR_INIT;
	}
	max7219Interface->status = MAX7219_OK;
	max7219Interface->ldCounter = 0;
	// Add new SPI interface for max 7219 in global Interface Description
	max7219InterfaceGlob.max7219InterfaceHandler[max7219InterfaceGlob.spiCounter] = max7219Interface;
	max7219InterfaceGlob.spiCounter++;
	return MAX7219_OK;
}


MAX7219_STATUS max7219AddLd(max7219LdHandler *max7219Ld, max7219Handler *max7219Interface)
{
	if(max7219Interface->ldCounter >= MAX7219_MAX_NUMBER)
	{
		return MAX7219_ERROR_LD_THRESHOLD;
	}
	max7219Interface->ldHandler[max7219Interface->ldCounter] = (void*)max7219Ld;
	max7219Interface->ldCounter++;
	max7219Ld->interface = max7219Interface;
	stmMaxInitGPIOLd(max7219Ld->portLd, max7219Ld->pinLd);
	return MAX7219_OK;
}


/**
  * @brief
  * @param brightnes:  brightnes value
  * @retval None
  */
MAX7219_STATUS max7219GetStatus(max7219Handler const *max7219Interface)
{
	return max7219Interface->status;
}


/*
  * @brief Send string to numDisp over videoMem
  *        last symbols mast bee number
  * @param color:  color of string ref. to the maxColor
  * @retval None
  *
  */
void maxSendString(maxColor color, maxMode mode, uint8_t numDisp ,uint8_t *str){

	uint8_t strLen=strlen((char*)str);



}


/**
  * @brief
  * @param brightnes:  brightnes value
  * @retval None
  */
void maxSetBrightnes(uint8_t brightnes){

}


