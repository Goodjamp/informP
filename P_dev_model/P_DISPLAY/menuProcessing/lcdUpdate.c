/********************************************************************************
 * @file    lcdUpdate.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    13-December-2017
 * @brief
 */
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "stddef.h"

#include "lcdUpdate.h"
#include "menuProcessing.h"
#include "processing_display.h"
#include "processing_display_extern.h"
#include "processing_mem_map_extern.h"

extern S_address_oper_data s_address_oper_data;

typedef void (*updateFun)(uint8_t*, uint16_t, uint16_t);

const static uint8_t errorInf[] = ERROR_INF;
const static uint8_t testStr[]  = TEST_STR;


struct {
	uint8_t sumbol[3];
	uint16_t addressStatus;
	uint16_t addressValue;
}paramIndication[QUANTITY_OF_VALUE] = {
		[PAR_TEMPERATURE] = { .sumbol = { SYMBOL_TEMPERATURE }, },
		[PAR_PRESSURE_PA] = { .sumbol = { SYMBOL_PRESSURE_PA}, },
		[PAR_PRESSURE_MM] = { .sumbol = { SYMBOL_PRESSURE_MM}, },
		[PAR_HUMIDITY]    = { .sumbol = { SYMBOL_HUMIDITY }, },
		[PAR_DATE_0]        = { .sumbol = { SYMBOL_DATE }, },
		[PAR_TIME_0]        = { .sumbol = { SYMBOL_TIME }, },
		[PAR_DATE_1]        = { .sumbol = { SYMBOL_DATE }, },
		[PAR_TIME_1]        = { .sumbol = { SYMBOL_TIME }, },
		[PAR_DATE_2]        = { .sumbol = { SYMBOL_DATE }, },
		[PAR_TIME_2]        = { .sumbol = { SYMBOL_TIME }, },
		[PAR_DATE_3]        = { .sumbol = { SYMBOL_DATE }, },
		[PAR_TIME_3]        = { .sumbol = { SYMBOL_TIME }, },
		[PAR_FRQ]         = { .sumbol = { SYMBOL_FRQ }, }
};


void initValueAddress(void){
	// copy address of all values that can indication
	paramIndication[PAR_TEMPERATURE].addressValue  = s_address_oper_data.s_sensor_address.rezTemperature;
	paramIndication[PAR_TEMPERATURE].addressStatus = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_PRESSURE_PA].addressValue  = s_address_oper_data.s_sensor_address.rezPressure_GPasc;
	paramIndication[PAR_PRESSURE_PA].addressStatus = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_PRESSURE_MM].addressValue  = s_address_oper_data.s_sensor_address.rezPressure_mmHg;
	paramIndication[PAR_PRESSURE_MM].addressStatus = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_HUMIDITY].addressValue     = s_address_oper_data.s_sensor_address.rezHumidity;
	paramIndication[PAR_HUMIDITY].addressStatus    = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_DATE_0].addressValue       = s_address_oper_data.s_TIME_address.clock[0].DATE;
	paramIndication[PAR_DATE_0].addressStatus      = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_TIME_0].addressValue       = s_address_oper_data.s_TIME_address.clock[0].TIME;
	paramIndication[PAR_TIME_0].addressStatus      = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_DATE_1].addressValue       = s_address_oper_data.s_TIME_address.clock[1].DATE;
	paramIndication[PAR_DATE_1].addressStatus      = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_TIME_1].addressValue       = s_address_oper_data.s_TIME_address.clock[1].TIME;
	paramIndication[PAR_TIME_1].addressStatus      = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_DATE_2].addressValue       = s_address_oper_data.s_TIME_address.clock[2].DATE;
	paramIndication[PAR_DATE_2].addressStatus      = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_TIME_2].addressValue       = s_address_oper_data.s_TIME_address.clock[2].TIME;
	paramIndication[PAR_TIME_2].addressStatus      = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_DATE_3].addressValue       = s_address_oper_data.s_TIME_address.clock[3].DATE;
	paramIndication[PAR_DATE_3].addressStatus      = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_TIME_3].addressValue       = s_address_oper_data.s_TIME_address.clock[3].TIME;
	paramIndication[PAR_TIME_3].addressStatus      = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_FRQ].addressValue          = s_address_oper_data.s_FRQmetter_address.rez_FRQmetter;
	paramIndication[PAR_FRQ].addressStatus         = s_address_oper_data.s_FRQmetter_address.status_FRQmetter;
}


bool menuWorkHight(uint8_t *str, uint16_t status, uint16_t value, uint8_t cnt){
	if( VALUE_STATUS_ERROR <= status){
		sprintf((char*)str, "%s%s", paramIndication[cnt].sumbol, (char*)errorInf);
		return true;
	}

	switch(cnt){
	case PAR_TEMPERATURE:
		switch(status){
		case VALUE_STATUS_OK:
			TEMPERATURE_NORMAL_NOERROR_HIGH;
			return true;
		}
		break;
	case PAR_PRESSURE_PA:
		switch(status){
		case VALUE_STATUS_OK:
			PRESSURE_PA_NORMAL_NOERROR_HIGH;
			return true;
		}
		break;
	case PAR_PRESSURE_MM:
		switch(status){
		case VALUE_STATUS_OK:
			PRESSURE_MM_NORMAL_NOERROR_HIGH;
			return true;
		}
		break;
	case PAR_HUMIDITY:
		switch(status){
		case VALUE_STATUS_OK:
			HUMIDITY_NORMAL_NOERROR_HIGH;
			return true;
		}
		break;
	case PAR_DATE_0:
	case PAR_DATE_1:
	case PAR_DATE_2:
	case PAR_DATE_3:
		switch(status){
		case VALUE_STATUS_OK:
			DATE_NORMAL_NOERROR_HIGH;
			return true;
		case VALUE_STATUS_ALLARM:
			DATE_NORMAL_ALLARM_HIGH;
			return true;
		}
		break;
	case PAR_TIME_0:
	case PAR_TIME_1:
	case PAR_TIME_2:
	case PAR_TIME_3:
		switch(status){
		case VALUE_STATUS_OK:
			TIME_NORMAL_NOERROR_HIGH;
			return true;
		case VALUE_STATUS_ALLARM:
			TIME_NORMAL_ALLARM_HIGH;
			return true;
		}
		break;
	case PAR_FRQ:
		switch(status){
	    case VALUE_STATUS_OK:
	    	FRQ_NORMAL_NOERROR_HIGH;
	    	return true;
	    case VALUE_STATUS_ALLARM:
	    	FRQ_NORMAL_ALLARM_HIGH;
	    	return true;
	    }
		break;
	}
	return false;
}


bool menuWorkLow(uint8_t *str, uint16_t status, uint16_t value, uint8_t cnt) {
	processing_mem_map_read_s_proces_object_modbus(&value, 1, paramIndication[cnt].addressValue);

	switch (cnt) {

	case PAR_DATE_0:
	case PAR_DATE_1:
	case PAR_DATE_2:
	case PAR_DATE_3:
		switch (status) {
		case VALUE_STATUS_OK:
			DATE_NORMAL_NOERROR_LOW;
			return true;
		case VALUE_STATUS_ALLARM:
			DATE_NORMAL_ALLARM_LOW;
			return true;
		}
		break;

	case PAR_TIME_0:
	case PAR_TIME_1:
	case PAR_TIME_2:
	case PAR_TIME_3:
		switch (status) {
		case VALUE_STATUS_OK:
			TIME_NORMAL_NOERROR_LOW;
			return true;
		case VALUE_STATUS_ALLARM:
			TIME_NORMAL_ALLARM_LOW;
			return true;
		}
		break;
	};
    return false;
}


bool menuAdjLowFocus(uint8_t *str, uint16_t status, uint16_t value, uint8_t cnt){
	if( VALUE_STATUS_ERROR == status){
		sprintf((char*)str, " %s", (char*)errorInf);
		return true;
	}

	switch(cnt){
	case PAR_TEMPERATURE:
		switch(status){
		case VALUE_STATUS_OK:
			TEMPERATURE_ADJUSTMENT_INFOCUS_NOERROR_LOW;
			return true;
		}
		break;
	case PAR_PRESSURE_PA:
		switch(status){
		case VALUE_STATUS_OK:
			PRESSURE_PA_ADJUSTMENT_INFOCUS_NOERROR_LOW;
			return true;
		}
		break;
	case PAR_PRESSURE_MM:
		switch(status){
		case VALUE_STATUS_OK:
			PRESSURE_MM_ADJUSTMENT_INFOCUS_NOERROR_LOW;
			return true;
		}
		break;
	case PAR_HUMIDITY:
		switch(status){
		case VALUE_STATUS_OK:
			HUMIDITY_ADJUSTMENT_INFOCUS_NOERROR_LOW;
			return true;
		}
		break;
	case PAR_DATE_0:
	case PAR_DATE_1:
	case PAR_DATE_2:
	case PAR_DATE_3:
		switch(status){
		case VALUE_STATUS_OK:
			DATE_ADJUSTMENT_INFOCUS_NOERROR_LOW;
			return true;
		case VALUE_STATUS_ALLARM:
			DATE_ADJUSTMENT_INFOCUS_NOERROR_LOW;
			return true;
		}
		break;
	case PAR_TIME_0:
	case PAR_TIME_1:
	case PAR_TIME_2:
	case PAR_TIME_3:
		switch(status){
		case VALUE_STATUS_OK:
			TIME_ADJUSTMENT_INFOCUS_NOERROR_LOW;
			return true;
		case VALUE_STATUS_ALLARM:
			TIME_ADJUSTMENT_INFOCUS_NOERROR_LOW;
			return true;
		}
		break;
	case PAR_FRQ:
		switch(status){
	    case VALUE_STATUS_OK:
	    	FRQ_ADJUSTMENT_INFOCUS_NOERROR_LOW;
	    	return true;
	    case VALUE_STATUS_ALLARM:
	    	FRQ_ADJUSTMENT_INFOCUS_NOERROR_LOW;
	    	return true;
	    }
		break;
	}
	return false;
}


bool menuTestHight(uint8_t *str, uint16_t status, uint16_t value, uint8_t cnt){
	memcpy(str, testStr, sizeof(testStr));
	return true;
}

bool updateLCD(uint8_t *str, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus, uint8_t numberValue ){
    uint16_t status;
    uint16_t value;
    processing_mem_map_read_s_proces_object_modbus(&status, 1,
    				paramIndication[numberValue].addressStatus);
    processing_mem_map_read_s_proces_object_modbus(&value, 1,
    				paramIndication[numberValue].addressValue);

    if(status >= VALUE_STATUS_MAX_VAL)
    {
    	status = VALUE_STATUS_ERROR;
    }
	switch (displayMenu) {
	case DISPLAY_MENU_WORK:
		switch(blinkState){
		case BLINK_STATE_HIGHT:
			return menuWorkHight(str, status, value, numberValue);
		case BLINK_STATE_LOW:
			return menuWorkLow(str, status, value, numberValue);
		}
		break;
	case DISPLAY_MENU_UDJUSTMENT:
		switch(blinkState){
		case BLINK_STATE_HIGHT:
			if(focus){
				return menuWorkHight(str, status, value, numberValue);
			}
			else{
				return menuWorkHight(str, status, value, numberValue);
			}
		case BLINK_STATE_LOW:
		    if(focus){//
		    	return menuAdjLowFocus(str, status, value, numberValue);
		    }
			else{
			    return menuWorkLow(str, status, value, numberValue);
			}
		}
		break;
	case DISPLAY_MENU_TEST:
		switch(blinkState){
		case BLINK_STATE_HIGHT:
			 return menuTestHight(str, status, value, numberValue);
			break;
		case BLINK_STATE_LOW:
			break;
		}
		break;
	}
	return false;
}



