/**
 ******************************************************************************
 * @file    stmMaxHarwareInit.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-November-2016
 * @brief
 */

#include "stddef.h"

#include "stmMaxHardwareInit.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

pfspiCallback spiCallback = NULL;

static HARDWARE_INIT_STATUS enableGPIO(GPIO_TypeDef *GPIOx)
{
	if(!IS_GPIO_ALL_PERIPH(GPIOx))
	{
		return HARDWARE_INIT_STATUS_ERROR_GPIO;
	}
	if(GPIOx == GPIOA)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE);
	}
	else if(GPIOx == GPIOB)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,  ENABLE);
	}
	else if(GPIOx == GPIOC)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,  ENABLE);
	}
	else if(GPIOx == GPIOD)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,  ENABLE);
	}
	else if(GPIOx == GPIOE)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,  ENABLE);
	}
	else
	{
		return HARDWARE_INIT_STATUS_ERROR_GPIO;
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
	return HARDWARE_INIT_STATUS_OK;
}

static HARDWARE_INIT_STATUS enableSPI(SPI_TypeDef *sellSPI)
{
	GPIO_InitTypeDef gpioSPI_SCK_init;
	GPIO_InitTypeDef gpioSPI_MOSI_init;
	// SCK pin init
	gpioSPI_SCK_init.GPIO_Speed = GPIO_Speed_10MHz;
	gpioSPI_SCK_init.GPIO_Mode  = GPIO_Mode_AF_PP;
	// MOSI pin init
	gpioSPI_MOSI_init.GPIO_Speed = GPIO_Speed_10MHz;
	gpioSPI_MOSI_init.GPIO_Mode  = GPIO_Mode_AF_PP;
	if(sellSPI == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,   ENABLE);
		RCC_APB2PeriphClockCmd(SPI1_GPIO_SCK_ENABLE,  ENABLE);
		RCC_APB2PeriphClockCmd(SPI1_GPIO_MOSI_ENABLE, ENABLE);
		gpioSPI_SCK_init.GPIO_Pin  = SPI1_GPIO_SCK_PIN;
		gpioSPI_MOSI_init.GPIO_Pin = SPI1_GPIO_MOSI_PIN;
		GPIO_Init(SPI1_GPIO_SCK,  &gpioSPI_SCK_init);
		GPIO_Init(SPI1_GPIO_MOSI, &gpioSPI_MOSI_init);
		NVIC_EnableIRQ(SPI1_IRQn);
	}
	else if(sellSPI == SPI2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,   ENABLE);
		RCC_APB2PeriphClockCmd(SPI2_GPIO_SCK_ENABLE,  ENABLE);
		RCC_APB2PeriphClockCmd(SPI2_GPIO_MOSI_ENABLE, ENABLE);
		gpioSPI_SCK_init.GPIO_Pin  = SPI2_GPIO_SCK_PIN;
		gpioSPI_MOSI_init.GPIO_Pin = SPI2_GPIO_MOSI_PIN;
		GPIO_Init(SPI2_GPIO_SCK,  &gpioSPI_SCK_init);
		GPIO_Init(SPI2_GPIO_MOSI, &gpioSPI_MOSI_init);
		NVIC_EnableIRQ(SPI2_IRQn);
	}
	else
	{
		return HARDWARE_INIT_STATUS_ERROR_SPI;
	}
	return  HARDWARE_INIT_STATUS_OK;
}

HARDWARE_INIT_STATUS stmMaxSPIinit(SPI_TypeDef *sellSPI, pfspiCallback pfCallback)
{
	SPI_InitTypeDef spiInitTypedef;
	if(enableSPI(sellSPI) == HARDWARE_INIT_STATUS_ERROR_SPI)
	{
		return HARDWARE_INIT_STATUS_ERROR_SPI;
	}
	spiInitTypedef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	spiInitTypedef.SPI_CPHA = SPI_CPHA_1Edge;
	spiInitTypedef.SPI_CPOL = SPI_CPOL_Low;
	spiInitTypedef.SPI_CRCPolynomial = 0;
	spiInitTypedef.SPI_DataSize = SPI_DataSize_16b;
	// TODO  check configuration field SPI_CPHA SPI_Direction
	spiInitTypedef.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spiInitTypedef.SPI_FirstBit = SPI_FirstBit_MSB;
	spiInitTypedef.SPI_Mode = SPI_Mode_Master;
	spiInitTypedef.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(sellSPI, &spiInitTypedef);
	// Set spi interrupt callback function]
	SPI_IT_TX_ENABLE(sellSPI);
	spiCallback = pfCallback;
	SPI_Cmd(sellSPI,ENABLE);
	return HARDWARE_INIT_STATUS_OK;
}


void stmMaxLdPuls(GPIO_TypeDef *GPIOx, uint32_t PINx)
{
	GPIO_SetBits(GPIOx, PINx);

	GPIO_ResetBits(GPIOx, PINx);
}

void SPI1_IRQHandler(void)
{
	// Call Callback function
	if(spiCallback != NULL)
	{
		spiCallback(SPI1);
	}
}

void SPI2_IRQHandler(void)
{
	// Call Callback function
	if(spiCallback != NULL)
	{
		spiCallback(SPI2);
	}
}

