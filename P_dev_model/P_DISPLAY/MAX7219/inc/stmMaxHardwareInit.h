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

#include "max7219.h"

#define LD_TIMER     TIM2
#define LD_MIN_us       1
#define LD_LENGT_uS     5


// Number of SPI
#define NUM_SPI		   2

// Macros definition for enable GPIO
//#define ENABLE_GPIO(X)  ENABLE_GPIO_(X)
//#define ENABLE_GPIO_(X) RCC_APB2PeriphClockCmd(RCC_APB2Periph_##X,  ENABLE);

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


// SPI Interrupt callback function prototype
typedef void (*pfspiCallback)(void);

typedef enum {
	HARDWARE_INIT_STATUS_OK,
	HARDWARE_INIT_STATUS_ERROR_GPIO,
	HARDWARE_INIT_STATUS_ERROR_SPI,
}HARDWARE_INIT_STATUS;


/* @brief
 * */
typedef enum {
	TX_ADDRESS_ONE = 0x0,
	TX_ADDRESS_ALL = 0x1F,
}TX_ADDRESS;

typedef struct{
	GPIO_TypeDef* port;
	uint16_t     pin;
}LDDescr;


DISPLAY_STATUS displayTxData(displayHandlerDef *displayHandler, uint16_t orderNumberDispl, uint8_t numData, TX_ADDRESS txAddress);
void initDisplay(displayHandlerDef *displayHandler, LDDescr *LDList, uint16_t numScreen, SPI_TypeDef *selSPI);

#endif
