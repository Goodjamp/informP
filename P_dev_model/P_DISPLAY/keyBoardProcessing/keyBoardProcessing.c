/********************************************************************************
 * @file    keyBoardProcessing.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-September-2016
 * @brief
 */
#include "stdint.h"
#include "stdio.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "keyBoardProcessing.h"



// Action processing
struct{
	actionProcessingDef actionProcessing[NUMBER_OF_ACTION];
	uint8_t actionIndex;
	bool    actionProcess;
}actionProcessing;

const actionDescrDef actionDescr[] = {
	[ACTION_MODE] = {
		.periodBefore    = ACTION_MODE_PERIOD_BEFORE,
		.periodAfter     = ACTION_MODE_PERIOD_AFTER,
		.comboButtonMask = 1 << KEY_MODE,
		.timePosAction   = BEFORE_PERIOD_AFTER
	},
	[ACTION_SEL] = {
		.periodBefore    = ACTION_SELL_PERIOD_BEFORE,
		.periodAfter     = ACTION_SELL_PERIOD_AFTER,
		.comboButtonMask = 1 << KEY_SEL,
		.timePosAction   = BEFORE_PERIOD_AFTER
	},
	[ACTION_TEST_] = {
		.periodBefore    = ACTION_TEST_PERIOD_BEFORE,
		.periodAfter     = ACTION_TEST_PERIOD_AFTER,
		.comboButtonMask = ( 1 << KEY_MODE | 1 << KEY_SEL),
		.timePosAction   = AFTER_PERIOD_AFTER
	},
};

/*
const uint16_t buttonAction[] = {

};
*/

actionButtonDef keyUpdate( void ){
	uint8_t cnt = 0;
	uint8_t pressButtonMask = 0;
//	low level processing button (pin level change detect)

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










