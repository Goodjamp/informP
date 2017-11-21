/********************************************************************************
 * @file    keyBoardProcessing.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-September-2016
 * @brief
 */
#include "stdint.h"
#include "stdbool.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "keyBoardProcessing.h"
#include "LCD.h"



const actionDescrDef actionDescr[] = {
	[ACTION_MODE] = {
		.comboButtonMask = 1 << KEY_MODE,
		.numPeriod = 2,
		.periodForAction   = {ACTION_MODE_PERIOD,400},
	},
	[ACTION_SELL] = {
		.comboButtonMask = 1 << KEY_SELL,
		.numPeriod = 1,
		.periodForAction   = {ACTION_SELL_PERIOD},
	},
	[ACTION_TEST] = {
		.comboButtonMask = ( 1 << KEY_MODE | 1 << KEY_SELL),
		.numPeriod = 1,
		.periodForAction   = {ACTION_TEST_PERIOD},

	},
};


// Action processing
struct{
	uint16_t cnt;
	uint8_t  index;
	bool     detect;
}actionProces;


actionButtonDef keyUpdate( void ){
    uint8_t cnt;
	uint16_t mask = keyGetPressMask();


	// detect action
	actionProces.detect = false;

    for(cnt = 0; cnt < sizeof(actionDescr)/sizeof(actionDescr[0]); cnt++)
    {
        if(mask != actionDescr[cnt].comboButtonMask)
        	continue;

        if( actionProces.index != cnt)
        {
        	actionProces.index = cnt;
        	actionProces.cnt = 0;
        }
        actionProces.detect = true;
        actionProces.cnt++;
    }


    if( actionProces.detect )
    {
    	if(actionProces.cnt >= actionDescr[actionProces.index].periodForAction[actionDescr[actionProces.index].numPeriod - 1] ) // action was detected !!
    	{
            //TO DO call callback function
    		//clear processing
            actionProces.cnt = 0;
    	}
    }
    else if( !actionProces.detect && ( actionProces.cnt > 0) )
    {
    	cnt = 0;
    	// finde nearest time period
    	while( ( cnt < actionDescr[actionProces.index].numPeriod ) && (actionDescr[actionProces.index].periodForAction[cnt] < actionProces.cnt ) )
    	{
            cnt++;
    	}
    	if( cnt != 0 ) //
    	{
    		actionProces.cnt++;
    	}
    	actionProces.cnt = 0;
    }

}









