/* File:   processing_FRQmetter.h
 * Author: Gerasimchuk A.
 * Date:  03.03.2017
 *
 */

#ifndef PROCESSING_FEQMETTER_H_
#define PROCESSING_FEQMETTER_H_

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "processing_mem_map.h"
#include "processing_modbus.h"
#include "init_system.h"
#include "processing_FRQmetter_extern.h"
#include "processing_reset_control.h"


// frequency threshold F*1000
#define FRQ_MAX 55000
#define FRQ_MIN 45000


// GPIO timer description
#define FREQ_TIMER       TIM1

// TIM max value CNT register
#define TIM_MAX_CNT     0xFFFF

// GPIO pin TIM in
#define FREQ_INPUT_PORT  GPIOA
#define FREQ_INPUT_PIN   GPIO_Pin_8

//Frequency measurement accuracy in *100 uHz
#define FRQ_ACCURACY     2

#define GET_TIM_F(X)     (uint32_t)(X.PCLK2_Frequency)

//Function prototype
void t_processing_FRQ(void);


#endif // PROCESSING_TC_SINAL
