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

#define DISPLAY_SPI  SPI2

#define numberOfScreen  4

#define ORDER_NUM_MATRIX        0
#define ORDER_NUM_7SEG          1
#define NUMBER_7_SEGMENTS_IND   4
#define MAX_PER_SCREEN          2


typedef enum{
	COLOR_GREEN = 0,
	COLOR_RED      ,
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


#define PORT_LD_1  GPIOB
#define PORT_LD_2  GPIOB
#define PORT_LD_3  GPIOB
#define PORT_LD_4  GPIOB

#define PIN_LD_1   GPIO_Pin_12
#define PIN_LD_2   GPIO_Pin_8
#define PIN_LD_3   GPIO_Pin_14
#define PIN_LD_4   GPIO_Pin_9

uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start);

#endif // PROCESSING_DISPLAY_H_
