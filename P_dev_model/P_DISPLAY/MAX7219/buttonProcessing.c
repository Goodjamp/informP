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

const actionDescrDef actionDescr[] = {
	[ACTION_MODE] = {
		.periodBefore    = ACTION_MODE_PERIOD_BEFORE,
		.periodAfter     = ACTION_MODE_PERIOD_AFTER,
		.comboButtonMask = 1 << BUTTON_MODE,
		.timePosAction   = BEFORE_PERIOD_AFTER
	},
	[ACTION_SEL] = {
		.periodBefore    = ACTION_SELL_PERIOD_BEFORE,
		.periodAfter     = ACTION_SELL_PERIOD_AFTER,
		.comboButtonMask = 1 << BUTTON_SEL,
		.timePosAction   = BEFORE_PERIOD_AFTER
	},
	[ACTION_TEST_] = {
		.periodBefore    = ACTION_TEST_PERIOD_BEFORE,
		.periodAfter     = ACTION_TEST_PERIOD_AFTER,
		.comboButtonMask = ( 1 << BUTTON_MODE | 1 << BUTTON_SEL),
		.timePosAction   = AFTER_PERIOD_AFTER
	},
};

//buttonProces buttonProces;
buttonProcesH buttonProces[NUMBER_OF_BUTTON] = {
		[BUTTON_MODE] = {
				.portButton = BUTTON_MODE_PORT,
				.pinButton  = BUTTON_MODE_PIN,
				.buttonMask = 1 << BUTTON_MODE,
		},
		[BUTTON_SEL] = {
				.portButton = BUTTON_SEL_PORT,
				.pinButton  = BUTTON_SEL_PIN,
				.buttonMask = 1 << BUTTON_SEL,
		}
};

// Action processing
struct{
	actionProcessingDef actionProcessing[NUMBER_OF_ACTION];
	uint8_t actionIndex;
	bool    actionProcess;
}actionProcessing;


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
/*
const uint16_t buttonAction[] = {

};
*/

actionButtonDef updateButtonCallBack( void ){
	uint8_t cnt = 0;
	uint8_t pressButtonMask = 0;
//	low level processing button (pin level change detect)
	for(;cnt < NUMBER_OF_BUTTON; cnt++){
		if( GPIO_ReadInputDataBit( buttonProces[cnt].portButton, buttonProces[cnt].pinButton ) == Bit_RESET )
		{

			if( buttonProces[cnt].pressF == BUTTON_PRESS )
			{
				buttonProces[cnt].pressF = BUTTON_NOT_PRESS;
				buttonProces[cnt].pressStateCnt = 0;
			}
			continue;
		}
		// if pin save level more then set period, clear current level
		/*
		if( buttonProces[cnt].pressStateCnt >= NUMBER_CYCLES_BUTTON_PRESS)
		{
			buttonProces[cnt].pressF = BUTTON_NOT_PRESS;
			continue;
		}
*/
		//
		if(buttonProces[cnt].pressF == BUTTON_NOT_PRESS)
		{
			buttonProces[cnt].pressF = BUTTON_PRESS;
			buttonProces[cnt].pressF++;
		}
		else
		{
			buttonProces[cnt].pressStateCnt++;
		}
		pressButtonMask |= buttonProces[cnt].buttonMask;

	}

//	HIGHT level  processing Action (after detection Action)
	if( actionProcessing.actionProcess )
	{
		uint8_t indexProc = actionProcessing.actionIndex;
		if(actionDescr[indexProc].comboButtonMask != pressButtonMask)
		{
			actionProcessing.actionProcess = false;
			actionProcessing.actionProcessing[indexProc].actionCntAfter = 0;
			actionProcessing.actionProcessing[indexProc].actionCntBefore = 0;
		}
		else
		{
			if( actionProcessing.actionProcessing[indexProc].actionCntAfter <= actionDescr[indexProc].periodAfter )
			{
				actionProcessing.actionProcessing[indexProc].actionCntAfter++;
				return;
			}
			//
			if( actionDescr[indexProc].timePosAction == AFTER_PERIOD_AFTER)
			{
					// TO DO Call function processing Action
			}
			// clear last Action
			actionProcessing.actionProcess = false;
			actionProcessing.actionProcessing[indexProc].actionCntAfter = 0;
			actionProcessing.actionProcessing[indexProc].actionCntBefore = 0;
			return;
		}

	}

// MEDIUM  level  processing Action
	actionProcessing.actionProcess = false;
	for(cnt =0; cnt < NUMBER_OF_ACTION; cnt++)
	{
		// find equal button combo template
		if(pressButtonMask == actionDescr[cnt].comboButtonMask)
		{
			actionProcessing.actionProcessing[cnt].actionCntBefore++;
			actionProcessing.actionProcess = true;
			actionProcessing.actionIndex = cnt;
		}
		else
		{
			actionProcessing.actionProcessing[cnt].actionCntBefore = 0;
		}
	}

	if(actionProcessing.actionProcess)
	{
		if( (actionProcessing.actionProcessing[actionProcessing.actionIndex].actionCntBefore-1) <= actionDescr[actionProcessing.actionIndex].periodBefore)
		{
			return;
		}
		// Call Action callback
		/*
		if( actionDescr[indexProc].timePosAction == BEFORE_PERIOD_AFTER)
		{
				// TO DO Call function processing Action
		}
		*/

	}

}










