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
	DISPLAY_MENU displayState;        // display state: test, work
}menuDescription = {
		.widjetCnt = 0,
		.displayState = DISPLAY_MENU_WORK,

};

static void increaseWidjetP(void);
static void increaseListBoxItem(void);
static void mainMenuUpdate(menuActionListDef inAction);
static void adjustmentMenuUpdate(menuActionListDef inAction);
static void testMenuUpdate(menuActionListDef inAction);
static void updateBrightnes(void);


void mainMenuUpdate(menuActionListDef inAction){
	switch(inAction){
	    case MENU_ACTION_ENTER:
	    	menuDescription.displayState = DISPLAY_MENU_UDJUSTMENT;
	    	break;
	    case MENU_ACTION_SELL:
	    	updateBrightnes();
	    	break;
	    case MENU_ACTION_TEST:
	    	menuDescription.displayState = DISPLAY_MENU_TEST;
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
	    	menuDescription.displayState = DISPLAY_MENU_WORK;
	    	break;
	    case MENU_ACTION_TEST:
	    	menuDescription.displayState = DISPLAY_MENU_TEST;
	    	break;
	    default: break;
	};
}


void testMenuUpdate(menuActionListDef inAction){

   menuDescription.displayState = DISPLAY_MENU_WORK;

}


void updateBrightnes(void){
	// Call user implementation function
	setBreightnes();
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


/**
  * @brief  Action on change display
  * @param
  * @retval
  */
void menuUpdate(menuActionListDef inAction){
	switch(menuDescription.displayState){
	case DISPLAY_MENU_WORK:
		mainMenuUpdate(inAction);
		break;
	case DISPLAY_MENU_UDJUSTMENT:
		adjustmentMenuUpdate(inAction);
		break;
	case DISPLAY_MENU_TEST:
		testMenuUpdate(inAction);
		break;
	}
}


/**
  * @brief  return current cursor position
  * @param
  * @retval
  */
uint8_t menuGetListboxItemIndex(uint8_t orderNumberLisbox){
	return menuDescription.listBox[orderNumberLisbox].item;
}

/**
  * @brief  return current cursor position
  * @param
  * @retval
  */
uint8_t menuGetListbox(void){
	return menuDescription.widjetCnt;
}


/**
  * @brief  return current menu
  * @param
  * @retval
  */
DISPLAY_MENU menuGetCurrentMenu(void){
	return menuDescription.displayState;
}


/**
  * @brief  set quantity of item in Listbox by the Listbox index
  * @param
  * @retval
  */
void menuSetLisBoxNumItem(uint8_t orderNumber, uint8_t numItem){
	menuDescription.listBox[orderNumber].numItem = numItem;
}
