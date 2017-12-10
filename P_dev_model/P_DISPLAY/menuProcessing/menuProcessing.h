/********************************************************************************
  * @file    menuProcessing.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    25-November-2017
  * @brief
  */
#ifndef MENU_PROCESSING_H
#define MENU_PROCESSING_H
#include "stdint.h"

#define NUM_OF_LISBOX      4

#define BRIGHTNESS_MAX 31
#define BRIGHTNESS_MIN  5
#define BRIGHTNESS_STEP 2



typedef enum{
	MENU_ACTION_ENTER,
	MENU_ACTION_SELL,
	MENU_ACTION_TEST,
	MENU_ACTION_ESC,
	MENU_NO_ACTION
}menuActionListDef;



// Structure described item inside widget ListBox
typedef struct{
	uint8_t numItem;
	uint8_t item;
}listBoxDef;

typedef enum{
	DISPLAY_STATE_WORK = 0,
	DISPLAY_STATE_UDJUSTMENT,
	DISPLAY_STATE_TEST
}DISPLAY_STATE;


typedef enum{
	ACTION_USER,
	ACTION_TIMER,
}DIASPALY_ACTION;


//--------------------------------------------------------------------------
//---------MENU PROCESSING FUNCTION PROTOTYPE-------------------------------
//--------------------------------------------------------------------------
void menulisBoxSetNumItem(uint8_t orderNumber, uint8_t numItem);
void menuBrightnesSet(uint8_t numStrinLCD, uint8_t brightnes);
uint8_t getListboxItemIndex(uint8_t orderNumberLisbox);

void menuUpdate(menuActionListDef inAction);
void mainMenuUpdate(menuActionListDef inAction);
void adjustmentMenuUpdate(menuActionListDef inAction);
void testMenuUpdate(menuActionListDef inAction);
void updateBrightnes(void);
void increaseWidjetP(void);
void increaseListBoxItem(void);



#endif
