/*
 * processing_gps.h
 *
 *  Created on: June 21, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_TIME_H_
#define PROCESSING_TIME_H_


#define gpsUSARTSpeed      9600
#define gpsUSARTStopBits   1
#define gpsUSARTParity     0
#define gpsUSARTWordLengt  8

#define USART_READ_BUFF_SIZE     20
#define USART_READ_BUFF_TIME_MS  30

#define gpsUSARTDef  USART3
#define gpsUSARTNum       2

void initTIME(void);


#endif
