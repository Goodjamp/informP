/**
  ******************************************************************************
  * @file    processing_display.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */

#ifndef PROCESSING_DISPLAY_H_
#define PROCESSING_DISPLAY_H_

#define DISPLAY_SPI  SPI1

#define LD_TIMER     TIM2
#define LD_MIN_us       1
#define LD_LENGT_uS     5

/* @brief
 *
 */
typedef enum{
	SCREEN_1 = 0,
	SCREEN_2 = 0,
	SCREEN_3 = 0,
	SCREEN_4 = 0,
}screenOrder;





#endif // PROCESSING_DISPLAY_H_
