/********************************************************************************
 * @file    lcdUpdate.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date     15-November-2017
 * @brief
 */

#ifndef LCD_UPDATE_H_
#define LCD_UPDATE_H_

#include "processing_display.h"
#include "menuProcessing.h"

#define SYMBOL_TEMPERATURE   "C"
#define SYMBOL_PRESSURE_PA   "Ph"
#define SYMBOL_PRESSURE_MM   "P"
#define SYMBOL_HUMIDITY      "H"
#define SYMBOL_DATE          "D"
#define SYMBOL_DATE_ALLARM   "D~"
#define SYMBOL_TIME          "T"
#define SYMBOL_TIME_ALLARM   "T~"
#define SYMBOL_FRQ           "F"
#define SYMBOL_FRQ_ALLARM    "F~"
#define ERROR_INF            "ErrO"
//#define ALLARM_INF           '~'
#define TEST_STR             "@8888"

typedef enum{
	VALUE_STATUS_OK       = 0,
	VALUE_STATUS_ALLARM   = 1,
	VALUE_STATUS_ERROR    = 2,
	VALUE_STATUS_MAX_VAL  = 3
}VALUE_STATUS;

/**************************************************************************/
/*               DESCRIPTION OF STRING FOR EVER VALUE                     */
/**************************************************************************/
/*
 * LIST OF STATE:
 * - NORMAL  NO_ERROR  HIGHT                #define VALUE_NORMAL_NOERROR_HIGH
 * - NORMAL  NO_ERROR  LOW                  #define VALUE_NORMAL_NOERROR_LOW
 * - NORMAL  ALLARM    HIGHT                #define VALUE_NORMAL_ALLARM_HIGH
 * - NORMAL  ALLARM    LOW                  #define VALUE_NORMAL_ALLARM_LOW
 * - NORMAL  ERROR     HIGHT                #define VALUE_NORMAL_ERROR_HIGH
 * - NORMAL  ERROR     LOW                  #define VALUE_NORMAL_ERROR_LOW
 *

 * - ADJUSTMENT  NO_FOCUS  NO_ERROR  HIGHT  #define VALUE_ADJUSTMENT_NOFOCUS_NOERROR_HIGH
 * - ADJUSTMENT  NO_FOCUS  NO_ERROR  LOW    #define VALUE_ADJUSTMENT_NOFOCUS_NOERROR_LOW
 * - ADJUSTMENT  NO_FOCUS  ALLARM    HIGHT  #define VALUE_ADJUSTMENT_NOFOCUS_ALLARM_HIGH
 * - ADJUSTMENT  NO_FOCUS  ALLARM    LOW    #define VALUE_ADJUSTMENT_NOFOCUS_ALLARM_LOW
 * - ADJUSTMENT  NO_FOCUS  ERROR     HIGHT  #define VALUE_ADJUSTMENT_NOFOCUS_ERROR_HIGHT
 * - ADJUSTMENT  NO_FOCUS  ERROR     LOW    #define VALUE_ADJUSTMENT_NOFOCUS_ERROR_LOW
 *
 * - ADJUSTMENT  IN_FOCUS  NO_ERROR  HIGHT  #define VALUE_ADJUSTMENT_INFOCUS_NOERROR_HIGH
 * - ADJUSTMENT  IN_FOCUS  NO_ERROR  LOW    #define VALUE_ADJUSTMENT_INFOCUS_NOERROR_LOW
 * - ADJUSTMENT  IN_FOCUS  ALLARM    HIGHT  #define VALUE_ADJUSTMENT_INFOCUS_ALLARM_HIGH
 * - ADJUSTMENT  IN_FOCUS  ALLARM    LOW    #define VALUE_ADJUSTMENT_INFOCUS_ALLARM_LOW
 * - ADJUSTMENT  IN_FOCUS  ERROR     HIGHT  #define VALUE_ADJUSTMENT_INFOCUS_ERROR_HIGHT
 * - ADJUSTMENT  IN_FOCUS  ERROR     LOW    #define VALUE_ADJUSTMENT_INFOCUS_ERROR_LOW
 * */

/****TEMPERATURE*******/
#define TEMPERATURE_NORMAL_NOERROR_HIGH            sprintf((char*)str, "%s%5.1f", paramIndication[cnt].sumbol, (float)((int16_t)value)/10 );
#define TEMPERATURE_ADJUSTMENT_INFOCUS_NOERROR_LOW sprintf((char*)str, " %5.1f",                               (float)((int16_t)value)/10 );
/****PRESSURE PA**********/
#define PRESSURE_PA_NORMAL_NOERROR_HIGH            sprintf((char*)str, "%s%4d", paramIndication[cnt].sumbol, value);
#define PRESSURE_PA_ADJUSTMENT_INFOCUS_NOERROR_LOW sprintf((char*)str, " %4d",                               value);
/****PRESSURE MM**********/
#define PRESSURE_MM_NORMAL_NOERROR_HIGH            sprintf((char*)str, "%s%4d", paramIndication[cnt].sumbol, value);
#define PRESSURE_MM_ADJUSTMENT_INFOCUS_NOERROR_LOW sprintf((char*)str, " %4d",                               value);
/****HUMIDITY**********/
#define HUMIDITY_NORMAL_NOERROR_HIGH               sprintf((char*)str, "%s%5.1f", paramIndication[cnt].sumbol, (float)(value)/10 );
#define HUMIDITY_ADJUSTMENT_INFOCUS_NOERROR_LOW    sprintf((char*)str, " %5.1f",                               (float)(value)/10 );
/****DATE**********/
#define DATE_NORMAL_NOERROR_HIGH                   sprintf((char*)str, "%s%02d.%02d", paramIndication[cnt].sumbol, (uint8_t)(value), (uint8_t)(value >> 8));
#define DATE_NORMAL_NOERROR_LOW                    sprintf((char*)str, "%s%02d%02d",  paramIndication[cnt].sumbol, (uint8_t)(value), (uint8_t)(value >> 8));
#define DATE_NORMAL_ALLARM_HIGH                    sprintf((char*)str, "%s%02d.%02d", SYMBOL_DATE_ALLARM,          (uint8_t)(value), (uint8_t)(value >> 8));
#define DATE_NORMAL_ALLARM_LOW                     sprintf((char*)str, "%s%02d%02d",  SYMBOL_DATE_ALLARM,          (uint8_t)(value), (uint8_t)(value >> 8));
#define DATE_ADJUSTMENT_INFOCUS_NOERROR_LOW        sprintf((char*)str, " %02d%02d",                                (uint8_t)(value), (uint8_t)(value >> 8));
/****TIME**********/
#define TIME_NORMAL_NOERROR_HIGH                   sprintf((char*)str, "%s%02d.%02d", paramIndication[cnt].sumbol, (uint8_t)(value>>8), (uint8_t)(value));
#define TIME_NORMAL_NOERROR_LOW                    sprintf((char*)str, "%s%02d%02d",  paramIndication[cnt].sumbol, (uint8_t)(value>>8), (uint8_t)(value));
#define TIME_NORMAL_ALLARM_HIGH                    sprintf((char*)str, "%s%02d.%02d", SYMBOL_TIME_ALLARM,          (uint8_t)(value>>8), (uint8_t)(value));
#define TIME_NORMAL_ALLARM_LOW                     sprintf((char*)str, "%s%02d%02d",  SYMBOL_TIME_ALLARM,          (uint8_t)(value>>8), (uint8_t)(value));
#define TIME_ADJUSTMENT_INFOCUS_NOERROR_LOW        sprintf((char*)str, " %02d%02d",                                (uint8_t)(value>>8), (uint8_t)(value));
/****FRQ*******/
#define FRQ_NORMAL_NOERROR_HIGH                    sprintf((char*)str, "%s%5.2f",    paramIndication[cnt].sumbol, (float)(value)/1000);
#define FRQ_NORMAL_ALLARM_HIGH                     sprintf((char*)str, "%s%5.2f",    SYMBOL_FRQ_ALLARM,           (float)(value)/1000);
#define FRQ_ADJUSTMENT_INFOCUS_NOERROR_LOW         sprintf((char*)str, " %5.2f",                                  (float)(value)/1000);


bool updateLCD(uint8_t *str, BLINK_STATE blinkState, DISPLAY_MENU displayMenu, bool focus, uint8_t numberValue );
void initValueAddress(void);

#endif
