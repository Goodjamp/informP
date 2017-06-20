/*
 * GPSProcessing.h
 *
 *  Created on:  19 jun 2015
 *  Author:      Gerasimchuk Alexandr
 *  Version:     1.0
 */


 #ifndef GPSPROCESSING_H_
 #define GPSPROCESSING_H_

#define GPRMC_HEADR    "$GPRMC,"

#define SIZE_GPRMS        7
#define SIZE_READ         20
//#define SIZE_BUF_READ     SIZE_GPRMS+SIZE_READ-1

#define NUM_DATA_GPRMC    100

#define NUM_CYKL_FIND_START 300

#define PORT_PPS   GPIOA
#define PIN_PPS    GPIO_Pin_8

#define gpsUSARTDef    		 USART1
#define gpsUSARTSpeed  		 9600
#define gpsUSARTParity       USART_Parity_No
#define gpsUSARTWordLengt    USART_WordLength_8b
#define gpsUSARTStopBits     USART_StopBits_1
#define gpsUSARTPotokControl 0


 #endif
