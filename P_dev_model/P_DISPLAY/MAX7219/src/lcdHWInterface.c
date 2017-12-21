/**
 ******************************************************************************
 * @file    stmMaxHarwareInit.c
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    23-November-2016
 * @brief
 */

#include "stddef.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"

#include "lcdHWInterface.h"



#include "LCD.h"

// ------------------------definition for screen----------------------
static LDDescr LDList[NUMBER_OF_LCD_STRING] = {
		[SCREEN_1] = {
				.port = PORT_LD_1,
				.pin =  PIN_LD_1
				},
		[SCREEN_2] = {
				.port = PORT_LD_2,
				.pin =  PIN_LD_2
				},
		[SCREEN_3] = {
				.port = PORT_LD_3,
				.pin =  PIN_LD_3
				},
		[SCREEN_4] = {
				.port = PORT_LD_4,
				.pin =  PIN_LD_4
				},
};


static volatile struct {
	uint16_t numLD;
	TX_ADDRESS address;
} ldTxDef;



// Static functions prototype definition
static void enableGPIO(GPIO_TypeDef *GPIOx);
static void enableSPI(SPI_TypeDef *sellSPI);
static void configSPI(SPI_TypeDef *sellSPI);
static void configGPIOLd(GPIO_TypeDef* ldGpio, uint16_t ldPin);
static void configTimer(void);
static void generateLdPuls(void);
static void spiInterruptTx(void);


static void enableGPIO(GPIO_TypeDef *GPIOx){
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
		return;
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
}

static void enableSPI(SPI_TypeDef *sellSPI){
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
}

static void configSPI(SPI_TypeDef *sellSPI){
	SPI_InitTypeDef spiInitTypedef;
	enableSPI(sellSPI);

	spiInitTypedef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
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
	SPI_I2S_ITConfig(sellSPI, SPI_I2S_IT_RXNE, ENABLE);
	SPI_Cmd(sellSPI,ENABLE);
}

void SPI1_IRQHandler(void){
	volatile uint16_t rxData;
	if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET)
	{
		return;
	}
	SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
	SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
	rxData = SPI_I2S_ReceiveData(SPI1);
	// Call Callback function
	spiInterruptTx();
}

void SPI2_IRQHandler(void){
	volatile uint16_t rxData;
	if(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET)
	{
		return;
	}
	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_RXNE);
	rxData = SPI_I2S_ReceiveData(SPI2);
	// Call Callback function
	spiInterruptTx();
}

/*
 * \brief stmMaxInitGPIOLD configuration LD pin as output
 * \param[in]  ldGpio       selected GPIO port
 * \param[in]  ldPin        selected GPIO Pin. This parameter can be any value of @ref GPIO_pins_define
 * \param[out] HARDWARE_INIT_STATUS result status
 *
 */
static void configGPIOLd(GPIO_TypeDef* ldGpio, uint16_t ldPin){
	GPIO_InitTypeDef gpioLdInit;
	enableGPIO(ldGpio);
	gpioLdInit.GPIO_Speed = GPIO_Speed_10MHz;
	gpioLdInit.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpioLdInit.GPIO_Pin   = ldPin;
	GPIO_Init(ldGpio, &gpioLdInit);
	// Reset LD pin
	GPIO_ResetBits(ldGpio, ldPin);
	return;// HARDWARE_INIT_STATUS_OK;
}


/**
  * @brief
  * @param
  * @retval
  */
static void configTimer(void){
	RCC_ClocksTypeDef rccClock;
	RCC_GetClocksFreq(&rccClock);
	uint16_t psc = (( rccClock.PCLK1_Frequency/1000000 )*2 - 1);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	TIM_PrescalerConfig(LD_TIMER, psc, TIM_PSCReloadMode_Immediate );
	TIM_ARRPreloadConfig(LD_TIMER, ENABLE);
	TIM_SetAutoreload(LD_TIMER ,LD_LENGT_uS );
	TIM_ITConfig(LD_TIMER, TIM_IT_Update, ENABLE);
	TIM_GenerateEvent(LD_TIMER, TIM_EventSource_Update);
	TIM_ClearFlag(LD_TIMER, TIM_FLAG_Update);
	NVIC_EnableIRQ(TIM2_IRQn);
}

/**
  * @brief
  * @param
  * @retval
  */
void TIM2_IRQHandler(void){
	if(TIM_GetFlagStatus(LD_TIMER, TIM_FLAG_Update) != SET){
		return;
	}
	TIM_ClearFlag(LD_TIMER, TIM_FLAG_Update);
	TIM_Cmd(LD_TIMER, DISABLE);

	if(ldTxDef.address == TX_ADDRESS_ONE)
	{
		GPIO_ResetBits(LDList[ldTxDef.numLD].port, LDList[ldTxDef.numLD].pin);
	}
	else
	{
		uint16_t cnt = 0;
		for(; cnt < NUMBER_OF_LCD_STRING; cnt++ )
		{
			GPIO_ResetBits(LDList[cnt].port, LDList[cnt].pin);
		}
	}
	TIM_Cmd(LD_TIMER, DISABLE);
	TIM_SetCounter(LD_TIMER, 0);

	spiInterruptTx();
}

/**
  * @brief
  * @param
  * @retval
  */
static void generateLdPuls(void){

	if(ldTxDef.address == TX_ADDRESS_ONE)
	{
		GPIO_SetBits(LDList[ldTxDef.numLD].port, LDList[ldTxDef.numLD].pin);
	}
	else
	{
		uint16_t cnt = 0;
		for(;cnt < NUMBER_OF_LCD_STRING; cnt++ )
		{
			GPIO_SetBits(LDList[cnt].port, LDList[cnt].pin);
		}
	}
	TIM_Cmd(LD_TIMER, ENABLE);
}


/**
  * @brief
  * @param
  * @retval
  */
static void spiInterruptTx(void){
	uint16_t nextSumbol;
	txState txState = getNextData(&nextSumbol);

	switch(txState)
	{
	case TX_DATA:
		SPI_I2S_SendData(DISPLAY_SPI, nextSumbol);
		break;
	case GEN_LD:
		generateLdPuls();
		break;
	case TX_COMPLITE:

		break;
	default: break;
	}
}


/**
  * @brief
  * @param
  * @retval
  */
void hwInterfaceInit(void){
	uint16_t cnt = 0;

	// Config peripherals
	for(;cnt < NUMBER_OF_LCD_STRING; cnt++)
	{
		configGPIOLd(LDList[cnt].port, LDList[cnt].pin);
	}
	configTimer();
	configSPI(DISPLAY_SPI);

}


/**
  * @brief
  * @param
  * @retval
  */
void hwInterfaceTx(uint16_t orderNumberDispl, TX_ADDRESS txAddress){

	ldTxDef.address = txAddress;
	ldTxDef.numLD   = orderNumberDispl;
    // Start transmit data
	spiInterruptTx();
}

