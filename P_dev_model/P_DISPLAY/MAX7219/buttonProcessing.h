/**
 ******************************************************************************
 * @file    buttonProcessing.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-September-2016
 * @brief
 */

#ifndef BUTTONPROCESSING_H_
#define BUTTONPROCESSING_H_

#include "stdint.h"
#include "stdbool.h"

#define NUMBER_OF_BUTTON          2
// Number of different action (selected, set, set test mode and so on)
#define NUMBER_OF_ACTION   3


//--------ACTION MODE--------------------
#define ACTION_MODE_PERIOD_BEFORE    2
#define ACTION_MODE_PERIOD_AFTER     10
//--------ACTION SELL--------------------
#define ACTION_SELL_PERIOD_BEFORE    2
#define ACTION_SELL_PERIOD_AFTER     10
//--------ACTION TEST--------------------
#define ACTION_TEST_PERIOD_BEFORE    3
#define ACTION_TEST_PERIOD_AFTER   200

typedef enum{
	BUTTON_MODE = 0,
	BUTTON_SEL  = 1
}buttonDef;

typedef enum{
	BUTTON_NOT_PRESS = 0,
	BUTTON_PRESS = 1
}buttonState;

typedef enum{
	ACTION_MODE,
	ACTION_SEL,
	ACTION_TEST_,
	ACTION_NO_DETECT
}actionButtonDef;

typedef enum{
	BEFORE_PERIOD_AFTER,
	AFTER_PERIOD_AFTER
}posActionDef;

#define NUM_OF_BUTTO                  2
#define BUTTON_MODE_PORT       GPIOA
#define BUTTON_MODE_PIN        GPIO_Pin_5

#define BUTTON_SEL_PORT       GPIOA
#define BUTTON_SEL_PIN        GPIO_Pin_5

uint32_t buttonTimerID;

// structure descried one action
typedef struct{
	uint8_t periodBefore;    // duration before detect Action
	uint8_t periodAfter;     // duration after detect Action
	uint8_t comboButtonMask; // flags buttons press
	posActionDef timePosAction;   //position of start action: before periodAfter or after of periodAfter
}actionDescrDef;

// Button low lewel description (port, pin, state)
typedef struct{
	buttonState pressF;
	uint8_t pressStateCnt;
	uint8_t buttonMask;
	GPIO_TypeDef *portButton;
	uint16_t     pinButton;
}buttonProcesH;

//
typedef struct{
	uint8_t actionCntBefore;
	uint8_t actionCntAfter;
}actionProcessingDef;

void buttonGpioConfig(void);
actionButtonDef updateButtonCallBack( void );

#endif
