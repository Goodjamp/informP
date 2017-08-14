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

/* @brief Max number of Ld line (max7219) per one SPI
 * */
#define MAX7219_MAX_NUMBER 12
/* @brief Max number of SPI in SOC
 * */
#define SPI_MAX_number     12
/* @brief number of max chips per 1 srting
 * */
#define NUM_MAX_PERF_STR 2

/* @brief size of video memory
 * */
#define SIZE_VIDEO_FREIM 2


/*********************************************************************/
/******************   MAX7219 COMAND LIST    *************************/
/*********************************************************************/
#define NO_OP          (uint8_t)0b00000000
#define DECODE_MODE    (uint8_t)0b00001001
#define INTENSITY      (uint8_t)0b00001010
#define SCAN_LIMIT     (uint8_t)0b00001011
#define SHUTDOWN       (uint8_t)0b00001100
#define DISPLAY_TEST   (uint8_t)0b00001111
/*********************************************************************/

// decodeModeDef
#define DECODE_B_Dig0  (uint8_t)0b1
#define DECODE_B_Dig1  (uint8_t)0b10
#define DECODE_B_Dig2  (uint8_t)0b100
#define DECODE_B_Dig3  (uint8_t)0b1000
#define DECODE_B_Dig4  (uint8_t)0b10000
#define DECODE_B_Dig5  (uint8_t)0b100000
#define DECODE_B_Dig6  (uint8_t)0b1000000
#define DECODE_B_Dig7  (uint8_t)0b10000000

typedef enum{
	SCAN_LIM_0 = 0,
	SCAN_LIM_1,
	SCAN_LIM_2,
	SCAN_LIM_3,
	SCAN_LIM_4,
	SCAN_LIM_5,
	SCAN_LIM_6,
	SCAN_LIM_7,
}scanLimDef;


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
}intensityDef;


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


typedef struct{
	uint8_t typeDecode: 1;
	uint8_t intensity:  4;
	uint8_t reserved:   3;
}maxConfigDef;


/* @brief
 * */
typedef enum{
	CURRENT,
	BLINC
}maxMode;

/* @brief
 * */
typedef enum{
	RED,
	GREEN,
	ORANGE
}maxColor;

/* @brief
 * */
typedef struct{
	uint8_t        txCounter;
	uint8_t        txSize;
	uint16_t       *txData;
	GPIO_TypeDef   *portLd;
	uint32_t       pinLd;
	uint8_t        txAddress; // pointed on the LD target: for current MAX or ALL max in the list
}_max7219TxHandler;

/* @brief
 * */
typedef struct{
	SPI_TypeDef      *selSPI;
	MAX7219_STATUS   status;
	_max7219TxHandler txHandler;
	void             *ldHandler[8*MAX7219_MAX_NUMBER]; // Array fore save pointer on max7219Handler
	uint8_t          ldCounter;    // counter o
}max7219Handler;

/* @brief
 * */
typedef struct{
	GPIO_TypeDef   *portLd;
	uint32_t       pinLd;
	max7219Handler *interface;
}max7219LdHandler;

/* @brief
 * */
typedef struct{
	max7219Handler   *max7219InterfaceHandler[MAX7219_MAX_NUMBER]; // Array fore save pointer on max7219Handler
	uint8_t          spiCounter;
}_max7219Interface;



MAX7219_STATUS max7219InterfaceInit(max7219Handler *max7219Interface);
MAX7219_STATUS max7219AddLd(max7219LdHandler *max7219Ld, max7219Handler *max7219Interface);
MAX7219_STATUS max7219Tx(max7219LdHandler *max7219Ld, uint8_t numData, uint16_t *pData, TX_ADDRESS txAddress);
MAX7219_STATUS max7219GetStatus(max7219Handler const *max7219Interface);

#endif
