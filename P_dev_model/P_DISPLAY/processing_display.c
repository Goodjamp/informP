  /********************************************************************************
  * @file    processing_display.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */
#include "stdint.h"
#include "stdbool.h"
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
#include "processing_display_extern.h"
#include "processing_mem_map_extern.h"

#include "LCD.h"
#include "keyBoardProcessingUserInterface.h"
#include "menuProcessing.h"
#include "lcdUpdate.h"
#include "flash_operation.h"


extern S_address_oper_data s_address_oper_data;
// Config module pointer
static S_display_user_config *displayUserConfig;

displayHandlerDef myDisplay;
uint8_t tempSaveBrightnes = 0;
//-----Queue for send action to menu
QueueHandle_t menuQueue;
uint8_t lcdPeriodType;
uint8_t *blinkStateP;
uint8_t lcdStr[12];
const static uint8_t brightnes[] = {DISPLAY_BRIGHTNES_7,
		                            DISPLAY_BRIGHTNES_11,
		                            DISPLAY_BRIGHTNES_19,
		                            DISPLAY_BRIGHTNES_31};
/*====================================BUTTON ================================================*/
//-Button processing timer variables
TimerHandle_t     keyTimerHandler;
TimerHandle_t     escTimerHandler;
volatile uint8_t  keyNumAction;
volatile uint8_t  keyNumPeriod;
/*====================================DISPLAY ================================================*/
TimerHandle_t     lcdTimerHandler;
uint8_t           currentBrightnes;

typedef struct{
	uint16_t cntTest;
}menuTest;

typedef struct{
	uint8_t brightnes;
}menuWork;

struct{
	uint8_t numPar;
	uint8_t listPar;
}screenAdjustment[NUMBER_OF_LCD_STRING];

/*============================================================================================*/

static uint8_t getNumberOfParamiter(uint16_t configBitField, uint8_t selectPos)
{
	uint16_t mask = 0b1;
	uint8_t  cntBit = 0;
	uint8_t  cntPar = 0;
	selectPos++;
	for(;cntPar < PAR_QUANTITY; cntPar++)
	{
		if(configBitField & mask)
		{
			cntBit++;
		}
		if(cntBit == selectPos)
		{
			return cntPar;
		}
		mask <<= 1;
	}
	return 0;
}


void updateLcdVal(BLINK_STATE blinkState) {
	uint8_t cnt;

	if( BLINK_STATE_HIGHT == blinkState )
	{
	    displaySetDefConfig(&myDisplay);
	}
	for (cnt = 0; cnt < displayUserConfig->numScreen; cnt++) {

		memset(lcdStr, 0, sizeof(lcdStr));

		updateLCD( lcdStr,
				   blinkState,
				   menuGetCurrentMenu(),
				   (cnt == menuGetListbox() ) ? (true):(false),
				   getNumberOfParamiter(displayUserConfig->screenConfig[cnt].bitsOfParamiters, menuGetListboxItemIndex(cnt)));
		displayWrite(&myDisplay, cnt, lcdStr, strlen((const char*)lcdStr), COLOR_GREEN, TX_ADDRESS_ONE);
	}
}


static void butttonTimerFunctionCB( TimerHandle_t xTimer ){
	if( keyUpdate(&keyNumAction, &keyNumPeriod) == ACTION_DETECT )
	{
		actionQueueMember actionMember = {
			.type = event_KEY,
			.queueType.KeyAction.action = keyNumAction,
			.queueType.KeyAction.periodIndex = keyNumPeriod,
		};
		if((keyNumAction == ACTION_MODE) && (keyNumPeriod == 1))
		{
			keyPause(300);
		}
		else
		{
			keyPause(50);
		}

		// push button action to queue
		xQueueSendToBack(menuQueue, (void*)&actionMember, 0);
	}
}


static void lcdTimerFunctionCB( TimerHandle_t xTimer ){
	actionQueueMember actionMember = {
		.type = event_DISPLAY
	};


    // push LCD UPDATE action to queue
    xQueueSendToBack(menuQueue, (void*)&actionMember, 0);
}

static void escTimerFunctionCB( TimerHandle_t xTimer ){
	actionQueueMember actionMember = {
		.type = event_KEY,
		.queueType.KeyAction.action = MENU_ACTION_ESC,
		.queueType.KeyAction.periodIndex = 0,
	};
	// push ESC action to queue
	xQueueSendToBack(menuQueue, (void*)&actionMember, 0);
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


void setBrightness(menuActionListDef menuAction){
	actionQueueMember actionMember = {
		.type = event_UPDATE_BRIGHTNESS,
		.queueType.payload[0] = (uint8_t)menuAction
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


static void initUserMenu(S_display_user_config *configData){
	// menu listboxs adjustment
	menuSetNumWidgets(configData->numScreen);
	menuSetLisBoxNumItem(0, configData->screenConfig[0].numParamiterPerScreen);
	menuSetLisBoxNumItem(1, configData->screenConfig[1].numParamiterPerScreen);
	menuSetLisBoxNumItem(2, configData->screenConfig[2].numParamiterPerScreen);
	menuSetLisBoxNumItem(3, configData->screenConfig[3].numParamiterPerScreen);

	displayInit( &myDisplay, brightnes, configData->numScreen);
	keyboardInit();

	//-------------------Init key processing timer and queue-------------------------------
	keyTimerHandler =  xTimerCreate((const char*)"BUTTON TIMER",
			BUTTON_PROCESSING_MS,
			pdTRUE,
			(void *)0,
			butttonTimerFunctionCB);
	lcdTimerHandler =  xTimerCreate((const char*)"LCD UPDATE TIMER",
			LCD_UPDATE_HIGHT_MS,
			pdTRUE,
			(void *)&lcdPeriodType,
			lcdTimerFunctionCB);
	escTimerHandler = xTimerCreate((const char*)"LCD ESC TIMER",
			BUTTON_ESC_PERIOD_MS,
			pdFALSE,
			(void *)0,
			escTimerFunctionCB);
	menuQueue = xQueueCreate( ACTION_QUEUE_QANTITY, sizeof(actionQueueMember) );
	// Start key processing timer
	xTimerStart( keyTimerHandler, 0 );
	// Start lcd processing timer
	xTimerStart( lcdTimerHandler, 0 );

}


static void getLastMenuData(void){
	uint8_t cnt;
	uint8_t cursorPos[NUMBER_OF_LCD_STRING];
	memcpy(cursorPos, (void*)(PAGE_ABS_ADDRESS(FLASH_PAGE_MENU_DATA)),  sizeof(cursorPos));
	for(cnt=0; cnt < NUMBER_OF_LCD_STRING;cnt++)
	{
		if( displayUserConfig->screenConfig[cnt].numParamiterPerScreen <= cursorPos[cnt])
		{
			continue;
		}
		menuSetListboxItemIndex(cnt, cursorPos[cnt]);
	}
}

void saveMenuConfigData(void){
	uint8_t cnt;
	uint8_t cursorPos[NUMBER_OF_LCD_STRING];
	for(cnt=0; cnt < NUMBER_OF_LCD_STRING;cnt++)
	{
		cursorPos[cnt] = menuGetListboxItemIndex(cnt);
	}
	FLASH_OPERATION_erase_page(FLASH_PAGE_MENU_DATA);
	FLASH_OPERATION_write_flash_page_16b( (uint16_t*)cursorPos, sizeof(cursorPos)/sizeof(uint16_t), (uint16_t)FLASH_PAGE_MENU_DATA);
}

/**
  * @brief
  * @param
  * @retval
  */
void t_processing_display(void *pvParameters){
	actionQueueMember actionMember;
	displayUserConfig = (S_display_user_config*)pvParameters;

	initUserMenu((S_display_user_config*)pvParameters );
	// set value indication according last adjustments
	getLastMenuData();

	vTaskDelay(30);
	//welcomeScreen();
	while(1){
		xQueueReceive(menuQueue,(void*)&actionMember,  portMAX_DELAY );

		switch( actionMember.type ){
		// lcd update
		case event_DISPLAY:
			blinkStateP = (uint8_t*)pvTimerGetTimerID(lcdTimerHandler);
			if( BLINK_STATE_HIGHT == *blinkStateP){
				xTimerChangePeriod(lcdTimerHandler, LCD_UPDATE_LOW_MS,0);
				*blinkStateP = BLINK_STATE_LOW;
			}
			else{
				xTimerChangePeriod(lcdTimerHandler, LCD_UPDATE_HIGHT_MS,0);
				*blinkStateP = BLINK_STATE_HIGHT;
			}
			//vTimerSetTimerID(lcdTimerHandler, (void*)blinkState);
			xTimerReset(lcdTimerHandler, 10);
			updateLcdVal(*blinkStateP);// 0b1 & blinkState_++ );
			break;
		// Key update
		case event_KEY:
			menuUpdate( decodeKeyAction(  actionMember.queueType.KeyAction.action,
							          actionMember.queueType.KeyAction.periodIndex
					               )
					  );
			// Start ESC timer
			if( MENU_ACTION_ESC !=  actionMember.queueType.KeyAction.action )
			{
			    xTimerStart(escTimerHandler,10);
			}
			break;
		case event_UPDATE_BRIGHTNESS:
			/* update brightness according menu that we wont to switch:
			  - switch to TEST_BRIGHTNES_INDEX if we swaith to test mode
			  - switch to to normal brightness level in other case
			*/
			switch( actionMember.queueType.Brightnes.action )
			{
			case MENU_ACTION_SWITCH_TO_TEST:
				//tempSaveBrightnes = myDisplay.currentSettings.brightnes;
				//myDisplay.currentSettings.brightnes = TEST_BRIGHTNES_INDEX;
				displaySetBrightness(&myDisplay, myDisplay.brightnesList[TEST_BRIGHTNES_INDEX], 0, TX_ADDRESS_ALL);
				break;
			case MENU_ACTION_SWITCH_TO_WORK:
			case MENU_ACTION_SWITCH_TO_ADJ:
				//myDisplay.currentSettings.brightnes = tempSaveBrightnes;
				displaySetBrightness(&myDisplay, myDisplay.brightnesList[myDisplay.currentSettings.brightnes], 0, TX_ADDRESS_ALL);
				break;
			case MENU_ACTION_SELL:
				if( ++myDisplay.currentSettings.brightnes > (sizeof(brightnes)/sizeof(brightnes[0])-1 ) )
				{
					myDisplay.currentSettings.brightnes = 0;
				}
				displaySetBrightness(&myDisplay, myDisplay.brightnesList[myDisplay.currentSettings.brightnes], 0, TX_ADDRESS_ALL);
				break;
			default:
				break;
			};
		}


	}
}
