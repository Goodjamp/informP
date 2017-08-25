/**
  ******************************************************************************
  * @file    processing_display.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */
#include "stdint.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "processing_mem_map_extern.h"
#include "processing_display.h"
#include "max7219.h"
#include "stmMaxHardwareInit.h"


extern S_address_oper_data s_address_oper_data;

static displayBuffDef    displayBuff[MAX_PER_SCREEN];
static displayHandlerDef displayHandler;
static LDDescr LDList[numberOfScreen] = {
		[SCREEN_1] = {
				.port = PORT_LD_1,
				.pin =  PIN_LD_1
				},
		[SCREEN_2] = {
				.port = PORT_LD_2,
				.pin =  PIN_LD_2
				},
		[SCREEN_3] = {
				.port = PORT_LD_3,
				.pin =  PIN_LD_3
				},
		[SCREEN_4] = {
				.port = PORT_LD_4,
				.pin =  PIN_LD_4
				},
};


/**
  * @brief
  * @param
  * @retval
  */
uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start){
	return adres_start;
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
void updateScreen(displayHandlerDef *displayHandler, uint16_t numScreen, uint8_t *str, uint16_t strSize, COLOR color, TX_ADDRESS txAddress)
{
	uint16_t cnt = 0;
	// For current screen configuration:
	// first max7219  - 8x8 matrix indicator
	// Second max7219 - 7-segment indicator

	displayClearBuff(displayHandler, MAX_PER_SCREEN);
	displayTxData(displayHandler, numScreen, MAX_PER_SCREEN, txAddress);
	while(displayIntarfaceGetStatus(displayHandler) == DISPLAY_BUSY){}
	// set max#1 sumbol - 8x8 matrix
	displaySet8x8Matrix( displayHandler, ORDER_NUM_MATRIX,  str[ORDER_NUM_MATRIX] );
	// set max#2 - 7 segment 4 ripple colors digits
	for(cnt = 1 ; cnt <= NUMBER_7_SEGMENTS_IND; cnt++)
	{
		switch (color){
		case COLOR_GREEN:
			displaySet7Segment(  displayHandler, ORDER_NUM_7SEG, str[cnt],  cnt);
			break;
		case COLOR_RED:
			displaySet7Segment(  displayHandler, ORDER_NUM_7SEG, str[cnt],  cnt + 4);
			break;
		case COLOR_ORANGE:
			displaySet7Segment(  displayHandler, ORDER_NUM_7SEG, str[cnt],  cnt);
			displaySet7Segment(  displayHandler, ORDER_NUM_7SEG, str[cnt],  cnt + 4);
			break;
		}
	}
	displayTxData(displayHandler, numScreen, MAX_PER_SCREEN, txAddress);
}


/**
  * @brief
  * @param
  * @retval
  */
void refreshDisplays(void)
{
	displayConfigWorkMode(&displayHandler, 0, SHUT_DOWN_NORMAL_OPERATION);
	displayConfigWorkMode(&displayHandler, 1, SHUT_DOWN_NORMAL_OPERATION);
	displayTxData(&displayHandler, 0, 2, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(&displayHandler) == DISPLAY_BUSY){}


	displayConfigScanLimit(&displayHandler, 0, SCAN_LIM_7);
	displayConfigScanLimit(&displayHandler, 1, SCAN_LIM_7);
	displayTxData(&displayHandler, 0, 2, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(&displayHandler) == DISPLAY_BUSY){}

	displayConfigIntensity(&displayHandler, 0, INTENSITY_23_32);
	displayConfigIntensity(&displayHandler, 1, INTENSITY_23_32);
	displayTxData(&displayHandler, 0, 2, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(&displayHandler) == DISPLAY_BUSY){}
}


uint8_t str[] = "12345";
/**
  * @brief
  * @param
  * @retval
  */
void t_processing_display(void *pvParameters){

	displayInterfaceInit(&displayHandler, displayBuff);
	initDisplay(&displayHandler, LDList, numberOfScreen, DISPLAY_SPI);
	//Clear all display
	displayClearBuff(&displayHandler, MAX_PER_SCREEN);
	displayTxData(&displayHandler, 0, MAX_PER_SCREEN, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(&displayHandler) == DISPLAY_BUSY){}

	refreshDisplays();

	uint32_t cnt = 0;
	COLOR currentCollor = COLOR_GREEN;
	while(1){
		updateScreen(&displayHandler, SCREEN_4, str, strlen(str), currentCollor, TX_ADDRESS_ONE);
		while(displayIntarfaceGetStatus(&displayHandler) == DISPLAY_BUSY){}
		while(cnt < 4000000){
			cnt++;
		}
		cnt = 0;

		str[0]++;
				if( str[0] > '9') str[0] = '0';

		str[1]++;
		if( str[1] > '9')
		{
		switch(currentCollor){
		case COLOR_GREEN:
			currentCollor = COLOR_RED;
			break;
		case COLOR_RED:
			currentCollor = COLOR_ORANGE;
			break;
		case COLOR_ORANGE:
			currentCollor = COLOR_GREEN;
			break;
		}
			str[1] = '0';
		}

		str[2]++;
		if( str[2] > '9') str[2] = '0';

		str[3]++;
		if( str[3] > '9') str[3] = '0';

		str[4]++;
		if( str[4] > '9') str[4] = '0';

	}
}
