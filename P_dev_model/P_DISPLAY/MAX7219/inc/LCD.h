/********************************************************************************
 * @file    LCD.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date     15-November-2017
 * @brief
 */

#ifndef SCREEN_PROCESSING_H_
#define SCREEN_PROCESSING_H_

#include "max7219.h"
#include "processing_display_extern.h"

/********SELECTED TYPE UF INTERFACE******/
//#define USE_SPI
#define USE_TIMER

// max7219 Position matrix LCD in screen
#define ORDER_NUM_MATRIX        0
// max7219 Position 7-SEGMENTS LCD in screen
#define ORDER_NUM_7SEG          1
// Number of 7 M
#define MAX_PER_SCREEN          2

typedef enum{
	COLOR_GREEN = 0,
	COLOR_RED,
	COLOR_ORANGE
}COLOR;

typedef enum{
	DISPLAY_BRIGHTNES_3 = 0,
	DISPLAY_BRIGHTNES_5,  //1
	DISPLAY_BRIGHTNES_7,  //2
	DISPLAY_BRIGHTNES_9,  //3
	DISPLAY_BRIGHTNES_11, //4 -
	DISPLAY_BRIGHTNES_15, //5
	DISPLAY_BRIGHTNES_17, //6
	DISPLAY_BRIGHTNES_19, //7
	DISPLAY_BRIGHTNES_21, //8 -
	DISPLAY_BRIGHTNES_23, //9
	DISPLAY_BRIGHTNES_25, //10
	DISPLAY_BRIGHTNES_27, //11
	DISPLAY_BRIGHTNES_29, //12
	DISPLAY_BRIGHTNES_31, //13
}DISPLAY_BRIGHTNES;

/* @brief
 *
 */
typedef enum{
	SCREEN_1 = 0,
	SCREEN_2 = 1,
	SCREEN_3 = 2,
	SCREEN_4 = 3,
}screenOrder;


/* @brief
 * */
typedef enum {
	TX_ADDRESS_ONE = 0x0,
	TX_ADDRESS_ALL = 0x1F,
}TX_ADDRESS;



/* @brief
 * */
typedef enum {
	DISPLAY_OK,
	DISPLAY_BUSY,
}DISPLAY_STATUS;


typedef enum{
	TX_DATA,
	GEN_LD,
	TX_COMPLITE
}txState;

typedef struct{
	uint8_t brightnes;
	COLOR   color[NUMBER_OF_LCD_STRING];

}displayCurrentSetings;

/* @brief
 * */
typedef struct{
	displayCurrentSetings    currentSettings;
	const uint8_t *          brightnesList;
	volatile DISPLAY_STATUS  status;
	uint16_t                 dataCnt;   // Data transmit counter  (set user according number of max chips per display)
	uint8_t                  digitCnt;  // digit transmit counter (count to max number of digits NUM_MAX_DIGITS)
	uint8_t                  numForTx;  // number of data to Tx per one MAX
	displayBuffDef           txData[MAX_PER_SCREEN];    // pointer on the buffer for Tx data
}displayHandlerDef;


//--------------------------------------------------------------------------
//---------DISPLAY HAL PROCESSING FUNCTION PROTOTYPE------------------------
//--------------------------------------------------------------------------
DISPLAY_STATUS displayInit( displayHandlerDef *displayHandlerIn, const uint8_t* brightnesPar);
void displayWrite(displayHandlerDef *displayHandlerIn, uint16_t numScreen, uint8_t *str, uint16_t strSize, COLOR color, TX_ADDRESS txAddress);
void displaySetDefConfig(displayHandlerDef *displayHandler);
void displayClear(displayHandlerDef *displayHandlerIn, uint16_t numScreen, TX_ADDRESS txAddress);
DISPLAY_STATUS displayIntarfaceGetStatus(displayHandlerDef const *displayHandler);
void displaySetBrightnes(displayHandlerDef *displayHandlerIn, DISPLAY_BRIGHTNES brightnes, uint16_t numScreen, TX_ADDRESS txAddress);




//---------------User implementation functions---------------------
void hwInterfaceInit(void);
void hwInterfaceTx(uint16_t orderNumberDispl, TX_ADDRESS txAddress);

//TO DO move on other file
txState getNextData(uint16_t *nexSymbol);

#endif
