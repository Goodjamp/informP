/********************************************************************************
 * @file    lcdUpdate.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    13-December-2017
 * @brief
 */
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "lcdUpdate.h"
#include "menuProcessing.h"
#include "processing_display.h"
#include "processing_display_extern.h"
#include "processing_mem_map_extern.h"
#include "processing_FRQmetter_extern.h"
#include "processing_sensor_extern.h"
#include "processing_TIME_extern.h"


#define SYMBOL_TEMPERATURE   "C"
#define SYMBOL_PRESSURE_PA   "Ph"
#define SYMBOL_PRESSURE_MM   "P"
#define SYMBOL_HUMIDITY      "H"
#define SYMBOL_DATE          "D"
#define SYMBOL_TIME          "T"
#define SYMBOL_FRQ           "F"

#define SYMBOL_ALLARM        "~"
#define SYMBOL_RAIN          "^"
#define SYMBOL_BATARY        "%"

#define ERROR_INF            "ErrO"
#define TEST_STR             "@   8888"


extern S_address_oper_data s_address_oper_data;

static void getTemperatureStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getPressurePaStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getPressureMMStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getHumidityStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getDate1Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getTime1Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getDate2Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getTime2Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getDate3Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getTime3Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getDate4Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getTime4Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static void getFrqStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);


typedef void (*UpdateFunStr)(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus);
static const UpdateFunStr updateFunStr[]=
{
    [PAR_TEMPERATURE] = getTemperatureStr,
    [PAR_PRESSURE_PA] = getPressurePaStr,
    [PAR_PRESSURE_MM] = getPressureMMStr,
    [PAR_HUMIDITY]    = getHumidityStr,
    [PAR_DATE_0]      = getDate1Str,
    [PAR_TIME_0]      = getTime1Str,
    [PAR_DATE_1]      = getDate2Str,
    [PAR_TIME_1]      = getTime2Str,
    [PAR_DATE_2]      = getDate3Str,
    [PAR_TIME_2]      = getTime3Str,
    [PAR_DATE_3]      = getDate4Str,
    [PAR_TIME_3]      = getTime4Str,
    [PAR_FRQ]         = getFrqStr
};


static void numToStr(uint8_t *str, int32_t currentValue, uint8_t strS, uint8_t fractionS, uint8_t fillSymboll)
{
#define MULL 10
    int8_t cnt = strS - 1;
    bool signV = true;
    currentValue = ((currentValue >= 0) ? (currentValue) : (signV = false, (-1) * currentValue));
    memset(str, fillSymboll, strS);

    if(fractionS){
        while((fractionS--) && (cnt >= 0)){
            str[cnt--] =  currentValue - (currentValue / MULL) * MULL + '0';
            currentValue /= MULL;
        }
        if(cnt < 0){
            return;
        }
        str[cnt--] = '.';

        if(cnt < 0){
            return;
        }
    }

    do{
        str[cnt--] =  currentValue - (currentValue / MULL) * MULL + '0';
        currentValue /= MULL;
    }while((currentValue) && (cnt >= 0));

    if(!signV && (cnt >= 0)){
        str[cnt--] = '-';
    }
#undef MULL
}


static void getDateTimeStr(uint16_t value, uint8_t *str, bool point)
{
    if(point)
    {
        numToStr(&str[3], value & 0xFF, 2, 0, '0');
        str[2] = '.';
    }
    else {
        numToStr(&str[2], value & 0xFF, 2, 0, '0');
    }
    numToStr(&str[0], value >> 8, 2, 0, '0');
}


static void addSymbols(uint8_t *dest, uint8_t lenDst, const uint8_t *src, uint8_t lenSrc)
{
    uint8_t cntDst = 0;
    uint8_t cntSrc = 0;
    while((dest[cntDst] != ' ') && (cntDst < lenDst))
    {
        cntDst++;
    }
    while((cntDst < lenDst) && (cntSrc < lenSrc))
    {
        dest[cntDst++] = src[cntSrc++];
    }
}


/*In this string we wont to show temperature and battery status*/
static void getTemperatureStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	uint16_t value;
	uint16_t status;

	processing_mem_map_read_s_proces_object_modbus(&status, 1, s_address_oper_data.s_sensor_address.status_sensor);

	//set value
	if(status & ( (1 << SENSOR_STATUS_ERROR_LOCAL)
			    | (1 << SENSOR_STATUS_ERROR_RECEIVER)
			    | (1 << SENSOR_STATUS_ERROR_REM_RX_TIMEOUT)
			    | (1 << SENSOR_STATUS_ERROR_REM_SENSOR)))
	{
		memcpy(strValue, ERROR_INF, sizeof(ERROR_INF));
	}
	else
	{
		processing_mem_map_read_s_proces_object_modbus(&value,  1, s_address_oper_data.s_sensor_address.rezTemperature);
		numToStr(strValue, value, 5, 1, ' ');
	}
	// set symbol
	addSymbols(strSymbol, 4, (const uint8_t*)SYMBOL_TEMPERATURE, (sizeof(SYMBOL_TEMPERATURE) - 1));
	if(status & (1 << SENSOR_STATUS_ERROR_REM_BATARY))                // set btray error
	{
		addSymbols(strSymbol, 4, (const uint8_t*)SYMBOL_BATARY, (sizeof(SYMBOL_BATARY) - 1));
	}

	switch (displayMenu) {
	case DISPLAY_MENU_WORK:
        break;
	case DISPLAY_MENU_UDJUSTMENT:
		if(blinkState == BLINK_STATE_LOW && focus)
		{
            memset(strSymbol,' ', 4);
		}
		break;
	default: break;
	}
}


/*In this string we wont to show pressure in Giga Pascal only*/
static void getPressurePaStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	uint16_t value;
	uint16_t status;

	processing_mem_map_read_s_proces_object_modbus(&status, 1, s_address_oper_data.s_sensor_address.status_sensor);

	//set value
	if(status & ( (1 << SENSOR_STATUS_ERROR_LOCAL)                     // set value
			    | (1 << SENSOR_STATUS_ERROR_RECEIVER)
			    | (1 << SENSOR_STATUS_ERROR_REM_RX_TIMEOUT)
			    | (1 << SENSOR_STATUS_ERROR_REM_SENSOR)))
	{
		memcpy(strValue, ERROR_INF, sizeof(ERROR_INF));
	}
	else
	{
		processing_mem_map_read_s_proces_object_modbus(&value,  1, s_address_oper_data.s_sensor_address.rezPressure_GPasc);
		numToStr(strValue, value, 4, 0, ' ');
	}
	// set symbol
	addSymbols(strSymbol, 4, (const uint8_t*)SYMBOL_PRESSURE_PA, (sizeof(SYMBOL_PRESSURE_PA) - 1));
	switch (displayMenu) {
	case DISPLAY_MENU_WORK:
        break;
	case DISPLAY_MENU_UDJUSTMENT:
		if(blinkState == BLINK_STATE_LOW && focus)
		{
            memset(strSymbol,' ', 4);
		}
		break;
	default: break;
	}
}

/*In this string we wont to show pressure in mm Hg only*/
static void getPressureMMStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	uint16_t value;
	uint16_t status;

	processing_mem_map_read_s_proces_object_modbus(&status, 1, s_address_oper_data.s_sensor_address.status_sensor);

	//set value
	if(status & ( (1 << SENSOR_STATUS_ERROR_LOCAL)                     // set value
			    | (1 << SENSOR_STATUS_ERROR_RECEIVER)
			    | (1 << SENSOR_STATUS_ERROR_REM_RX_TIMEOUT)
			    | (1 << SENSOR_STATUS_ERROR_REM_SENSOR)))
	{
		memcpy(strValue, ERROR_INF, sizeof(ERROR_INF));
	}
	else
	{
		processing_mem_map_read_s_proces_object_modbus(&value,  1, s_address_oper_data.s_sensor_address.rezPressure_mmHg);
		numToStr(strValue, value, 4, 0, ' ');
	}
	// set symbol
	addSymbols(strSymbol, 4, (const uint8_t*)SYMBOL_PRESSURE_MM, (sizeof(SYMBOL_PRESSURE_MM) - 1));
	switch (displayMenu) {
	case DISPLAY_MENU_WORK:
        break;
	case DISPLAY_MENU_UDJUSTMENT:
		if(blinkState == BLINK_STATE_LOW && focus)
		{
            memset(strSymbol,' ', 4);
		}
		break;
	default: break;
	}
}


/*In this string we wont to show Humidity*/
static void getHumidityStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	uint16_t value;
	uint16_t valueRain;
	uint16_t status;

	processing_mem_map_read_s_proces_object_modbus(&status, 1, s_address_oper_data.s_sensor_address.status_sensor);

	//set value
	if(status & ( (1 << SENSOR_STATUS_ERROR_LOCAL)                     // set value
			    | (1 << SENSOR_STATUS_ERROR_RECEIVER)
			    | (1 << SENSOR_STATUS_ERROR_REM_RX_TIMEOUT)
			    | (1 << SENSOR_STATUS_ERROR_REM_SENSOR)))
	{
		memcpy(strValue, ERROR_INF, sizeof(ERROR_INF));
	}
	else
	{

		processing_mem_map_read_s_proces_object_modbus(&value,  1, s_address_oper_data.s_sensor_address.rezHumidity);
		numToStr(strValue, value, 5, 1, ' ');
	}
	// set symbol
	addSymbols(strSymbol, 4, (const uint8_t*)SYMBOL_HUMIDITY, (sizeof(SYMBOL_HUMIDITY) - 1));
	processing_mem_map_read_s_proces_object_modbus(&valueRain,  1, s_address_oper_data.s_sensor_address.rezRain);
	if(valueRain)
	{
		addSymbols(strSymbol, 4, (const uint8_t*)SYMBOL_RAIN, (sizeof(SYMBOL_RAIN) - 1));
	}
	switch (displayMenu) {
	case DISPLAY_MENU_WORK:
        break;
	case DISPLAY_MENU_UDJUSTMENT:
		if(blinkState == BLINK_STATE_LOW && focus)
		{
            memset(strSymbol,' ', 4);
		}
		break;
	default: break;
	}
}


/*In this string we wont to show Humidity*/
static void getFrqStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	uint16_t value;
	uint16_t status;

	processing_mem_map_read_s_proces_object_modbus(&status, 1, s_address_oper_data.s_FRQmetter_address.status_FRQmetter);

	//set value
	if(status == FRQ_STATUS_ERROR)
	{
		memcpy(strValue, ERROR_INF, sizeof(ERROR_INF));
	}
	else
	{
		processing_mem_map_read_s_proces_object_modbus(&value,  1, s_address_oper_data.s_FRQmetter_address.rez_FRQmetter);
		value /= 10;
		numToStr(strValue, value, 5, 2, ' ');
	}
	// set symbol. We should show alarm status also if need
	addSymbols(strSymbol, 4, (const uint8_t*)SYMBOL_FRQ, (sizeof(SYMBOL_FRQ) - 1));
	switch (displayMenu) {
	case DISPLAY_MENU_WORK:
		if(status == FRQ_STATUS_ALLARM)
		{
			addSymbols(strSymbol, 4, (const uint8_t*)SYMBOL_ALLARM, (sizeof(SYMBOL_ALLARM) - 1));
		}
        break;
	case DISPLAY_MENU_UDJUSTMENT:
		if(blinkState == BLINK_STATE_LOW && focus)
		{
			if(focus)
			{
                memset(strSymbol,' ', 4);
			}
		}
		break;
	default: break;
	}
}


static void getDateStr(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus, uint8_t clockPos, bool isDate)
{
	uint16_t value;
	uint16_t status;

	processing_mem_map_read_s_proces_object_modbus(&status, 1, s_address_oper_data.s_TIME_address.status_TIME);
	//set value
	processing_mem_map_read_s_proces_object_modbus(&value,  1,
			(isDate) ? (s_address_oper_data.s_TIME_address.clock[clockPos].DATE): (s_address_oper_data.s_TIME_address.clock[clockPos].TIME));
	// set symbol. We should show alarm status also
	addSymbols(strSymbol,
			   4,
			   (isDate) ? ((const uint8_t*)SYMBOL_DATE) : ((const uint8_t*)SYMBOL_TIME),
			   (sizeof(SYMBOL_DATE) - 1));
	if(status == FRQ_STATUS_ALLARM)
	{
		addSymbols(strSymbol, 4, (const uint8_t*)SYMBOL_ALLARM, (sizeof(SYMBOL_ALLARM) - 1));
	}
	switch (displayMenu) {
	case DISPLAY_MENU_WORK:


		if((blinkState == BLINK_STATE_HIGHT))
		{
			//set value
			getDateTimeStr(value, strValue, true);
		}
		else // blink state low
		{
			//set value
			getDateTimeStr(value, strValue, false);
		}
        break;
	case DISPLAY_MENU_UDJUSTMENT:
		if((blinkState == BLINK_STATE_HIGHT))
		{
			//set value
			getDateTimeStr(value, strValue, true);
		}
		else // blink state low
		{
			getDateTimeStr(value, strValue, false);
			if(focus)
			{
                memset(strSymbol, ' ', 4);
			}
		}
        break;
	default: break;
	}
}


static void getDate1Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	getDateStr(strSymbol, strValue, blinkState, displayMenu, focus, 0, true );
}


static void getTime1Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	getDateStr(strSymbol, strValue, blinkState, displayMenu, focus, 0, false);
}


static void getDate2Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	getDateStr(strSymbol, strValue, blinkState, displayMenu, focus, 1, true);
}


static void getTime2Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	getDateStr(strSymbol, strValue, blinkState, displayMenu, focus, 1, false);
}


static void getDate3Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	getDateStr(strSymbol, strValue, blinkState, displayMenu, focus, 2, true);
}


static void getTime3Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	getDateStr(strSymbol, strValue, blinkState, displayMenu, focus, 2, false);
}


static void getDate4Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	getDateStr(strSymbol, strValue, blinkState, displayMenu, focus, 3, true);
}


static void getTime4Str(uint8_t *strSymbol, uint8_t *strValue, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus)
{
	getDateStr(strSymbol, strValue, blinkState, displayMenu, focus, 3, false);
}


static void menuTestHight(uint8_t *str){
	memcpy(str, TEST_STR, strlen(TEST_STR));
}


void updateLCD(uint8_t *str, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus, uint8_t numberValue ){
	switch (displayMenu) {
		case DISPLAY_MENU_WORK:
		case DISPLAY_MENU_UDJUSTMENT:
			memset(str, ' ', 4);
			updateFunStr[numberValue](str, &str[4], blinkState, displayMenu, focus);
			break;
		case DISPLAY_MENU_TEST:
			menuTestHight(str);
			break;
		}
}

