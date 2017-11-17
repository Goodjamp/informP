/**
 ******************************************************************************
 * @file    keyBoardProcessingUserInterface.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    14-November-2016
 * @brief
 */

#ifndef KEYBOARD_PROCESSING_USER_INTERFACE_H_
#define KEYBOARD_PROCESSING_USER_INTERFACE_H_

#include "stdint.h"

#define MAX_NUM_TIME  2

typedef struct{
	uint8_t buttonMask;
	uint8_t timeHold[MAX_NUM_TIME];
	uint8_t numTimeHold;
}buttonActionDescDef;

typedef void (*buttonActionCallbackDef)(uint8_t buttonActionIndex, uint8_t timeHoldIndex);


void keyboardInit(buttonActionDescDef *buttonActionDesc, uint8_t numButtonActionDesc );
void keyboardSetCallback(buttonActionCallbackDef buttonActionCallback);

void keyboardGetButtonMask(uint16_t *buttonMask);


#endif
