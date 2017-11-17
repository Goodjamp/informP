/********************************************************************************
 * @file    lcdHWInterface.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-November-2016
 * @brief
 */


#ifndef STMMAXSHARDWAREINIT_H_
#define STMMAXSHARDWAREINIT_H_
#include "stm32f10x_spi.h"

#include "max7219.h"

#define LD_TIMER     TIM2
#define LD_MIN_us       1
#define LD_LENGT_uS     5

#define DISPLAY_SPI  SPI2

// ----------SPI1 pin/port definition---------
#define SPI1_GPIO_SCK_ENABLE  RCC_APB2Periph_GPIOA
#define SPI1_GPIO_SCK         GPIOA
#define SPI1_GPIO_SCK_PIN     GPIO_Pin_5
#define SPI1_GPIO_MOSI_ENABLE RCC_APB2Periph_GPIOA
#define SPI1_GPIO_MOSI        GPIOA
#define SPI1_GPIO_MOSI_PIN    GPIO_Pin_7
#define SPI1_GPIO_MISO_ENABLE RCC_APB2Periph_GPIOA
#define SPI1_GPIO_MISO        GPIOA
#define SPI1_GPIO_MISO_PIN    GPIO_Pin_6

// ----------SPI2 pin/port definition---------
#define SPI2_GPIO_SCK_ENABLE  RCC_APB2Periph_GPIOB
#define SPI2_GPIO_SCK         GPIOB
#define SPI2_GPIO_SCK_PIN     GPIO_Pin_13
#define SPI2_GPIO_MOSI_ENABLE RCC_APB2Periph_GPIOB
#define SPI2_GPIO_MOSI        GPIOB
#define SPI2_GPIO_MOSI_PIN    GPIO_Pin_15
#define SPI2_GPIO_MISO_ENABLE RCC_APB2Periph_GPIOB
#define SPI2_GPIO_MISO        GPIOB
#define SPI2_GPIO_MISO_PIN    GPIO_Pin_14

// ----------LD pin/port definition---------
#define PORT_LD_1  GPIOB
#define PORT_LD_2  GPIOB
#define PORT_LD_3  GPIOB
#define PORT_LD_4  GPIOB

#define PIN_LD_1   GPIO_Pin_12
#define PIN_LD_2   GPIO_Pin_8
#define PIN_LD_3   GPIO_Pin_14
#define PIN_LD_4   GPIO_Pin_9



typedef struct{
	GPIO_TypeDef* port;
	uint16_t     pin;
}LDDescr;
#endif
