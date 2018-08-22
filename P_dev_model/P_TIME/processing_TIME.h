/*
 * processing_gps.h
 *
 *  Created on: June 21, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_TIME_H_
#define PROCESSING_TIME_H_


#define SECOND_EVENT_BIT      (uint8_t)(1<<0)
#define ALARM_EVENT_BITS      (uint8_t)(1<<1)
#define SERVER_UPDATE_TIME    (uint8_t)(1<<2)

#define gpsUSARTSpeed      9600
#define gpsUSARTStopBits   1
#define gpsUSARTParity     0
#define gpsUSARTWordLengt  8

#define USART_READ_BUFF_SIZE     20
#define USART_READ_BUFF_TIME_MS  30

#define gpsUSARTDef  USART3
#define gpsUSARTNum       2

#define SECONDS_PER_HOUR    3600
#define SECONDS_PER_MINUTES 60

//-----------------DAYLIGHT SWITCH PARAMITERS-------------------
#define DAYLIGHT_SWITCH_HOUR_UTC    3
#define DAYLIGHT_WEEK_DAY           7

#define DAYLIGHT_START_MOUNTH_DAYS  31
#define DAYLIGHT_START_MOUNTH        3

#define DAYLIGHT_STOP_MOUNTH_DAYS  31
#define DAYLIGHT_STOP_MOUNTH       10

#define ERROR_IND_TIME_THRESHOLD   5000

typedef enum{
	SEZON_TIME_NOT_SET,
	SEZON_TIME_WINTER,
	SEZON_TIME_SUMMER
}SEZON_TIME;

typedef enum{
	SYNCRONISATION_SOURCE_GPS    = 0,
	SYNCRONISATION_SOURCE_SERVER = 1,
}SYNCRONISATION_SOURCE;


void initTIME(void);


#endif
