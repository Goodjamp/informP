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

#define NUMBER_OF_BUTTON    2

typedef enum{
	MODE_BUTTON = 0,
	SEL_BUTTON = 1
}buttonDef;

#define NUM_OF_BUTTO                  2
#define BUTTON_MODE_PORT       GPIOA
#define BUTTON_MODE_PIN        GPIO_Pin_5

#define BUTTON_SEL_PORT       GPIOA
#define BUTTON_SEL_PIN        GPIO_Pin_5


#define BUTTON_TIMER_PERIOD_UPDATE    50
unt32_t buttonTimerID;

typedef struct{
	uint8_t pressF;
	uint8_t pressStateCnt;
	GPIO_TypeDef portButton;
	uint16_t     pinButtpn;
}buttonProcesS;



#endif
