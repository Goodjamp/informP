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
	TX_ADDRESS_ONE = 0x0,
	TX_ADDRESS_ALL = 0x1F,
}TX_ADDRESS;


/* @brief
 * */
typedef enum {
	MAX7219_OK,
	MAX7219_BUSY,
	MAX7219_ERROR_INIT,
	MAX7219_ERROR_LD_THRESHOLD,
	MAX7219_ERROR_TX_NUM
}MAX7219_STATUS;


/* @brief Description video bufer for one MAX7219
 * */
typedef uint16_t displayBuffDef[NUM_MAX_DIGITS];

typedef void (*interfaseTxDef)(uint16_t data);
typedef void (*setLDDef)(uint16_t numberOfDisp, TX_ADDRESS txAddress);

typedef struct{
	interfaseTxDef   txDadaCB;
	setLDDef         setLDCB;
}displayCallbackDef;

/* @brief
 * */
typedef struct{
	uint16_t         dataCnt;   // Data transmit counter  (set user according number of max chips per display)
	uint8_t          digitCnt;  // digit transmit counter (count to max number of digits NUM_MAX_DIGITS)
	uint8_t          txSize;    //number of maxBuffTypedef (the same as number of MAX chips per display)
	displayBuffDef  *txData;
	uint8_t          txAddress; // pointed on the Display target: for current Display or ALL Display from the list
}displayTxHandlerDef;

/* @brief
 * */
typedef struct{
	MAX7219_STATUS      status;
	displayTxHandlerDef  txBuff;
	void                *dispList;     // pointer on Array for save pointer on max7219Handler
	uint16_t   		  orderDispl;
	uint8_t           displayCnt;   // counter
}displayHandlerDef;


void displaySetCallback(displayCallbackDef *displayCallbackIn);
void displayTxCallback(displayHandlerDef *tmpInterH);
void displayStartGenLD(void);
MAX7219_STATUS displayTx(displayHandlerDef *displayHandler, uint16_t orderNumberDispl, uint8_t numData, displayBuffDef *pData, TX_ADDRESS txAddress);
MAX7219_STATUS displayInterfaceInit(displayHandlerDef *max7219Interface, void* displayList, uint16_t sizeDisplayList);
MAX7219_STATUS displayIntarfaceGetStatus(displayHandlerDef const *max7219Interface);

void displaySetMatrix(uint8_t numMax, uint8_t data, displayBuffDef *maxBuff);
void displaySetConfig(uint8_t numMax, maxComandDef maxComand, uint8_t data, displayBuffDef *maxBuff);
void displayConfigDecodeMode(uint8_t numMax, uint8_t data, displayBuffDef *maxBuff);
void displayConfigIntensity(uint8_t numMax, maxIntensityDef data, displayBuffDef *maxBuff);

#endif
