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
#include "processing_mem_map_extern.h"

extern S_address_oper_data s_address_oper_data;

typedef void (*updateFun)(uint8_t*, uint16_t, uint16_t);

const static uint8_t errorInf[] = ERROR_INF;
const static uint8_t testStr[]  = TEST_STR;


struct {
	uint8_t sumbol[2];
	uint16_t addressStatus;
	uint16_t addressValue;
}paramIndication[NUMBER_OF_VALUE] = {
		[PAR_TEMPERATURE] = { .sumbol = { SYMBOL_TEMPERATURE }, },
		[PAR_PRESSURE]    = { .sumbol = { SYMBOL_PRESSURE}, },
		[PAR_HUMIDITY]    = { .sumbol = { SYMBOL_HUMIDITY }, },
		[PAR_DATE]        = { .sumbol = { SYMBOL_DATE }, },
		[PAR_TIME]        = { .sumbol = { SYMBOL_TIME }, },
		[PAR_FRQ]         = { .sumbol = { SYMBOL_FRQ }, }
};


void initValueAddress(void){
	// copy address of all values that can indication
	paramIndication[PAR_TEMPERATURE].addressValue  = s_address_oper_data.s_sensor_address.rezTemperature;
	paramIndication[PAR_TEMPERATURE].addressStatus = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_PRESSURE].addressValue     = s_address_oper_data.s_sensor_address.rezPressure;
	paramIndication[PAR_PRESSURE].addressStatus    = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_HUMIDITY].addressValue     = s_address_oper_data.s_sensor_address.rezHumidity;
	paramIndication[PAR_HUMIDITY].addressStatus    = s_address_oper_data.s_sensor_address.status_sensor;
	paramIndication[PAR_DATE].addressValue         = s_address_oper_data.s_TIME_address.DATE;
	paramIndication[PAR_DATE].addressStatus        = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_TIME].addressValue         = s_address_oper_data.s_TIME_address.TIME;
	paramIndication[PAR_TIME].addressStatus        = s_address_oper_data.s_TIME_address.status_TIME;
	paramIndication[PAR_FRQ].addressValue          = s_address_oper_data.s_FRQmetter_address.rez_FRQmetter;
	paramIndication[PAR_FRQ].addressStatus         = s_address_oper_data.s_FRQmetter_address.status_FRQmetter;
}


bool menuWorkHight(uint8_t *str, uint16_t status, uint16_t value, uint8_t cnt){
	if( VALUE_STATUS_ERROR == status){
		sprintf((char*)str, "%1c%s", (char)paramIndication[cnt].sumbol[0], (char*)errorInf);
		return true;
	}

	switch(cnt){
	case PAR_TEMPERATURE:
		switch(status){
		case VALUE_STATUS_OK:
			sprintf((char*)str, "%1c%5.1f", (char)paramIndication[cnt].sumbol[0], (float)((int16_t)value)/10 );
			return true;
		}
		break;
	case PAR_PRESSURE:
		switch(status){
		case VALUE_STATUS_OK:
			sprintf((char*)str, "%1c%4d", (char)paramIndication[cnt].sumbol[0], value);
			return true;
		}
		break;
	case PAR_HUMIDITY:
		switch(status){
		case VALUE_STATUS_OK:
			sprintf((char*)str, "%1c%5.1f", (char)paramIndication[cnt].sumbol[0], (float)(value)/10 );
			return true;
		}
		break;
	case PAR_DATE:
		switch(status){
		case VALUE_STATUS_OK:
			sprintf((char*)str, "%1c%02d.%02d", (char)paramIndication[cnt].sumbol[0], (uint8_t)(value>>8), (uint8_t)(value));
			return true;
		}
		break;
	case PAR_TIME:
		switch(status){
		case VALUE_STATUS_OK:
			sprintf((char*)str, "%1c%02d.%02d", (char)paramIndication[cnt].sumbol[0], (uint8_t)(value>>8), (uint8_t)(value));
			return true;
		}
		break;
	case PAR_FRQ:
		switch(status){
	    case VALUE_STATUS_OK:
	    	sprintf((char*)str, "%1c%5.2f", (char)paramIndication[cnt].sumbol[0], (float)(value)/1000);
	    	return true;
	    }
		break;
	}
	return false;
}


bool menuWorkLow(uint8_t *str, uint16_t status, uint16_t value, uint8_t cnt) {
	processing_mem_map_read_s_proces_object_modbus(&value, 1, paramIndication[cnt].addressValue);

	switch (cnt) {
	case PAR_FRQ:
		switch (status) {
		case VALUE_STATUS_ALLARM:
			sprintf((char*) str, "%1c    ",
					(char) paramIndication[cnt].sumbol[0]);
			return true;
		}
	case PAR_DATE:
		switch (status) {
		case VALUE_STATUS_OK:
			sprintf((char*) str, "%1c%02d%02d",
					(char) paramIndication[cnt].sumbol[0],
					(uint8_t)(value >> 8), (uint8_t)(value));
			return true;
		}
		break;
	case PAR_TIME:
		switch (status) {
		case VALUE_STATUS_OK:
			sprintf((char*) str, "%1c%02d%02d",
					(char) paramIndication[cnt].sumbol[0],
					(uint8_t)(value >> 8), (uint8_t)(value));
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
			sprintf((char*)str, " %5.1f", (float)((int16_t)value)/10 );
			return true;
		}
		break;
	case PAR_PRESSURE:
		switch(status){
		case VALUE_STATUS_OK:
			sprintf((char*)str, " %4d", value);
			return true;
		}
		break;
	case PAR_HUMIDITY:
		switch(status){
		case VALUE_STATUS_OK:
			sprintf((char*)str, " %5.1f", (float)(value)/10 );
			return true;
		}
		break;
	case PAR_DATE:
		switch(status){
		case VALUE_STATUS_OK:
			sprintf((char*)str, " %02d%02d", (uint8_t)(value>>8), (uint8_t)(value));
			return true;
		}
		break;
	case PAR_TIME:
		switch(status){
		case VALUE_STATUS_OK:
			sprintf((char*)str, " %02d%02d", (uint8_t)(value>>8), (uint8_t)(value));
			return true;
		}
		break;
	case PAR_FRQ:
		switch(status){
	    case VALUE_STATUS_OK:
	    	sprintf((char*)str, " %5.2f", (float)(value)/1000);
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



