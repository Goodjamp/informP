/*
 * processing_gps.h
 *
 *  Created on: June 21, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_TIME_H_
#define PROCESSING_TIME_H_


#define SECOND_EVENT_BIT (uint8_t)(1<<0)
#define ALARM_EVENT_BITS  (uint8_t)(1<<1)

#define gpsUSARTSpeed      9600
#define gpsUSARTStopBits   1
#define gpsUSARTParity     0
#define gpsUSARTWordLengt  8

#define USART_READ_BUFF_SIZE     20
#define USART_READ_BUFF_TIME_MS  30

#define gpsUSARTDef  USART3
#define gpsUSARTNum       2

#define SECONDS_PER_HOUR  3600

void initTIME(void);


#endif
