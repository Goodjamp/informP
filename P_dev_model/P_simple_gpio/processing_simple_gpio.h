/*
 * processing_simple_gpio.h
 *
 *  Created on: Jun 8, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_SIMPLE_GPIO_H_
#define PROCESSING_SIMPLE_GPIO_H_
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

// Настройки джамперов
// порты джамперов выбора режима конфигурации (по_умолчанию/пользовательская)
#define PORT_JAMP1         GPIOB

// пины джамперов
#define PIN_JAMP1           GPIO_Pin_0

// Настройки светодиодовв индикации
// порты светодиодов индикации
#define PORT_IND_ERROR         GPIOB

// пины светодиодов индикации
#define PIN_IND_ERROR          GPIO_Pin_5


// ---------- МАКРОСЫ ЧТЕНИЯ ДЖАМПЕРОВ --------------------------------------
#define IS_USER_CONFIG          GPIO_ReadInputDataBit(PORT_JAMP1,PIN_JAMP1)

// ---------- МАКРОСЫ УПРАВЛЕНИЯ ИНДИКАЦИЕЙ --------------------------------------
#define IND_OUT_INVERSE
#ifdef IND_OUT_INVERSE
// если управление инверсное (лог 0 соответствует led on)
#define IND_ERROR_RESET          GPIO_SetBits(PORT_IND_ERROR,PIN_IND_ERROR);
#define IND_ERROR_SET            GPIO_ResetBits(PORT_IND_ERROR,PIN_IND_ERROR);
#else
// если управление нормальное (лог 1 соответствует ВЫКЛ релле)
#define IND_ERROR_RESET          GPIO_ResetBits(PORT_IND_ERROR,PIN_IND_ERROR);
#define IND_ERROR_SET            GPIO_SetBits(PORT_IND_ERROR,PIN_IND_ERROR);
#endif

//-------------Прототипы функций--------------
void processing_simple_gpio(void);
void work_jamper_init(void);
void work_ind_init(void);
void remap_out_pin(void);



#endif
