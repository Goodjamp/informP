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
#include "sumbol8x8Matrix.h"
#include "sumbol7Segments.h"
#include "fifo_.h"


static displayCallbackDef displayCallback;

/**
  * @brief
  * @param
  * @retval None
  */
void displaySetCallback(displayCallbackDef *displayCallbackIn){
	memcpy(&displayCallback, displayCallbackIn, sizeof(displayCallbackDef));
}

/**
  * @brief
  * @param
  * @retval None
  */
void displayTxCallback(displayHandlerDef *displayHandler)
{
	if( displayHandler->txBuff.dataCnt >= displayHandler->txBuff.txSize)
	{
		displayHandler->txBuff.dataCnt = 0;
		displayHandler->txBuff.digitCnt++;
		displayCallback.setLDCB(displayHandler->orderDispl, displayHandler->txBuff.txAddress);
		return;
	}
	if(displayHandler->txBuff.digitCnt >= NUM_MAX_DIGITS){
		// stop Tx
		displayHandler->status = MAX7219_OK;
		return;
	}
	// Send next data
	displayCallback.txDadaCB(displayHandler->txBuff.txData[displayHandler->txBuff.dataCnt][displayHandler->txBuff.digitCnt]);
	displayHandler->txBuff.dataCnt++;
	return;

}

/**
  * @brief
  * @param
  * @retval None
  */
MAX7219_STATUS displayTx(displayHandlerDef *displayHandler, uint16_t orderNumberDispl, uint8_t numData, displayBuffDef *pData, TX_ADDRESS txAddress)
{
	if(displayHandler->status == MAX7219_BUSY)
	{
		return MAX7219_BUSY;
	}
	if(numData == 0)
	{
		return MAX7219_ERROR_TX_NUM;
	}

	displayHandler->orderDispl       = orderNumberDispl;
	displayHandler->txBuff.digitCnt  = 0;
	displayHandler->txBuff.dataCnt   = 0;
	displayHandler->txBuff.txSize    = numData;
	displayHandler->txBuff.txData    = pData;
	displayHandler->txBuff.txAddress = txAddress;
	displayHandler->status = MAX7219_BUSY;
	// start transmit: first digit firs max chip
	displayTxCallback(displayHandler);

	return MAX7219_OK;
}

/**
  * @brief
  * @param
  * @retval None
  */
MAX7219_STATUS displayInterfaceInit(displayHandlerDef *displayHandler, void* displayList, uint16_t sizeDisplayList)
{
	displayHandler->status = MAX7219_OK;
	displayHandler->dispList = displayList;
	displayHandler->displayCnt  = sizeDisplayList;

	return MAX7219_OK;
}


/**
  * @brief
  * @param
  * @retval None
  */
MAX7219_STATUS displayIntarfaceGetStatus(displayHandlerDef const *displayHandler)
{
	return displayHandler->status;
}


/**
  * @brief Fill video buffer for 7 segments indicator
  * @param
  * @retval None
  */
void displaySet7Segment(uint8_t numMax, uint8_t numDig, uint8_t data, displayBuffDef *maxBuff )
{
	uint8_t cnt = 0;
	for(;cnt < sizeof(symbols7Segments)/sizeof(symbols7Segments[0]); cnt++){
		if( data == symbols7Segments[cnt].symbol){
			// fill buffer
			maxBuff[numMax][numDig] = symbols7Segments[cnt].segments;
			return;
		}
	}
}

/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */
void displaySetMatrix(uint8_t numMax, uint8_t data, displayBuffDef *maxBuff)
{
	uint16_t cnt = 0;
	for(;cnt < sizeof(symbols8x8Matrix)/sizeof(symbols8x8Matrix[0]); cnt++){
		if( data == symbols8x8Matrix[cnt].symbol){
			// fill buffer
			memcpy(maxBuff[numMax], &symbols8x8Matrix[cnt].points, sizeof(symbols8x8Matrix[0].points) );
			return;
		}
	}
}


/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */
void displaySetConfig(uint8_t numMax, maxComandDef maxComand, uint8_t data, displayBuffDef *maxBuff)
{
	maxBuff[0][numMax] = maxSetConfig_(maxComand, data);
}

/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */
void displaySetConfigMulti(uint8_t sizeBuff, maxComandDef maxComand, uint8_t data, displayBuffDef *maxBuff)
{
	uint8_t cnt=0;
	uint16_t newConfig = maxSetConfig_(maxComand, data);
	for(; cnt<sizeBuff; cnt++)
	{
		maxBuff[0][cnt] = newConfig;
	}
}


/**
  * @brief Set decode for selected digits
  * @param
  * @retval None
  */
void displayConfigDecodeMode(uint8_t numMax, uint8_t data, displayBuffDef *maxBuff)
{
	maxBuff[0][numMax] = maxSetConfig_(DECODE_MODE, data);
}

/**
  * @brief Set Intensity for selected digits
  * @param
  * @retval None
  */
void displayConfigIntensity(uint8_t numMax, maxIntensityDef data, displayBuffDef *maxBuff)
{
	maxBuff[0][numMax] = maxSetConfig_(INTENSITY, data);
}


