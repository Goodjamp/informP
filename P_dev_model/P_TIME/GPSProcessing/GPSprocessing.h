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

#define ENABLE_GPS_DEBUG_MES

#ifdef ENABLE_GPS_DEBUG_MESx
#define GPS_PRINTF(...) printf(__VA_ARGS__)
#else
#define GPS_PRINTF(...)
#endif

#define NUM_DATA_GPRMC    100

#define NUM_CYKL_FIND_START 300

#define PORT_PPS   GPIOA
#define PIN_PPS    GPIO_Pin_8

#define SIZEOF_PARS_BUFF     70
#define SIZEOF_CHECKSUM      2

#define FIELD_DEVIDER       ','

#define GPRMC_MES "GPRMC"



typedef enum{
    GPS_STATUS_COMPLETE,
    GPS_STATUS_RX_PROCESS,
    GPS_STATUS_ERROR_TYPE_MES,
    GPS_STATUS_ERROR_CRC,
    GPS_STATUS_ERROR_CONV_TO_NUM,
    GPS_STATUS_ERROR_TAKE_FIELD,
    GPS_STATUS_ERROR_GENERAL
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


typedef enum{
    TYPE_NUM_FLOAT,
    TYPE_NUM_INT,
    TYPE_NUM_DATA,
    TYPE_NUM_ARRAY
}TYPE_NUM;


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

#define DEVIDE_SYMBOL  ','

/*-------------------------------RMC message definition------------------------------*/
//   eg3. $GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70
//                 0    1    2    3    4     5    6    7      8     9   10 11

#define sateliteRx         'A'
#define sateliteNotRx      'V'

#define RMC_F_TIME              0
#define RMC_F_TIME_HOURS        0
#define RMC_F_TIME_HOURS_SIZE   2
#define RMC_F_TIME_MINUTES      2
#define RMC_F_TIME_MINUTES_SIZE 2
#define RMC_F_TIME_SECONDS      4
#define RMC_F_TIME_SECONDS_SIZE 2
#define RMC_F_VALIDITY          1
#define RMC_F_LATITUDE          2
#define RMC_F_NORTH_SOUTH       3
#define RMC_F_NORTH_SOUTH_SIZE  2
#define RMC_F_LONGITITUDE       4
#define RMC_F_EAST_WEST         5
#define RMC_F_EAST_WEST_SIZE    2
#define RMC_F_SPEED             6
#define RMC_F_TRUE_COURSE       7
#define RMC_F_DATE              8
#define RMC_F_DATE_DAY          0
#define RMC_F_DATE_DAY_SIZE     2
#define RMC_F_DATE_MONTH        2
#define RMC_F_DATE_MONTH_SIZE   2
#define RMC_F_DATE_YER          4
#define RMC_F_DATE_YER_SIZE     2
#define RMC_F_VARIATION         9
#define RMC_F_V_EAST_WEST       10

typedef struct{
    processingGPS procesingPars;
    uint8_t  year;
    uint8_t  mounth;
    uint8_t  date;
    uint8_t  honour;
    uint8_t  minutes;
    float    seconds;
    uint16_t mseconds;
    float    latitude;
    float    longitude;
    uint8_t  latitude_side;
    uint8_t  longitude_side;
    float    speed;
}GPRMC_Def;

// Standallone functin for every messages
// API function and some message description

GPS_STATUS parsGPS(void *procGPS_in, uint8_t *symbols, uint8_t length);
GPS_STATUS addGPSPars(GPS_TYPE_MES mesType, void *procGPSIn);
GPS_STATUS rxGPSStatus(void *procGPS_in);

 #endif
