/**
  ******************************************************************************
  * @file    processing_display.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */

#ifndef PROCESSING_DISPLAY_H_
#define PROCESSING_DISPLAY_H_
#include "processing_display_extern.h"

#define MAX_NUM_PAR            6

typedef enum{
	PARAM_SYMB_POS_DATE        = 0,
	PARAM_SYMB_POS_TIME        = 1,
	PARAM_SYMB_POS_FREQUENCY   = 2,
	PARAM_SYMB_POS_HUMIDITY    = 3,
	PARAM_SYMB_POS_ATM_PRESURE = 4,
	PARAM_SYMB_POS_TEMPERATURE = 5
}PARAM_SYMB_POS;

// Structure described item inside widget ListBox
typedef struct{
	uint8_t parSymbol;
	uint16_t parAddress;
	uint32_t data;
}itemListBox;


typedef struct{
	itemListBox listOfPar[MAX_NUM_PAR];
}ListBox;

// structure described menu
typedef struct{
	uint8_t selPos;      // order number of selected menu item (list), should be les then 0x11111110 = 0x254
	uint8_t numPar;      // quantity of menu item
}listBox;

typedef enum{
	DISPLAY_STATE_WORK,
	DISPLAY_STATE_TEST
}DISPLAY_STATE;


typedef enum{
	ACTION_USER,
	ACTION_TIMER,
}DIASPALY_ACTION;

#define isDIASPALY_ACTION(X) (( X == ACTION_USER)|| \\
							  ( X == ACTION_TIMER) )

typedef enum{
	ACTION_ENTER,
	ACTION_ESC,
	ACTION_SELL,
	ACTION_TEST
}USER_ACTION; // relation to Press Button




//--------------------------------------------------------------------------
//---------MENU PROCESSING FUNCTION PROTOTYPE-------------------------------
//--------------------------------------------------------------------------
uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start);
void userActionReaction(USER_ACTION inAction);
void listBoxProcess(USER_ACTION inAction);
void listBoxChangePar(void);
void increaseWidjetP(void);
void decreaseWidjetP(void);

#endif // PROCESSING_DISPLAY_H_
