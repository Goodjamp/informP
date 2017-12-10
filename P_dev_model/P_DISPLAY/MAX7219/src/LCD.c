/********************************************************************************
 * @file    LCD.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    15-November-2017
 * @brief
 */

#include "stdint.h"

#include "LCD.h"
#include "max7219.h"
#include "debugStuff.h"

volatile displayHandlerDef *displayHandler;

/**
  * @brief
  * @param
  * @retval None
  */
DISPLAY_STATUS displayInit( displayHandlerDef *displayHandlerIn)
{
	displayHandlerIn->status = DISPLAY_OK;
	hwInterfaceInit();
	displayClear(displayHandlerIn, 0, TX_ADDRESS_ALL);
	displaySetDefConfig(displayHandlerIn);
	return DISPLAY_OK;
}

/**
  * @brief
  * @param
  * @retval None
  */
txState getNextData(uint16_t *nexSymbol){
	if( displayHandler->dataCnt >= MAX_PER_SCREEN)
	{
		debugPin_2_Set;
		displayHandler->dataCnt = 0;
		displayHandler->digitCnt++;
		debugPin_2_Clear;
		return GEN_LD;
	}
	if(displayHandler->digitCnt >= displayHandler->numForTx){
		// stop Tx
		debugPin_1_Set;
		displayHandler->status = DISPLAY_OK;
		debugPin_1_Clear;
		return TX_COMPLITE;
	}
	// Send next data
	*nexSymbol = displayHandler->txData[displayHandler->dataCnt][displayHandler->digitCnt];
	displayHandler->dataCnt++;
	return TX_DATA;
}

/**
  * @brief
  * @param
  * @retval None
  */
static DISPLAY_STATUS displayTxFrame(displayHandlerDef *displayHandlerIn, uint16_t numScreen,  TX_ADDRESS txAddress )
{
	if(displayHandler->status == DISPLAY_BUSY)
	{
		return DISPLAY_BUSY;
	}

	displayHandler  = displayHandlerIn;
	displayHandler->digitCnt = 0;
	displayHandler->dataCnt = 0;
	displayHandler->numForTx = MAX_NUM_DATA_SEND_FRAME;
	displayHandler->status = DISPLAY_BUSY;
	hwInterfaceTx(numScreen, txAddress);

	return DISPLAY_OK;
}


/**
  * @brief
  * @param
  * @retval None
  */
static DISPLAY_STATUS displayTxCommand(displayHandlerDef *displayHandlerIn, uint16_t numScreen,  TX_ADDRESS txAddress )
{
	if(displayHandler->status == DISPLAY_BUSY)
	{
		return DISPLAY_BUSY;
	}

	displayHandler  = displayHandlerIn;
	displayHandler->digitCnt = 0;
	displayHandler->dataCnt = 0;
	displayHandler->numForTx = MAX_NUM_DATA_SEND_COMMAND;
	displayHandler->status = DISPLAY_BUSY;
	hwInterfaceTx(numScreen, txAddress);

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
  * @brief This function implement parsing string according next template: max#1 - 8x8 matrix, max#2 - 7 segment 4 ripple colors digits
  * @param
  * [displayHandler]  display handler (should be initialized before used)
  * [numScreen]       order number of the screen (associate with order number LD pin)
  * [str]             ASCII string
  * [strSize]         size of str
  * [color]           selected color of screen
  * [txAddress]       address of str: for all screen inside displayHandler, or only one - numScreen
  * @retval
  */
void displayWrite(displayHandlerDef *displayHandlerIn, uint16_t numScreen, uint8_t *str, uint16_t strSize, COLOR color, TX_ADDRESS txAddress)
{
	uint16_t cnt = 0;
	uint16_t digCnt = 0;
	LAYER_ORDER layer;
	// For current screen configuration:
	// first max7219  - 8x8 matrix indicator
	// Second max7219 - 7-segment indicator
	displayClearBuff(displayHandlerIn->txData,MAX_PER_SCREEN);
	// set max#1 sumbol - 8x8 matrix
	displaySet8x8Matrix(displayHandlerIn->txData, ORDER_NUM_MATRIX,  str[ORDER_NUM_MATRIX] );
	// set max#2 - 7 segment 4 ripple colors digits
	for(cnt = 0 ; cnt < strSize; cnt++)
	{
		if(str[cnt + 1] >= 128 ){ // current symbol in first layer
			digCnt--;
			str[cnt + 1] &= 0b1111111;
			layer = LAYER_ORDER_SECOND;
		}
		else
		{
			layer = LAYER_ORDER_FIRST;
		}
		switch (color){
		case COLOR_GREEN:
			displaySet7Segment(  displayHandlerIn->txData, ORDER_NUM_7SEG, str[cnt + 1],  digCnt, layer);
			break;
		case COLOR_RED:
			displaySet7Segment(  displayHandlerIn->txData, ORDER_NUM_7SEG, str[cnt + 1],  digCnt + 4, layer);
			break;
		case COLOR_ORANGE:
			displaySet7Segment(  displayHandlerIn->txData, ORDER_NUM_7SEG, str[cnt + 1],  digCnt, layer);
			displaySet7Segment(  displayHandlerIn->txData, ORDER_NUM_7SEG, str[cnt + 1],  digCnt + 4, layer);
			break;
		}

		digCnt++;
	}
	displayTxFrame(displayHandlerIn, numScreen, txAddress);
	while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}
}


/**
  * @brief
  * @param
  * @retval
  */
void displaySetDefConfig(displayHandlerDef *displayHandlerIn)
{
	uint8_t cnt;

	// Shut down test mode
	for(cnt = 0; cnt < MAX_PER_SCREEN; cnt++)
	{
		displayConfigTestMode(displayHandlerIn->txData, cnt, DISPLAY_TEST_NORMAL_OPERATION);
	}
	displayTxCommand(displayHandlerIn, 0, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}

	//Set scan all
	for(cnt = 0; cnt < MAX_PER_SCREEN; cnt++)
	{
	    displayConfigScanLimit(displayHandlerIn->txData, cnt, SCAN_LIM_7);
	}
	displayTxCommand(displayHandlerIn, 0, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}

	// Set work mode
	for(cnt = 0; cnt < MAX_PER_SCREEN; cnt++)
	{
		displayConfigWorkMode(displayHandlerIn->txData, cnt, SHUT_DOWN_NORMAL_OPERATION);
	}
	displayTxCommand(displayHandlerIn, 0, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}

}

/**
  * @brief This function clear
  * @param
  * [displayHandler]  display handler (should be initialized before used)
  * [numScreen]       order number of the screen (associate with order number LD pin)
  * [txAddress]       address of str: for all screen inside displayHandler, or only one - numScreen
  * @retval
  */
void displayClear(displayHandlerDef *displayHandlerIn, uint16_t numScreen, TX_ADDRESS txAddress){
	displayClearBuff(displayHandlerIn->txData, MAX_PER_SCREEN);
	displayTxFrame(displayHandlerIn, numScreen, txAddress);
	while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}
}


/**
  * @brief This function clear
  * @param
  * [displayHandler]  display handler (should be initialized before used)
  * [numScreen]       order number of the screen (associate with order number LD pin)
  * [txAddress]       address of str: for all screen inside displayHandler, or only one - numScreen
  * @retval
  */
void displaySetBrightnes(displayHandlerDef *displayHandlerIn, DISPLAY_BRIGHTNES brightnes, uint16_t numScreen, TX_ADDRESS txAddress){
    uint8_t cnt;
	for(cnt = 0; cnt < MAX_PER_SCREEN; cnt++)
	{
	    displayConfigIntensity(displayHandlerIn->txData, cnt, brightnes);
	}
	displayTxCommand(displayHandlerIn, numScreen, txAddress);
	while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}
}

