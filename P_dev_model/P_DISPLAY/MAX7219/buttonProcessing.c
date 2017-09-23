/**
 ******************************************************************************
 * @file    buttonProcessing.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-September-2016
 * @brief
 */
#include "stdint.h"
#include "stdio.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "buttonProcessing.h"

//buttonProces buttonProces;


void buttonGpioConfig(void){
	GPIO_InitTypeDef gpioConfig;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpioConfig.GPIO_Mode = GPIO_Mode_IPU;
	gpioConfig.GPIO_Pin = BUTTON_MODE_PIN;
	gpioConfig.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BUTTON_MODE_PORT, &gpioConfig);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpioConfig.GPIO_Pin = BUTTON_SEL_PIN;
	GPIO_Init(BUTTON_SEL_PORT, &gpioConfig);
}


void updateButtonCallBack( void ){
	uint8_t cnt = 0;

	for(;cnt < NUMBER_OF_BUTTON; cnt++){

	}

}
