/********************************************************************************
 * @file    lcdUpdate.h
 * @author  Gerasimchuk A.
 * @version V1.0.1
 * @date    15-November-2017
 * @brief
 */

#ifndef LCD_UPDATE_H_
#define LCD_UPDATE_H_

#include "processing_display.h"

void updateLCD(uint8_t *str, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus, uint8_t numberValue );

#endif
