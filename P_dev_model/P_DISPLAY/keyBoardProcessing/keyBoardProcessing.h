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

#include "keyBoardProcessingUserInterface.h"


#define ACTION_MODE_PERIOD   5
#define ACTION_SELL_PERIOD   5
#define ACTION_TEST_PERIOD   400



// structure descried one action
typedef struct{
	uint8_t  comboButtonMask; // flags buttons press
	uint8_t  numPeriod;
	uint16_t periodForAction[NUMBER_OF_PERIOD];
}actionDescrDef;

//-------------------------user implementation  functions----------------------
void keyClear(uint16_t mask);

#endif
