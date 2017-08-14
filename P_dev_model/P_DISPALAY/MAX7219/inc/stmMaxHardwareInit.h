/**
 ******************************************************************************
 * @file    HardwareInit.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-November-2016
 * @brief
 */


#ifndef STMMAXSHARDWAREINIT_H_
#define STMMAXSHARDWAREINIT_H_
#include "stm32f10x_spi.h"

// Macros definition for enable/disable interrupt
#define SPI_IT_TX_ENABLE(X)	 SPI_I2S_ITConfig(X, SPI_I2S_IT_RXNE, ENABLE);
#define SPI_IT_TX_DISABLE(X) SPI_I2S_ITConfig(X, SPI_I2S_IT_RXNE,DISABLE);

// Macros definition for enable GPIO
#define ENABLE_GPIO(X)  ENABLE_GPIO_(X)
#define ENABLE_GPIO_(X) RCC_APB2PeriphClockCmd(RCC_APB2Periph_##X,  ENABLE);

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
#define SPI2_GPIO_MOSI_PIN    GPIO_Pin_14
#define SPI2_GPIO_MISO_ENABLE RCC_APB2Periph_GPIOB
#define SPI2_GPIO_MISO        GPIOB
#define SPI2_GPIO_MISO_PIN    GPIO_Pin_15

typedef enum {
	HARDWARE_INIT_STATUS_OK,
	HARDWARE_INIT_STATUS_ERROR_GPIO,
	HARDWARE_INIT_STATUS_ERROR_SPI,
}HARDWARE_INIT_STATUS;

typedef void (*pfspiCallback)(SPI_TypeDef*);

HARDWARE_INIT_STATUS stmMaxSPIinit(SPI_TypeDef *sellSPI, pfspiCallback pfCallback);
HARDWARE_INIT_STATUS stmMaxInitGPIOLd(GPIO_TypeDef *ldGpio,uint16_t ldPin);
void stmMaxLdPuls(GPIO_TypeDef *GPIOx, uint32_t PINx);
HARDWARE_INIT_STATUS stmMaxresetLD(void);

#endif
