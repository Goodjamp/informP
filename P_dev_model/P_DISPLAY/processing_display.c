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

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include "task.h"

#include "processing_display.h"

#include "processing_mem_map_extern.h"

#include "LCD.h"

#include "keyBoardProcessingUserInterface.h"

extern S_address_oper_data s_address_oper_data;

displayHandlerDef myDisplay;

struct{
	uint8_t widjetCnt;
	uint8_t widjetNum;
	listBox **listOfListBox;
	uint8_t brightnes;         // global brightness
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
  * @brief  Action on change display
  * @param
  * @retval
  */
/*
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
*/

/**
  * @brief
  * @param
  * @retval
  */
uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start){
	return adres_start;
}

/**
  * @brief
  * @param
  * @retval
  */
void t_processing_display(void *pvParameters){

	/*init display:
	  - config perephirials;
	  - start config ALL display (mean ALL chips in ALL screen)
	 */
	displayInit( &myDisplay );
	keyHWConfig();

	COLOR currentCollor = COLOR_RED;
	while(1){
		vTaskDelay(5);
		keyUpdate();
		//displayWrite(&myDisplay, SCREEN_3, str, strlen((const char*)str), currentCollor, TX_ADDRESS_ONE);
	}
}

/*
 * Display is -  menu with 2 levels: 1 (ROOT) - selected screen, (have 1 adjustment parameter - brightens)
 *                                   2 -  adjustment screen parameters (sel. showing parameter).
 *
 * shift between screen - is like a menu tree. Press "MODE" button - open current menu position.
 *                                             Press "CORREC" button - set new menu parameters
 *
 * */




