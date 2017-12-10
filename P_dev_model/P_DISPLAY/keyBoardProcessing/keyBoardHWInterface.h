/********************************************************************************
 * @file    keyBoardHWInterface.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    18-November-2017
 * @brief
 */
#ifndef KEYBOARD_PROCESSING_H_
#define KEYBOARD_PROCESSING_H_

#include "stdint.h"
#include "stm32f10x_gpio.h"

#define NUM_OF_KEY              2

#define BUTTON_MODE_PORT       GPIOB
#define BUTTON_MODE_PIN        GPIO_Pin_2

#define BUTTON_SEL_PORT       GPIOC
#define BUTTON_SEL_PIN        GPIO_Pin_13

#define BOUNS_PERIOD         5


// structure descried one action
typedef struct{
	uint16_t      keyMask;
	GPIO_TypeDef  *keyPort;
	uint16_t      keyPin;
}keyDescDef;

#endif
