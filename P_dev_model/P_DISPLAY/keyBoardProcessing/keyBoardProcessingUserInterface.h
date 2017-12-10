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


typedef enum{
	ACTION_DETECT,
	ACTION_NO_DETECT
}actionDetect;


#define NUMBER_OF_PERIOD  3
#define NUMBER_OF_BUTTON  2

typedef enum{
	KEY_MODE = 0,
	KEY_SELL  = 1
}buttonDef;

typedef enum{
	ACTION_MODE,
	ACTION_SELL,
	ACTION_TEST
}actionButtonDef;


typedef struct{
	uint8_t buttonMask;
	uint8_t timeHold[NUMBER_OF_PERIOD];
	uint8_t numTimeHold;
}buttonActionDescDef;

typedef void (*buttonActionCallbackDef)(uint8_t buttonActionIndex, uint8_t timeHoldIndex);

void keyboardInit( void );
actionDetect keyUpdate(volatile uint8_t *actionNum, volatile uint8_t *timePeriodNumber );
void keyPause(uint16_t pause);

//-------------- User define init/processing HW button interface-----------------
void keyHWConfig(void);
uint16_t keyGetPressMask(void);



#endif
