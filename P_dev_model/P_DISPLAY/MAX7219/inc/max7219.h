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

#ifdef STM32F10X_MD_VL
	#include "stm32f10x.h"
#endif
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

#define NUM_MAX_DIGITS     8


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
typedef uint8_t maxConfigScanLin;


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


/* @brief
 * */
typedef enum {
	DISPLAY_OK,
	DISPLAY_BUSY,
	DISPLAY_ERROR_INIT,
	DISPLAY_ERROR_TX_NUM
}DISPLAY_STATUS;


typedef enum{
	TX_DATA,
	GEN_LD,
	TX_COMPLITE
}txState;

/* @brief Description video bufer for one MAX7219
 * */
typedef uint16_t displayBuffDef[NUM_MAX_DIGITS];

/* @brief
 * */
typedef struct{
	uint16_t         dataCnt;   // Data transmit counter  (set user according number of max chips per display)
	uint8_t          digitCnt;  // digit transmit counter (count to max number of digits NUM_MAX_DIGITS)
	uint8_t          txSize;    // number of maxBuffTypedef (the same as number of MAX chips per display)
	displayBuffDef  *txData;
}displayTxHandlerDef;

/* @brief
 * */
typedef struct{
	DISPLAY_STATUS       status;
	displayTxHandlerDef  txBuff;
}displayHandlerDef;

txState displayTxCallback(displayHandlerDef *displayHandler, uint16_t *nexSymbol);
DISPLAY_STATUS displayTx(displayHandlerDef *displayHandler, uint8_t numData);
DISPLAY_STATUS displayInterfaceInit( displayHandlerDef *displayHandler, displayBuffDef *displayBuff);
DISPLAY_STATUS displayIntarfaceGetStatus(displayHandlerDef const *max7219Interface);


// Update screen function
void displaySet7Segment(displayHandlerDef  *displayIntarface, uint8_t numMax, uint8_t data, uint8_t numDig);
void displaySet8x8Matrix(displayHandlerDef *displayIntarface, uint8_t numMax, uint8_t data);


// Config display function
void displayClearBuff(displayHandlerDef *displayIntarface, uint8_t numMax);
void displaySetConfig(displayHandlerDef        *displayIntarface, uint8_t numMax,  maxComandDef maxComand, uint8_t data);
void displaySetConfigMulti(displayHandlerDef   *displayIntarface, uint8_t sizeBuff, maxComandDef maxComand, uint8_t data);
void displayConfigDecodeMode(displayHandlerDef *displayIntarface, uint8_t numMax, uint8_t data);
void displayConfigIntensity(displayHandlerDef * displayIntarface, uint8_t numMax, maxIntensityDef data);
void displayConfigWorkMode(displayHandlerDef   *displayIntarface, uint8_t numMax, shutDownDef data);
void displayConfigScanLimit(displayHandlerDef  *displayIntarface, uint8_t numMax, maxScanLimDef data);

#endif
