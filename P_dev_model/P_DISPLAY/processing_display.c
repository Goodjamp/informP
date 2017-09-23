/**
  ******************************************************************************
  * @file    processing_display.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */
#include "stdint.h"
#include "stddef.h"
#include "string.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include "task.h"

#include "processing_mem_map_extern.h"
#include "processing_display.h"
#include "max7219.h"
#include "stmMaxHardwareInit.h"
#include "buttonProcessing.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

extern S_address_oper_data s_address_oper_data;
xTimerHandle screenUpdateTimer;


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

struct{
	uint8_t widjetCnt;
	uint8_t widjetNum;
	listBox **listOfListBox;
	uint8_t brightnes;         // global brightens
	DISPLAY_STATE displayState;     // display state: test, work
	uint8_t listBoxSellFlag;
}menuDescription;


const char *paramitersSymbols[] = {
		[PARAM_SYMB_POS_DATE]        = "Dt",
		[PARAM_SYMB_POS_TIME]        = "Tm",
		[PARAM_SYMB_POS_FREQUENCY]   = "Fr",
		[PARAM_SYMB_POS_HUMIDITY]    = "Hd",
		[PARAM_SYMB_POS_ATM_PRESURE] = "Pr",
		[PARAM_SYMB_POS_TEMPERATURE] = "Tc",
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
	for(cnt = 0 ; cnt < NUMBER_7_SEGMENTS_IND; cnt++)
	{
		switch (color){
		case COLOR_GREEN:
			displaySet7Segment(  displayHandler, ORDER_NUM_7SEG, str[cnt + 1],  cnt);
			break;
		case COLOR_RED:
			displaySet7Segment(  displayHandler, ORDER_NUM_7SEG, str[cnt + 1],  cnt + 4);
			break;
		case COLOR_ORANGE:
			displaySet7Segment(  displayHandler, ORDER_NUM_7SEG, str[cnt + 1],  cnt);
			displaySet7Segment(  displayHandler, ORDER_NUM_7SEG, str[cnt + 1],  cnt + 4);
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


void screenUpdateTimerCallback( TimerHandle_t xTimer ){

}



uint8_t str[] = "h111a";
/**
  * @brief
  * @param
  * @retval
  */
void t_processing_display(void *pvParameters){

	buttonGpioConfig();
	screenUpdateTimer = xTimerCreate("SCREEN UPDATE TIMER", BUTTON_TIMER_PERIOD_UPDATE, pdFALSE, (void*)(0) , screenUpdateTimerCallback);



	displayInterfaceInit(&displayHandler, displayBuff);
	initDisplay(&displayHandler, LDList, numberOfScreen, DISPLAY_SPI);
	//Clear all display
	displayClearBuff(&displayHandler, MAX_PER_SCREEN);
	displayTxData(&displayHandler, 0, MAX_PER_SCREEN, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(&displayHandler) == DISPLAY_BUSY){}

	refreshDisplays();

	uint32_t cnt = 0;
	COLOR currentCollor = COLOR_RED;
	while(1){
		updateScreen(&displayHandler, SCREEN_4, str, strlen((const char*)str), currentCollor, TX_ADDRESS_ONE);

		while(displayIntarfaceGetStatus(&displayHandler) == DISPLAY_BUSY){}
		while(cnt < 200000){
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


/**
  * @brief  Action on change display
  * @param
  * @retval
  */
void userActionReaction(USER_ACTION inAction){
// menuDescription
	if(menuDescription.listBoxSellFlag){


	}
	if(inAction == ACTION_ENTER)
	{
		// shift to next widget
		increaseWidgetP();
	}
	else if(inAction == ACTION_ESC)
	{
		decreaseWidgetP();
	}

}

void listBoxProcess(USER_ACTION inAction){
	switch(inAction)
	{
	case ACTION_ENTER:
		increaseWidjetP();
		break;
	case ACTION_ESC:
		decreaseWidjetP();
		break;
	case ACTION_SELL:
		listBoxChangePar();
		break;
	case ACTION_TEST:
		break;
	};
}

void listBoxChangePar(void){
	listBox* currentListBox = menuDescription.listOfListBox[menuDescription.widjetCnt];
	if(currentListBox->selPos >= currentListBox->numPar)
	{
		currentListBox->selPos = 0;
		return;
	}
	currentListBox->selPos++;
}

void increaseWidjetP(void){
	if(menuDescription.widjetCnt >= (menuDescription.widjetNum - 1))
	{
		menuDescription.widjetCnt = 0;
		return;
	}
	menuDescription.widjetCnt++;
}

void decreaseWidjetP(void){
	if(menuDescription.widjetCnt == 0)
	{
		menuDescription.widjetCnt = menuDescription.widjetNum - 1;
		return;
	}
	menuDescription.widjetCnt--;
}

/*
 * Display is -  menu with 2 levels: 1 (ROOT) - selected screen, (have 1 adjustment parameter - brightens)
 *                                   2 -  adjustment screen parameters (sel. showing parameter).
 *
 * shift between screen - is like a menu tree. Press "MODE" button - open current menu position.
 *                                             Press "CORREC" button - set new menu parameters
 *
 * */




