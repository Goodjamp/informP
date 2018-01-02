/********************************************************************************
 * @file    keyBoardProcessing.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-September-2016
 * @brief
 */
#include "stdint.h"
#include "stdbool.h"

#include "keyBoardProcessing.h"
#include "keyBoardProcessingUserInterface.h"



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
	uint16_t deadTimeRevCnt;
	bool     detect;
}actionProces;


void keyboardInit( void ){
	keyHWConfig();
}

void keyPause(uint16_t pause){
	actionProces.cnt = 0;
	actionProces.deadTimeRevCnt = pause;
	actionProces.detect = false;
}


actionDetect keyUpdate(volatile uint8_t *actionNum, volatile uint8_t *timePeriodNumber ){
    uint8_t cnt;
    actionDetect rezButProcessing = ACTION_NO_DETECT;
	uint16_t mask = keyGetPressMask();
	// detect action
	actionProces.detect = false;
	// if pause set
	if(actionProces.deadTimeRevCnt > 0)
	{
		actionProces.deadTimeRevCnt--;
		return 	rezButProcessing;
	}

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
            // return detected action and period
    		*actionNum        = actionProces.index;
    		*timePeriodNumber = actionDescr[actionProces.index].numPeriod - 1;
    		//clear processing
            actionProces.cnt = 0;
            rezButProcessing = 	ACTION_DETECT;
    	}
    }
    else if( !actionProces.detect && ( actionProces.cnt > 0) )
    {
    	cnt = 0;
    	// find nearest time period
    	while( ( cnt < actionDescr[actionProces.index].numPeriod ) && (actionDescr[actionProces.index].periodForAction[cnt] < actionProces.cnt ) )
    	{
            cnt++;
    	}
    	if( cnt != 0 ) //
    	{
    	    // return detected action and period
    	    *actionNum        = actionProces.index;
    	    *timePeriodNumber = cnt-1;
    	    rezButProcessing = 	ACTION_DETECT;
        }
    	//clear processing
    	actionProces.cnt = 0;
    }
    return 	rezButProcessing;
}









