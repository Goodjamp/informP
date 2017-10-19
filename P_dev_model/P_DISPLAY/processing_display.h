/**
  ******************************************************************************
  * @file    processing_display.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */

#ifndef PROCESSING_DISPLAY_H_
#define PROCESSING_DISPLAY_H_

#include "stm32f10x_gpio.h"

#include "max7219.h"
#include "stmMaxHardwareInit.h"

#define DISPLAY_SPI  SPI2

#define numberOfScreen  4

#define ORDER_NUM_MATRIX        0
#define ORDER_NUM_7SEG          1
#define NUMBER_7_SEGMENTS_IND   4
#define MAX_PER_SCREEN          2

#define PORT_LD_1  GPIOB
#define PORT_LD_2  GPIOB
#define PORT_LD_3  GPIOB
#define PORT_LD_4  GPIOB

#define PIN_LD_1   GPIO_Pin_12
#define PIN_LD_2   GPIO_Pin_8
#define PIN_LD_3   GPIO_Pin_14
#define PIN_LD_4   GPIO_Pin_9

#define SCREEN_UPDATE_PERIOD    20

typedef enum{
	COLOR_GREEN = 0,
	COLOR_RED,
	COLOR_ORANGE
}COLOR;


/* @brief
 *
 */
typedef enum{
	SCREEN_1 = 0,
	SCREEN_2 = 1,
	SCREEN_3 = 2,
	SCREEN_4 = 3,
}screenOrder;


#define MAX_NUM_PAR            6

typedef enum{
	PARAM_SYMB_POS_DATE        = 0,
	PARAM_SYMB_POS_TIME        = 1,
	PARAM_SYMB_POS_FREQUENCY   = 2,
	PARAM_SYMB_POS_HUMIDITY    = 3,
	PARAM_SYMB_POS_ATM_PRESURE = 4,
	PARAM_SYMB_POS_TEMPERATURE = 5
}PARAM_SYMB_POS;

// Structure described item inside widget ListBox
typedef struct{
	uint8_t parSymbol;
	uint16_t parAddress;
	uint32_t data;
}itemListBox;


typedef struct{
	itemListBox listOfPar[MAX_NUM_PAR];
}ListBox;

// structure described menu
typedef struct{
	uint8_t selPos;      // order number of selected menu item (list), should be les then 0x11111110 = 0x254
	uint8_t numPar;      // quantity of menu item
}listBox;

typedef enum{
	DISPLAY_STATE_WORK,
	DISPLAY_STATE_TEST
}DISPLAY_STATE;


typedef enum{
	ACTION_USER,
	ACTION_TIMER,
}DIASPALY_ACTION;

#define isDIASPALY_ACTION(X) (( X == ACTION_USER)|| \\
							  ( X == ACTION_TIMER) )

typedef enum{
	ACTION_ENTER,
	ACTION_ESC,
	ACTION_SELL,
	ACTION_TEST
}USER_ACTION; // connected with Press Button


//--------------------------------------------------------------------------
//---------DISPLAY HAL PROCESSING FUNCTION PROTOTYPE------------------------
//--------------------------------------------------------------------------
void updateScreen(displayHandlerDef *displayHandler, uint16_t numScreen, uint8_t *str, uint16_t strSize, COLOR color, TX_ADDRESS txAddress);
void refreshDisplays(void);

//--------------------------------------------------------------------------
//---------MENU PROCESSING FUNCTION PROTOTYPE-------------------------------
//--------------------------------------------------------------------------
uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start);
void userActionReaction(USER_ACTION inAction);
void listBoxProcess(USER_ACTION inAction);
void listBoxChangePar(void);
void increaseWidjetP(void);
void decreaseWidjetP(void);

#endif // PROCESSING_DISPLAY_H_
