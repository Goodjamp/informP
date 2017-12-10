/********************************************************************************
  * @file    processing_display.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */
#include "stdint.h"
#include "stdio.h"
#include "stddef.h"
#include "string.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"

#include "processing_mem_map.h"
#include "processing_display.h"
#include "processing_mem_map_extern.h"

#include "LCD.h"
#include "keyBoardProcessingUserInterface.h"
#include "menuProcessing.h"


extern S_address_oper_data s_address_oper_data;

displayHandlerDef myDisplay;
//-----Queue for send action to menu
QueueHandle_t menuQueue;
/*====================================BUTTON ================================================*/
//-Button processing timer variables
TimerHandle_t     keyTimerHandler;
volatile uint8_t  keyNumAction;
volatile uint8_t  keyNumPeriod;
/*====================================DISPLAY ================================================*/
TimerHandle_t     lcdTimerHandler;

#define NUMBER_OF_VALUE  6

struct {
	uint8_t sumbol[2];
	uint16_t statusAddress;
	uint16_t memAddress;
}paramIndication[NUMBER_OF_VALUE] = {
		[PAR_TEMPERATURE] = { .sumbol = { 'C' }, },
		[PAR_PRESSURE]    = { .sumbol = { 'P' }, },
		[PAR_HUMIDITY]    = { .sumbol = { 'H' }, },
		[PAR_DATE]        = { .sumbol = { 'D' }, },
		[PAR_TIME]        = { .sumbol = { 'T' }, },
		[PAR_FRQ]         = { .sumbol = { 'F' }, }
};


typedef struct{
	uint16_t cntTest;
}menuTest;

typedef struct{
	uint8_t brightnes;
}menuWork;

typedef struct{
    uint8_t cursorPos;
    uint8_t QW;
}menuConfig;

const static uint8_t errorInf[5] = "ErrO";

uint16_t valueAddress[NUMBER_OF_VALUE];

struct{
	uint8_t numPar;
	uint8_t listPar;
}screenAdjustment[NUMBER_LCD_STRING];


/*============================================================================================*/

void lcdTestSate(void){

}

void lcdAdjustmentState(uint8_t sellLCD, uint8_t sellItem){

}

void lcdWorkState(uint8_t mewBrightnes){

	displaySetBrightnes(&myDisplay, mewBrightnes, 0, TX_ADDRESS_ALL);
	// update
}



void updateLcdVal(void) {
	uint8_t lcdStr[7];
	uint8_t cnt;
	uint8_t k;
	static uint16_t redVal;
	// update every LCD string
	// TODO get NUMBER_LCD_STRING FROM CONFIG TABLE!!!
	displaySetDefConfig(&myDisplay);
	for (cnt = 0; cnt < NUMBER_LCD_STRING; cnt++) {
		// value status
		processing_mem_map_read_s_proces_object_modbus(&redVal, 1,
				paramIndication[getListboxItemIndex(cnt)].statusAddress);

		if (redVal) // status of value should be 0, if more - exception
		{

			sprintf((char*)lcdStr, "%1c%s", (char)paramIndication[getListboxItemIndex(cnt)].sumbol[0], (char*)errorInf);

		} else {
			redVal = 234;
			// TODO get index of current value FROM CONFIG TABLE for ever LCD string!!!
			processing_mem_map_read_s_proces_object_modbus(&redVal, 1,
					paramIndication[getListboxItemIndex(cnt)].memAddress);

			// convert values
			switch (getListboxItemIndex(cnt)) {
			case PAR_DATE:
				break;
			case PAR_TIME:
				break;
			case PAR_HUMIDITY:
				sprintf((char*)lcdStr, "%1c%5.1f", (char)paramIndication[PAR_HUMIDITY].sumbol[0], (float)(redVal)/10 );
				break;
			case PAR_PRESSURE:
				sprintf((char*)lcdStr, "%1c%4d", (char)paramIndication[PAR_PRESSURE].sumbol[0], redVal);
				break;
			case PAR_TEMPERATURE:
				sprintf((char*)lcdStr, "%1c%5.1f", (char)paramIndication[PAR_TEMPERATURE].sumbol[0], (float)((int16_t)redVal)/10 );
				break;
			case PAR_FRQ:
				sprintf((char*)lcdStr, "%1c%5.2f", (char)paramIndication[PAR_FRQ].sumbol[0], (float)(redVal)/1000);
				break;
			}
		}
		for(k = 0; k < sizeof(lcdStr); k++)
		{
			if('.' == lcdStr[k])
			{
				lcdStr[k] |= 0b10000000;
			}
		}
		displayWrite(&myDisplay, cnt, lcdStr, strlen((const char*)lcdStr), COLOR_RED, TX_ADDRESS_ONE);
	}
}


void updateClock(uint8_t lcdPos){
	/*
	if( updateClockCnt >= LCD_UPDATE_PERIOD)
	{
		// TODO read data from memory map
		// TODO add code for update lcd
		updateClockCnt = 0;
	}
	*/
}


static void butttonTimerFunctionCB( TimerHandle_t xTimer ){
	if( keyUpdate(&keyNumAction, &keyNumPeriod) == ACTION_DETECT )
	{
		actionQueueMember actionMember = {
			.type = event_KEY,
			.payload[0] = keyNumAction,
			.payload[1] = keyNumPeriod,
		};
		keyPause(50);
		// push button action to queue
		xQueueSendToBack(menuQueue, (void*)&actionMember, 0);
	}
}


static void lcdTimerFunctionCB( TimerHandle_t xTimer ){
	actionQueueMember actionMember = {
		.type = event_DISPLAY
	};
    // push button action to queue
    xQueueSendToBack(menuQueue, (void*)&actionMember, 0);
}


/**
  * @brief
  * @param
  * @retval
  */
uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start){
	return adres_start;
}


static menuActionListDef decodeKeyAction(uint8_t action, int8_t intervalIn){
	menuActionListDef decodeUserAction = MENU_NO_ACTION;
	switch(action)
	{
	    case ACTION_MODE:
	    	decodeUserAction = MENU_ACTION_ENTER;
	    	break;
	    case ACTION_SELL:
	    	decodeUserAction = MENU_ACTION_SELL;
	    	break;
	    case ACTION_TEST:
	    	decodeUserAction = MENU_ACTION_TEST;
	    	break;
	    case 3:
	    	decodeUserAction = MENU_ACTION_ESC;
	    	break;
	}

	return decodeUserAction;
}

/**
  * @brief
  * @param
  * @retval
  */
void t_processing_display(void *pvParameters){
	actionQueueMember actionMember;
	// copy address of all values that can indication
	paramIndication[PAR_TEMPERATURE].memAddress    = s_address_oper_data.s_sensor_address.rezTemperature;
	paramIndication[PAR_TEMPERATURE].statusAddress = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_PRESSURE].memAddress       = s_address_oper_data.s_sensor_address.rezPressure;
	paramIndication[PAR_PRESSURE].statusAddress    = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_HUMIDITY].memAddress       = s_address_oper_data.s_sensor_address.rezHumidity;
	paramIndication[PAR_HUMIDITY].statusAddress    = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_DATE].memAddress           = s_address_oper_data.s_TIME_address.DATE;
	paramIndication[PAR_DATE].statusAddress        = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_TIME].memAddress           = s_address_oper_data.s_TIME_address.TIME;
	paramIndication[PAR_TIME].statusAddress        = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_FRQ].memAddress            = s_address_oper_data.s_FRQmetter_address.rez_FRQmetter;
	paramIndication[PAR_FRQ].statusAddress         = s_address_oper_data.s_FRQmetter_address.status_FRQmetter;

	displayInit( &myDisplay );
	keyboardInit();
	menulisBoxSetNumItem(0, 6);
	menulisBoxSetNumItem(1, 6);
	menulisBoxSetNumItem(2, 6);
	menulisBoxSetNumItem(3, 6);
	//-------------------Init key processing timer and queue-------------------------------
	keyTimerHandler =  xTimerCreate((const char*)"BUTTON TIMER",
			                                 BUTTON_PROCESSING_MS,
			                                 pdTRUE,
	                                         (void *)0,
	                                         butttonTimerFunctionCB);
	lcdTimerHandler =  xTimerCreate((const char*)"LCD TIMER",
                                     LCD_UPDATE_MS,
                                     pdTRUE,
                                     (void *)0,
                                     lcdTimerFunctionCB);

	menuQueue = xQueueCreate( ACTION_QUEUE_QANTITY, sizeof(actionQueueMember) );
   // Start key processing timer
	xTimerStart( keyTimerHandler, 0 );
	// Start lcd processing timer
	xTimerStart( lcdTimerHandler, 0 );
    // initialization string
	uint8_t str[] = "*8888";
	displayWrite(&myDisplay, SCREEN_4, str, strlen((const char*)str), COLOR_RED, TX_ADDRESS_ALL);
	vTaskDelay(4000);

	while(1){
		xQueueReceive(menuQueue,(void*)&actionMember,  portMAX_DELAY );
		if(actionMember.type == event_DISPLAY)
		{
			updateLcdVal();
		}
		else if(actionMember.type == event_KEY)
		{
			menuUpdate(
					decodeKeyAction(  ((keyActionDescription*)actionMember.payload)->action ,
					                  ((keyActionDescription*)actionMember.payload)->periodIndex
					               )
					  );

		}

	}
}
