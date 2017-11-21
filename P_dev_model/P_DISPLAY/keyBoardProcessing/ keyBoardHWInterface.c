/********************************************************************************
 * @file    keyBoardHWInterface.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    18-November-2017
 * @brief
 */
#include "stdint.h"

#include "keyBoardProcessing.h"
#include "keyBoardHWInterface.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


//buttonProces buttonProces;
const static keyDescDef keyDesc[NUMBER_OF_BUTTON] = {
		[KEY_MODE] = {
				.keyPort = BUTTON_MODE_PORT,
				.keyPin  = BUTTON_MODE_PIN,
				.keyMask = (uint16_t)(1<<KEY_MODE),
		},
		[KEY_SELL] = {
				.keyPort = BUTTON_SEL_PORT,
				.keyPin  = BUTTON_SEL_PIN,
				.keyMask = (uint16_t)(1<<KEY_SELL),
		}
};

volatile static uint8_t bounsCnt[NUMBER_OF_BUTTON];



static void enableGPIO(GPIO_TypeDef *GPIOx){
	if(GPIOx == GPIOA)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE);
	}
	else if(GPIOx == GPIOB)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,  ENABLE);
	}
	else if(GPIOx == GPIOC)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,  ENABLE);
	}
	else if(GPIOx == GPIOD)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,  ENABLE);
	}
	else if(GPIOx == GPIOE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,  ENABLE);
	}
	else
	{
		return;
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
}


void keyHWConfig(void){
	GPIO_InitTypeDef gpioConfig;

	enableGPIO(keyDesc[KEY_MODE].keyPort);
	enableGPIO(keyDesc[KEY_SELL].keyPort);

	gpioConfig.GPIO_Mode = GPIO_Mode_IPU;
	gpioConfig.GPIO_Speed = GPIO_Speed_2MHz;

	gpioConfig.GPIO_Pin = keyDesc[KEY_MODE].keyPin;
	GPIO_Init(keyDesc[KEY_MODE].keyPort, &gpioConfig);


	gpioConfig.GPIO_Pin = keyDesc[KEY_SELL].keyPin;
	GPIO_Init(keyDesc[KEY_SELL].keyPort, &gpioConfig);
}

void keyClear(uint16_t mask){
	uint8_t cnt;
	for(cnt = 0; cnt < NUMBER_OF_BUTTON; cnt++)
	{
		if(keyDesc[cnt].keyMask == mask)
		{
			bounsCnt[cnt] = 0;
		}
	}
}



uint16_t keyGetPressMask(void){
	uint8_t cnt = 0;
	uint16_t mask = 0;
	for(; cnt < NUMBER_OF_BUTTON; cnt++)
	{
		if( GPIO_ReadInputDataBit(keyDesc[cnt].keyPort, keyDesc[cnt].keyPin) )
		{
			bounsCnt[cnt] = 0;
			continue;
		}
        // bouns filter
		if(bounsCnt[cnt] >= BOUNS_PERIOD)
		{
			mask |= keyDesc[cnt].keyMask;
			continue;
		}
		bounsCnt[cnt]++;

	}
	return mask;
}
