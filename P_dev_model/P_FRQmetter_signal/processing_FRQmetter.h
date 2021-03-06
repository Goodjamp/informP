/**
  ******************************************************************************
  * @file    processing_FRQmetter.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    3-Mach-2017
  * @brief
  */


#ifndef PROCESSING_FEQMETTER_H_
#define PROCESSING_FEQMETTER_H_

#include "stdint.h"
#include "processing_FRQmetter_extern.h"

// frequency threshold F*1000
#define FRQ_MAX     51000
#define FRQ_MIN     49000
#define FRQ_MAX_MES 60000
#define FRQ_MIN_MES 40000


// GPIO timer description
#define FREQ_TIMER       TIM1

// TIM max value CNT register
#define TIM_MAX_CNT     0xFFFF

// GPIO pin TIM in
#define FREQ_INPUT_PORT  GPIOA
#define FREQ_INPUT_PIN   GPIO_Pin_8

//Frequency measurement accuracy in *100 uHz
#define FRQ_ACCURACY     2
//200 uHz = 0.0002 Hz

// timout before ERROR STATUS
#define ERROR_TIMEOUT_MS    1000

//Average interval
#define NUMBER_OF_RESULTS       32
#define MAX_INDEX_OF_RESULTS    31
#define OUT_OF_RANGE_THRESHOLD  5


#define GET_TIM_F(X)     (uint32_t)(X.PCLK2_Frequency)

// union for cast FRQ_STATUS to uint16_t
typedef union{
	uint16_t   statusU16;
	FRQ_STATUS statusUser;
}castStatus;

//Function prototype
void t_processing_FRQ(void);


#endif // PROCESSING_TC_SINAL
