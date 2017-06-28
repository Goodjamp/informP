/*
 * GPSProcessing.h
 *
 *  Created on:  19 jun 2015
 *  Author:      Gerasimchuk Alexandr
 *  Version:     1.0
 */


 #ifndef GPSPROCESSING_H_
 #define GPSPROCESSING_H_

#include "stdint.h"

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

#define SIZEOF_PARS_BUFF     50
#define SIZEOF_CHECKSUM      2

#define GPRMC_MES "GPRMC"


typedef enum{
	GPS_OK,
	GPS_NOT_VALID,
	GPS_ERROR_MESSAGE,
	GPS_ERROR_TYPE_MES,
	GPS_ERROR_RX_CHEKSUM,
	GPS_ERROR_CHEKSUM,
	GPS_ERROR_SYMBOL,
}GPS_STATUS;

typedef enum{
	GPRMC = 0,
	GPGGA
}GPS_TYPE_MES;

#define ISGPSMES(X)   ((X==GPRMC)|| \
					   (X==GPGGA))
typedef enum{
	Rx_HEAD,
	Rx_PAYLOAD,
	Rx_CHECKSUM
}STATE_GPS;

typedef struct{
	uint8_t typeMes:4;
	uint8_t state:4;
	uint8_t headSize:4;
	GPS_STATUS statusGPS:4;
    uint8_t rxMesSize;
	uint8_t rxCheckSum;
	uint8_t calcCheckSum;
	uint8_t cntChekSum;
    uint8_t parsGPSBuff[SIZEOF_PARS_BUFF];
}processingGPS;

typedef struct{
	processingGPS procesingPars;
	uint8_t year;
	uint8_t mounth;
	uint8_t date;
	uint8_t honour;
	uint8_t minutes;
	uint8_t seconds;
	uint16_t mseconds;
	float   latitude;
	float   longitude;
	uint8_t latitude_side:4;
	uint8_t longitude_side:4;
	float   speed;
}GPRMC_Def;

#define DEVIDE_SYMBOL  ','

/*-------------------------------RMC message definition------------------------------*/
//   eg3. $GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70
//                 0    1    2    3    4     5    6    7      8     9   10 11

#define sateliteRx         'V'
#define sateliteNotRx      'A'

#define RMC_F_TIMESTAMP_1   0
#define RMC_F_VALIDITY      1
#define RMC_F_LATITUDE      2
#define RMC_F_NORTH_SOUTH   3
#define RMC_F_LONGITITUDE   4
#define RMC_F_EAST_WEST     5
#define RMC_F_SPEED         6
#define RMC_F_TRUE_COURSE   7
#define RMC_F_UT_DATE	    8
#define RMC_F_VARIATION     9
#define RMC_F_V_EAST_WEST  10


void parsGPS(void *procGPS_in, uint8_t *symbols, uint8_t length);
GPS_STATUS addGPSPars(GPS_TYPE_MES mesType, void *procGPSIn);

 #endif
