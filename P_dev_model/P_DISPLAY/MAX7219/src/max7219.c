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

#include "max7219.h"
#include "sumbol8x8Matrix.h"
#include "sumbol7Segments.h"

/**
  * @brief Fill video buffer for 7 segments indicator
  * @param
  * @retval None
  */
void displayClearBuff(displayBuffDef *displayBuffer, uint8_t numMax){
	uint16_t cntDig = 0;
	uint16_t cntMax = 0;

	for(;cntDig < NUM_MAX_DIGITS; cntDig++)
	{
		for(cntMax = 0; cntMax < numMax; cntMax++)
		{
			displayBuffer[cntMax][cntDig] = maxSetConfig_((cntDig+1), 0);
		}
	}
}


/**
  * @brief Fill video buffer for 7 segments indicator
  * @param
  * @retval None
  */
void displaySet7Segment(displayBuffDef *displayBuffer, uint8_t numMax, uint8_t data, uint8_t numDig, LAYER_ORDER layerOrder)
{
	uint8_t cnt = 0;
	for(;cnt < sizeof(symbols7Segments)/sizeof(symbols7Segments[0]); cnt++){
		if( data == symbols7Segments[cnt].symbol){
			if(layerOrder == LAYER_ORDER_FIRST)
			{
			    displayBuffer[numMax][numDig] = maxSetConfig_( (numDig + 1), symbols7Segments[cnt].segments);
			}
			else
			{
				displayBuffer[numMax][numDig] |= maxSetConfig_( (numDig + 1), symbols7Segments[cnt].segments);
			}
			return;
		}
	}
	// if symbol don't fond - set error symbol
	displayBuffer[numMax][numDig] = maxSetConfig_( (numDig + 1), error7Segments.segments);
}


/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */
void displaySet8x8Matrix(displayBuffDef *displayBuffer, uint8_t numMax, uint8_t data, LAYER_ORDER layerOrder)
{
	uint16_t cnt, cnt2;
	for(cnt = 0 ;cnt < sizeof(symbols8x8Matrix)/sizeof(symbols8x8Matrix[0]); cnt++){
		if( data == symbols8x8Matrix[cnt].symbol){
			// Copy
			if( LAYER_ORDER_FIRST == layerOrder ){
			    memcpy( (uint8_t*)displayBuffer[numMax], symbols8x8Matrix[cnt].points, sizeof(symbols8x8Matrix[cnt].points) );
			} else {
                for(cnt2 = 0; cnt2 < NUM_MAX_DIGITS; cnt2++){
                    displayBuffer[numMax][cnt2] |= symbols8x8Matrix[cnt].points[cnt2];
                }
			}
			return; // if symbol was found - return
		}
	}
	// if symbol was not found, copy
   memcpy( (uint8_t*)displayBuffer[numMax], symbols8x8Matrix[cnt].points, sizeof(symbols8x8Matrix[cnt].points) );
}


/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */
void displaySetConfig(displayBuffDef *displayBuffer, uint8_t numMax,  maxComandDef maxComand, uint8_t data)
{
	displayBuffer[numMax][0] = maxSetConfig_(maxComand, data);
}

/**
  * @brief Fill video buffer for 8X8 matrix  indicator
  * @param
  * @retval None
  */


void displaySetConfigMulti(displayBuffDef *displayBuffer, uint8_t sizeBuff, maxComandDef maxComand, uint8_t data)
{
	uint8_t cnt=0;
	uint16_t newConfig = maxSetConfig_(maxComand, data);
	for(; cnt<sizeBuff; cnt++)
	{
		displayBuffer[cnt][0] = newConfig;
	}
}


/**
  * @brief Set decode for selected digits
  * @param
  * @retval None
  */
void displayConfigDecodeMode(displayBuffDef *displayBuffer, uint8_t numMax, uint8_t data)
{
	displayBuffer[numMax][0] = maxSetConfig_(DECODE_MODE, data);
}

/**
  * @brief Set Intensity for selected digits
  * @param
  * @retval None
  */
void displayConfigIntensity(displayBuffDef *displayBuffer, uint8_t numMax, maxIntensityDef data)
{
	displayBuffer[numMax][0] = maxSetConfig_(INTENSITY, data);
}


void displayConfigScanLimit(displayBuffDef *displayBuffer, uint8_t numMax, maxScanLimDef data)
{
	displayBuffer[numMax][0] = maxSetConfig_(SCAN_LIMIT, data);
}



/**
  * @brief Set SHUTDOWN/NORMAL_OPERATION
  * @param
  * [displayIntarface] - pointer on the display interface (display - serial connection of few (from one to ...) MAX7219 chips)
  * [numMax]           - order number of MAX7219 in display
  * [data]             - new mode for selected numMax. ref to the "shutDownDef"
  * @retval None
  */
void displayConfigWorkMode(displayBuffDef *displayBuffer, uint8_t numMax, shutDownDef data)
{
	displayBuffer[numMax][0] = maxSetConfig_(SHUTDOWN, data);
}


/**
  * @brief Set DISPLAY_TEST_NORMAL_OPERATION/DISPLAY_TEST_DISPLAY_TEST_MODE
  * @param
  * [displayIntarface] - pointer on the display interface (display - serial connection of few (from one to ...) MAX7219 chips)
  * [numMax]           - order number of MAX7219 in display
  * [data]             - new mode for selected numMax. ref to the "shutDownDef"
  * @retval None
  */
void displayConfigTestMode(displayBuffDef *displayBuffer, uint8_t numMax, maxTestDef data)
{
	displayBuffer[numMax][0] = maxSetConfig_(DISPLAY_TEST, data);
}



