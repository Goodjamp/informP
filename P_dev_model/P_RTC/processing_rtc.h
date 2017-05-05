/*
 * processing_gps.h
 *
 *  Created on: June 21, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_GPS_H_
#define PROCESSING_GPS_H_

#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"

//include FreeRtos header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
// include my header
#include "processing_USART.h"
#include "USART_fifo_operation.h"
#include "strings.h"


#define SIZE_GPRMS        7
#define SIZE_READ         20
//#define SIZE_BUF_READ     SIZE_GPRMS+SIZE_READ-1

#define NUM_DATA_GPRMC    100

#define NUM_CYKL_FIND_START 300

#define PORT_PPS   GPIOA
#define PIN_PPS    GPIO_Pin_8


void initRTC(void);


#endif
