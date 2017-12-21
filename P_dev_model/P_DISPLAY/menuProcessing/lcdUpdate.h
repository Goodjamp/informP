/********************************************************************************
 * @file    lcdUpdate.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date     15-November-2017
 * @brief
 */

#ifndef LCD_UPDATE_H_
#define LCD_UPDATE_H_

#include "processing_display.h"
#include "menuProcessing.h"

#define NUMBER_OF_VALUE  6

#define SYMBOL_TEMPERATURE   'C'
#define SYMBOL_PRESSURE      'P'
#define SYMBOL_HUMIDITY      'H'
#define SYMBOL_DATE          'D'
#define SYMBOL_TIME          'T'
#define SYMBOL_FRQ           'F'
#define ERROR_INF            "ErrO"
#define TEST_STR             "@8888"

typedef enum{
	VALUE_STATUS_OK     = 0b0,
	VALUE_STATUS_ALLARM = 0b1,
	VALUE_STATUS_ERROR  = 0b10
}VALUE_STATUS;

bool updateLCD(uint8_t *str, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus, uint8_t numberValue );
void initValueAddress(void);


#endif
