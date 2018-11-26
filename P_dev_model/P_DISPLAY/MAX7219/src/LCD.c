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

static volatile displayHandlerDef *displayHandler;


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
static DISPLAY_STATUS displayTxCommand(displayHandlerDef *displayHandlerIn, uint16_t numString,  TX_ADDRESS txAddress )
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
	hwInterfaceTx(numString, txAddress);

	return DISPLAY_OK;
}

static void displaySetScanLim(displayHandlerDef *displayHandlerIn, uint8_t numString){

	displayConfigScanLimit(displayHandlerIn->txData, 0, SCAN_LIM_7);
	switch( displayHandlerIn->currentSettings.color[numString]){
	case COLOR_GREEN:
	    displayConfigScanLimit(displayHandlerIn->txData, 1, SCAN_LIM_3);
		break;
	case COLOR_ORANGE:
	case COLOR_RED:
		displayConfigScanLimit(displayHandlerIn->txData, 1, SCAN_LIM_7);
		break;
	}
	displayTxCommand(displayHandlerIn, numString, TX_ADDRESS_ONE);
	while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){};

}

/**
  * @brief
  * @param
  * @retval None
  */
DISPLAY_STATUS displayInit( displayHandlerDef *displayHandlerIn, const uint8_t* brightnesPar, uint8_t numberOfActiveDisplay)
{
	uint8_t cnt;
	displayHandlerIn->brightnesList = brightnesPar;
	displayHandlerIn->currentSettings.brightnes = 2;
	displayHandlerIn->currentSettings.numberOfActivDisplay = numberOfActiveDisplay;
	// default color of ALL strings
	for(cnt = 0; cnt < NUMBER_OF_LCD_STRING; cnt++){
		displayHandlerIn->currentSettings.color[cnt] = COLOR_GREEN;
	}

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
		displayHandler->dataCnt = 0;
		displayHandler->digitCnt++;
		return GEN_LD;
	}
	if(displayHandler->digitCnt >= displayHandler->numForTx){
		displayHandler->status = DISPLAY_OK;
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
void displayWrite(displayHandlerDef *displayHandlerIn, uint16_t numString, uint8_t *str, uint16_t strSize, COLOR color, TX_ADDRESS txAddress)
{
	uint8_t seg7Shift = 1;
	uint16_t cnt = 0;
	uint16_t digCnt = 0;
	LAYER_ORDER layer;
	// For current screen configuration:
	// first  max7219  - 8x8 matrix indicator
	// second max7219 - 7-segment indicator
	displayClearBuff(displayHandlerIn->txData,MAX_PER_SCREEN);
	// set max#1 symbol - 8x8 matrix(maximum 2 layer suport)
	displaySet8x8Matrix(displayHandlerIn->txData, ORDER_NUM_MATRIX,  str[ORDER_NUM_MATRIX], LAYER_ORDER_FIRST );
	// second matrix layer
    if(str[ORDER_NUM_MATRIX+1] >  0x80)
    {
    	str[ORDER_NUM_MATRIX + 1] &= 0b1111111;
    	displaySet8x8Matrix(displayHandlerIn->txData, ORDER_NUM_MATRIX,  str[ORDER_NUM_MATRIX + 1], LAYER_ORDER_SECOND );
    	seg7Shift++;
    }
	// set max#2 - 7 segment 4 ripple colors digits
	for(cnt = 0 ; cnt < (strSize-1); cnt++)
	{
		if(str[cnt + seg7Shift] >= 0x80 ){ // current symbol in first layer
			digCnt--;
			str[cnt + seg7Shift] &= 0b1111111;
			layer = LAYER_ORDER_SECOND;
		}
		else
		{
			layer = LAYER_ORDER_FIRST;
		}

		// update scan limit parameters
		if( displayHandlerIn->currentSettings.color[numString] !=  color )
		{
			displayHandlerIn->currentSettings.color[numString] =  color;
			displaySetScanLim(displayHandlerIn,numString);
		}

		// set new value for selected screen
		switch (color){
		case COLOR_GREEN:
			displaySet7Segment(  displayHandlerIn->txData, ORDER_NUM_7SEG, str[cnt + seg7Shift],  digCnt,     layer);
			break;
		case COLOR_RED:
			displaySet7Segment(  displayHandlerIn->txData, ORDER_NUM_7SEG, str[cnt + seg7Shift],  digCnt + 4, layer);
			break;
		case COLOR_ORANGE:
			displaySet7Segment(  displayHandlerIn->txData, ORDER_NUM_7SEG, str[cnt + seg7Shift],  digCnt,     layer);
			displaySet7Segment(  displayHandlerIn->txData, ORDER_NUM_7SEG, str[cnt + seg7Shift],  digCnt + 4, layer);
			break;
		}

		digCnt++;
	}
	displayTxFrame(displayHandlerIn, numString, txAddress);
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

	//Set scan limit according color
	for(cnt = 0; cnt < displayHandlerIn->currentSettings.numberOfActivDisplay; cnt++)
	{
		displaySetScanLim(displayHandlerIn, cnt);
	}

	// Set work mode for current user configured displays
	for(cnt = 0; cnt < MAX_PER_SCREEN; cnt++)
	{
		displayConfigWorkMode(displayHandlerIn->txData, cnt, SHUT_DOWN_NORMAL_OPERATION);
	}
	for(cnt = 0; cnt < displayHandlerIn->currentSettings.numberOfActivDisplay; cnt++)
	{
	    displayTxCommand(displayHandlerIn, cnt, TX_ADDRESS_ONE);
	    while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}
	}
	// Set shut down mode for unused displays
	for(cnt = 0; cnt < MAX_PER_SCREEN; cnt++)
	{
		displayConfigWorkMode(displayHandlerIn->txData, cnt, SHUT_DOWN_SHUTDOWN_MODE);
	}
	for(cnt = displayHandlerIn->currentSettings.numberOfActivDisplay; cnt < NUMBER_OF_LCD_STRING; cnt++)
	{
	    displayTxCommand(displayHandlerIn, cnt, TX_ADDRESS_ONE);
	    while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}
	}

	displaySetBrightness(displayHandlerIn, displayHandlerIn->brightnesList[displayHandlerIn->currentSettings.brightnes], 0, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}

	displayConfigDecodeMode(displayHandlerIn->txData, 0, 0b0);
	displayConfigDecodeMode(displayHandlerIn->txData, 1, 0b0);
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
void displaySetBrightness(displayHandlerDef *displayHandlerIn, DISPLAY_BRIGHTNES brightnes, uint16_t numScreen, TX_ADDRESS txAddress){
    uint8_t cnt;
	for(cnt = 0; cnt < MAX_PER_SCREEN; cnt++)
	{
	    displayConfigIntensity(displayHandlerIn->txData, cnt, brightnes);
	}
	displayTxCommand(displayHandlerIn, numScreen, txAddress);
	while(displayIntarfaceGetStatus(displayHandlerIn) == DISPLAY_BUSY){}
}


