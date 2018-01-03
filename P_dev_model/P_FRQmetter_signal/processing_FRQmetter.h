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


// frequency threshold F*1000
#define FRQ_MAX 51000
#define FRQ_MIN 49000


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

#define GET_TIM_F(X)     (uint32_t)(X.PCLK2_Frequency)

//Function prototype
void t_processing_FRQ(void);


#endif // PROCESSING_TC_SINAL
