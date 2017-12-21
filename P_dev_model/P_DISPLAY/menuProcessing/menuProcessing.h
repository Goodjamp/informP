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
	DISPLAY_MENU_WORK = 0,
	DISPLAY_MENU_UDJUSTMENT,
	DISPLAY_MENU_TEST
}DISPLAY_MENU;


typedef enum{
	ACTION_USER,
	ACTION_TIMER,
}DIASPALY_ACTION;


//--------------------------------------------------------------------------
//---------MENU PROCESSING FUNCTION PROTOTYPE-------------------------------
//--------------------------------------------------------------------------
void menuUpdate                 (menuActionListDef inAction);
void menuSetLisBoxNumItem       (uint8_t orderNumber, uint8_t numItem);
uint8_t menuGetListboxItemIndex (uint8_t orderNumberLisbox);
DISPLAY_MENU menuGetCurrentMenu (void);
uint8_t menuGetListbox(void);
// User implementation function
void setBreightnes(void);



#endif
