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
DISPLAY_STATUS displayInterfaceInit( displayHandlerDef volatile *displayHandler)
{
	displayHandler->status = DISPLAY_OK;

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
DISPLAY_STATUS displayTx(displayHandlerDef *displayHandler, uint8_t numData, displayBuffDef *pData )
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
	displayHandler->txBuff.txData    = pData;
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
void displaySet7Segment(uint8_t numMax, uint16_t totalNumMax, uint8_t numDig, uint8_t data, displayBuffDef *maxBuff )
{
	uint8_t cnt = 0;
	for(;cnt < sizeof(symbols7Segments)/sizeof(symbols7Segments[0]); cnt++){
		if( data == symbols7Segments[cnt].symbol){
			// fill buffer
			*((uint16_t*)maxBuff + cnt * (totalNumMax - 1)) = symbols7Segments[cnt].segments;
			//maxBuff[numMax][numDig] = symbols7Segments[cnt].segments;
			return;
		}
	}
}


/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */
void displaySetMatrix(uint8_t numMax, uint16_t totalNumMax, uint8_t data, displayBuffDef *maxBuff)
{
	uint16_t cnt = 0;
	uint16_t *tempBuf = (uint16_t*)maxBuff;
	uint8_t orderDigit = totalNumMax-1;
	for(;cnt < sizeof(symbols8x8Matrix)/sizeof(symbols8x8Matrix[0]); cnt++){
		if( data == symbols8x8Matrix[cnt].symbol){
			//memcpy(maxBuff[numMax], symbols8x8Matrix[cnt].points, )
			//break;
		}
	}
	// Copy symbol in special order
	//for(cnt = 0; cnt < NUM_MAX_DIGITS; cnt++)
	//{
		//*(tempBuf + cnt * orderDigit) = symbols8x8Matrix[cnt].points[cnt];
	//}
}


/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */
void displaySetConfig(uint8_t numMax, maxComandDef maxComand, uint8_t data, displayBuffDef *maxBuff)
{
	maxBuff[numMax][0] = maxSetConfig_(maxComand, data);
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
		maxBuff[cnt][0] = newConfig;
	}
}


/**
  * @brief Set decode for selected digits
  * @param
  * @retval None
  */
void displayConfigDecodeMode(uint8_t numMax, uint8_t data, displayBuffDef *maxBuff)
{
	maxBuff[numMax][0] = maxSetConfig_(DECODE_MODE, data);
}

/**
  * @brief Set Intensity for selected digits
  * @param
  * @retval None
  */
void displayConfigIntensity(uint8_t numMax, maxIntensityDef data, displayBuffDef *maxBuff)
{
	maxBuff[numMax][0] = maxSetConfig_(INTENSITY, data);
}

/**
  * @brief Set SHUTDOWN/NORMAL_OPERATION for selected digits
  * @param
  * @retval None
  */
void displayConfigWorkMode(uint8_t numMax, shutDownDef data, displayBuffDef *maxBuff)
{
	maxBuff[numMax][0] = maxSetConfig_(SHUTDOWN, data);
}


void displayConfigScanLimit(uint8_t numMax, maxScanLimDef data, displayBuffDef *maxBuff)
{
	maxBuff[numMax][0] = maxSetConfig_(SCAN_LIMIT, data);
}
