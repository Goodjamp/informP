/**
  ******************************************************************************
  * @file    .c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    22-DEcembefr-2016
  * @brief   This file contains interface for access to  chip
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

/**
  * @brief
  * @param
  * @retval None
  */
DISPLAY_STATUS displayInterfaceInit( displayHandlerDef *displayHandler, displayBuffDef *displayBuff)
{
	displayHandler->status = DISPLAY_OK;
	displayHandler->txBuff.txData = displayBuff;

	return DISPLAY_OK;
}

/**
  * @brief
  * @param
  * @retval None
  */
txState displayTxCallback(displayHandlerDef *displayHandler, uint16_t *nexSymbol)
{
	if( displayHandler->txBuff.dataCnt >= displayHandler->txBuff.txSize)
	{
		displayHandler->txBuff.dataCnt = 0;
		displayHandler->txBuff.digitCnt++;
		return GEN_LD;
	}
	if(displayHandler->txBuff.digitCnt >= NUM_MAX_DIGITS){
		// stop Tx
		displayHandler->status = DISPLAY_OK;
		return TX_COMPLITE;
	}
	// Send next data
	*nexSymbol = displayHandler->txBuff.txData[displayHandler->txBuff.dataCnt][displayHandler->txBuff.digitCnt];
	displayHandler->txBuff.dataCnt++;
	return TX_DATA;
}

/**
  * @brief
  * @param
  * @retval None
  */
DISPLAY_STATUS displayTx(displayHandlerDef *displayHandler, uint8_t numData )
{
	if(displayHandler->status == DISPLAY_BUSY)
	{
		return DISPLAY_BUSY;
	}
	if(numData == 0)
	{
		return DISPLAY_ERROR_TX_NUM;
	}

	displayHandler->txBuff.digitCnt  = 0;
	displayHandler->txBuff.dataCnt   = 0;
	displayHandler->txBuff.txSize    = numData;
	displayHandler->status = DISPLAY_BUSY;

	return DISPLAY_OK;
}


/**
  * @brief
  * @param
  * @retval None
  */
DISPLAY_STATUS displayIntarfaceGetStatus(displayHandlerDef const *displayHandler)
{
	return displayHandler->status;
}


/**
  * @brief Fill video buffer for 7 segments indicator
  * @param
  * @retval None
  */
void displayClearBuff(displayHandlerDef *displayIntarface, uint8_t numMax)
{   uint16_t cnt = 0;
	for(;cnt < NUM_MAX_DIGITS; cnt++){
		displayIntarface->txBuff.txData[0][cnt] = maxSetConfig_( (cnt+1), 0);
		displayIntarface->txBuff.txData[1][cnt] = maxSetConfig_( (cnt+1), 0);
	}
}


/**
  * @brief Fill video buffer for 7 segments indicator
  * @param
  * @retval None
  */
void displaySet7Segment(displayHandlerDef *displayIntarface, uint8_t numMax, uint8_t data, uint8_t numDig)
{
	uint8_t cnt = 0;
	for(;cnt < sizeof(symbols7Segments)/sizeof(symbols7Segments[0]); cnt++){
		if( data == symbols7Segments[cnt].symbol){
			displayIntarface->txBuff.txData[numMax][numDig] = maxSetConfig_( (numDig + 1), symbols7Segments[cnt].segments);
			return;
		}
	}
	// if symbol don't fond - set error symbol
	displayIntarface->txBuff.txData[numMax][numDig] = maxSetConfig_( (numDig + 1), error7Segments.segments);
}


/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */
void displaySet8x8Matrix(displayHandlerDef * displayIntarface, uint8_t numMax, uint8_t data)
{
	uint16_t cnt = 0;
	for(;cnt < sizeof(symbols8x8Matrix)/sizeof(symbols8x8Matrix[0]); cnt++){
		if( data == symbols8x8Matrix[cnt].symbol){
			// Copy all symboll
			memcpy( (uint8_t*)displayIntarface->txBuff.txData[numMax], symbols8x8Matrix[cnt].points, sizeof(symbols8x8Matrix[cnt].points) );
			return;
		}
	}
	memcpy( (uint8_t*)displayIntarface->txBuff.txData[numMax], symbols8x8Matrix[cnt].points, sizeof(symbols8x8Matrix[cnt].points) );
}


/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */
void displaySetConfig(displayHandlerDef * displayIntarface, uint8_t numMax,  maxComandDef maxComand, uint8_t data)
{
	displayIntarface->txBuff.txData[numMax][0] = maxSetConfig_(maxComand, data);
}

/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */


void displaySetConfigMulti(displayHandlerDef * displayIntarface, uint8_t sizeBuff, maxComandDef maxComand, uint8_t data)
{
	uint8_t cnt=0;
	uint16_t newConfig = maxSetConfig_(maxComand, data);
	for(; cnt<sizeBuff; cnt++)
	{
		displayIntarface->txBuff.txData[cnt][0] = newConfig;
	}
}


/**
  * @brief Set decode for selected digits
  * @param
  * @retval None
  */
void displayConfigDecodeMode(displayHandlerDef * displayIntarface, uint8_t numMax, uint8_t data)
{
	displayIntarface->txBuff.txData[numMax][0] = maxSetConfig_(DECODE_MODE, data);
}

/**
  * @brief Set Intensity for selected digits
  * @param
  * @retval None
  */
void displayConfigIntensity(displayHandlerDef * displayIntarface, uint8_t numMax, maxIntensityDef data)
{
	displayIntarface->txBuff.txData[numMax][0] = maxSetConfig_(INTENSITY, data);
}

/**
  * @brief Set SHUTDOWN/NORMAL_OPERATION for selected digits
  * @param
  * [displayIntarface] - pointer on the display interface (display - serial connection of few (from one to ...) MAX7219 chips)
  * [numMax]           - order number of MAX7219 in display
  * [data]             - new mode for selected numMax. ref to the "shutDownDef"
  * @retval None
  */
void displayConfigWorkMode(displayHandlerDef * displayIntarface, uint8_t numMax, shutDownDef data)
{
	displayIntarface->txBuff.txData[numMax][0] = maxSetConfig_(SHUTDOWN, data);
}


void displayConfigScanLimit(displayHandlerDef * displayIntarface, uint8_t numMax, maxScanLimDef data)
{
	displayIntarface->txBuff.txData[numMax][0] = maxSetConfig_(SCAN_LIMIT, data);
}
