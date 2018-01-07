/**
  ******************************************************************************
  * @file    debugStuff.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    4-November-2017
  * @brief
  */

#ifndef DEBUGSTUFF_H_
#define DEBUGSTUFF_H_

#include "stm32f10x_gpio.h"

//P4
#define debugPin1  GPIO_Pin_2
//P5
#define debugPin2  GPIO_Pin_3
#define debugPort1 GPIOA
#define debugPort2 GPIOA
#define debugPin_1_Set     GPIO_SetBits(debugPort1, debugPin1)
#define debugPin_1_Clear   GPIO_ResetBits(debugPort1, debugPin1)
#define debugPin_2_Set     GPIO_SetBits(debugPort2, debugPin2)
#define debugPin_2_Clear   GPIO_ResetBits(debugPort2, debugPin2)

void debugPinConfig(void);

#endif
