/********************************************************************************
  * @file    menuProcessing.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    25-November-2017
  * @brief
  */

#include "stdint.h"
#include "menuProcessing.h"
#include "keyBoardProcessingUserInterface.h"
#include "LCD.h"

extern displayHandlerDef myDisplay;;

typedef struct{

}udjusymentMenuDef;

struct{
	uint8_t       widjetCnt;           // ordering number selected widget
	listBoxDef    listBox[NUM_OF_LISBOX]; // pointer on the list of ListBox
	uint8_t       brightnes;           // global brightness
	DISPLAY_STATE displayState;        // display state: test, work
}menuDescription = {
		.widjetCnt = 0,
		.displayState = DISPLAY_STATE_WORK,

};


void menulisBoxSetNumItem(uint8_t orderNumber, uint8_t numItem){
	menuDescription.listBox[orderNumber].numItem = numItem;
}

void menuBrightnesSet(uint8_t numStrinLCD, uint8_t brightnes){
	menuDescription.brightnes = brightnes;
}

/**
  * @brief  Action on change display
  * @param
  * @retval
  */
void menuUpdate(menuActionListDef inAction){
	switch(menuDescription.displayState){
	case DISPLAY_STATE_WORK:
		mainMenuUpdate(inAction);
		break;
	case DISPLAY_STATE_UDJUSTMENT:
		adjustmentMenuUpdate(inAction);
		break;
	case DISPLAY_STATE_TEST:
		testMenuUpdate(inAction);
		break;
	}
}


void mainMenuUpdate(menuActionListDef inAction){
	switch(inAction){
	    case MENU_ACTION_ENTER:
	    	menuDescription.displayState = DISPLAY_STATE_UDJUSTMENT;
	    	break;
	    case MENU_ACTION_SELL:
	    	updateBrightnes();
	    	break;
	    case MENU_ACTION_TEST:
	    	menuDescription.displayState = DISPLAY_STATE_TEST;
	    	break;
	    default: break;
	};
}


void adjustmentMenuUpdate(menuActionListDef inAction){
	switch(inAction){
	    case MENU_ACTION_ENTER:
	    	increaseWidjetP();
	    	break;
	    case MENU_ACTION_SELL:
	    	increaseListBoxItem();
	    	break;
	    case MENU_ACTION_ESC:
	    	menuDescription.displayState = DISPLAY_STATE_WORK;
	    	break;
	    case MENU_ACTION_TEST:
	    	menuDescription.displayState = DISPLAY_STATE_TEST;
	    	break;
	    default: break;
	};
}


void testMenuUpdate(menuActionListDef inAction){
    if( inAction == MENU_ACTION_ESC)
    {
    	menuDescription.displayState = DISPLAY_STATE_WORK;
    }
}


void updateBrightnes(void){
	menuDescription.brightnes += BRIGHTNESS_STEP;
	if( menuDescription.brightnes > BRIGHTNESS_MAX )
	{
		menuDescription.brightnes = BRIGHTNESS_MIN;
	}
    // TODO temporary decision call LCD function from menu function!!!!  Move this code to some LCD processing part!!
	displaySetBrightnes(&myDisplay, menuDescription.brightnes, 0, TX_ADDRESS_ALL);
	//-----------------------------------
}


void increaseWidjetP(void){
	menuDescription.widjetCnt++;
	if( menuDescription.widjetCnt >= sizeof(menuDescription.listBox)/sizeof(menuDescription.listBox[0]) )
	{
		menuDescription.widjetCnt = 0;
		return;
	}
}


void increaseListBoxItem(void){
	menuDescription.listBox[menuDescription.widjetCnt].item ++;
	if( menuDescription.listBox[menuDescription.widjetCnt].item >= menuDescription.listBox[menuDescription.widjetCnt].numItem )
	{
		menuDescription.listBox[menuDescription.widjetCnt].item = 0;
	}
}


uint8_t getListboxItemIndex(uint8_t orderNumberLisbox){
	return menuDescription.listBox[orderNumberLisbox].item;
}
