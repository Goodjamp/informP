/**
 ******************************************************************************
 * @file    bkeyBoardProcessing..h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-September-2016
 * @brief
 */

#ifndef KEYBOARDPROCESSING_H_
#define KEYBOARDPROCESSING_H_

#include "stdint.h"
#include "stdbool.h"

#define NUMBER_OF_BUTTON          2
// Number of different action (selected, set, set test mode and so on)
#define NUMBER_OF_ACTION   3

#define MAX_NUM_PERIOD       3

#define ACTION_MODE_PERIOD   40
#define ACTION_SELL_PERIOD   40
#define ACTION_TEST_PERIOD   400

typedef enum{
	KEY_MODE = 0,
	KEY_SELL  = 1
}buttonDef;

typedef enum{
	BUTTON_NOT_PRESS = 0,
	BUTTON_PRESS = 1
}buttonState;

typedef enum{
	ACTION_MODE,
	ACTION_SELL,
	ACTION_TEST,
	ACTION_NO_DETECT
}actionButtonDef;




uint32_t buttonTimerID;

// structure descried one action
typedef struct{
	uint8_t  comboButtonMask; // flags buttons press
	uint8_t  numPeriod;
	uint16_t periodForAction[MAX_NUM_PERIOD];
}actionDescrDef;


actionButtonDef keyUpdate( void );
actionButtonDef key1Update( void );

//-------------------------user implementation  functions----------------------
void keyHWConfig(void);
uint16_t keyGetPressMask(void);
void keyClear(uint16_t mask);

#endif
