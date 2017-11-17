/**
  ******************************************************************************
  * @file     debugStuff.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    4-November-2017
  * @brief
  */

#include "stdint.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "debugStuff.h"

void debugPinConfig(void){
	GPIO_InitTypeDef initGpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	initGpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	initGpio.GPIO_Speed = GPIO_Speed_10MHz;
	initGpio.GPIO_Pin   = debugPin1;
	GPIO_Init(debugPort1, &initGpio);

	initGpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	initGpio.GPIO_Speed = GPIO_Speed_10MHz;
	initGpio.GPIO_Pin   = debugPin2;
	GPIO_Init(debugPort2, &initGpio);
}

