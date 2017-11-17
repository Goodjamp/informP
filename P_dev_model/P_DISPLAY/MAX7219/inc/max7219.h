/**
  ******************************************************************************
  * @file    max7219.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    22-DEcembefr-2016
  * @brief   This file contains interface for access to max7219 chip
  */

#ifndef MAX7219_H_
#define MAX7219_H_

#include "stdint.h"



#define NUM_MAX_DIGITS     8

/* @brief Description video bufer for one MAX7219
 * */
typedef uint16_t displayBuffDef[NUM_MAX_DIGITS];

/**
  * @brief  Set configuration data
  * @param
  * @retval None
  */
#define maxSetConfig_(X,Y)    ( ( (uint8_t)X<<8 ) | ( (uint8_t)Y ) )


/*********************************************************************/
/******************   MAX7219 COMAND LIST    *************************/
/*********************************************************************/
typedef enum{
	NO_OP        = 0b00000000,
	DECODE_MODE  = 0b00001001,
	INTENSITY    = 0b00001010,
	SCAN_LIMIT   = 0b00001011,
	SHUTDOWN     = 0b00001100,
	DISPLAY_TEST = 0b00001111
}maxComandDef;

/*********************************************************************/

// decodeModeDef
typedef enum{
	DECODE_B_Dig0 = 0b1,
	DECODE_B_Dig1 = 0b10,
	DECODE_B_Dig2 = 0b100,
	DECODE_B_Dig3 = 0b1000,
	DECODE_B_Dig4 = 0b10000,
	DECODE_B_Dig5 = 0b100000,
	DECODE_B_Dig6 = 0b1000000,
	DECODE_B_Dig7 = 0b10000000,
}maxConfigScanLimDef;


typedef enum{
	SCAN_LIM_0 = 0,
	SCAN_LIM_1,
	SCAN_LIM_2,
	SCAN_LIM_3,
	SCAN_LIM_4,
	SCAN_LIM_5,
	SCAN_LIM_6,
	SCAN_LIM_7,
}maxScanLimDef;


typedef enum{
	INTENSITY_3_32 = 0,
	INTENSITY_5_32,
	INTENSITY_7_32,
	INTENSITY_9_32,
	INTENSITY_11_32,
	INTENSITY_13_32,
	INTENSITY_15_32,
	INTENSITY_17_32,
	INTENSITY_19_32,
	INTENSITY_21_32,
	INTENSITY_23_32,
	INTENSITY_25_32,
	INTENSITY_27_32,
	INTENSITY_29_32,
	INTENSITY_31_32,
}maxIntensityDef;


typedef enum{
	SHUT_DOWN_SHUTDOWN_MODE = 0,
	SHUT_DOWN_NORMAL_OPERATION
}shutDownDef;


typedef enum{
	DISPLAY_TEST_NORMAL_OPERATION = 0,
	DISPLAY_TEST_DISPLAY_TEST_MODE
}maxTestDef;


// Update screen function
void displayClearBuff(displayBuffDef *displayBuffer, uint8_t numMax);
void displaySet7Segment(displayBuffDef *displayBuffer, uint8_t numMax, uint8_t data, uint8_t numDig);
void displaySet8x8Matrix(displayBuffDef *displayBuffer, uint8_t numMax, uint8_t data);
void displaySetConfig(displayBuffDef *displayBuffer, uint8_t numMax,  maxComandDef maxComand, uint8_t data);
void displaySetConfigMulti(displayBuffDef *displayBuffer, uint8_t sizeBuff, maxComandDef maxComand, uint8_t data);
void displayConfigDecodeMode(displayBuffDef *displayBuffer, uint8_t numMax, uint8_t data);
void displayConfigIntensity(displayBuffDef *displayBuffer, uint8_t numMax, maxIntensityDef data);
void displayConfigWorkMode(displayBuffDef *displayBuffer, uint8_t numMax, shutDownDef data);
void displayConfigScanLimit(displayBuffDef *displayBuffer, uint8_t numMax, maxScanLimDef data);

#endif
