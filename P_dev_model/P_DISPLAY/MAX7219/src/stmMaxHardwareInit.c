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
#include "stm32f10x_tim.h"

#include "max7219.h"

pfspiCallback spiCallback[NUM_SPI] = {[0 ... NUM_SPI - 1] = NULL};

struct {
	uint16_t numScreen;
	LDDescr *ListOfLD;
	SPI_TypeDef *sellSPI;
} LDGenDef;

displayHandlerDef *displayIntarface;

volatile struct {
	enum {
		LD_FREE,
		LD_BUSY
	} state;
	displayHandlerDef *displayHandler;
	uint16_t numLD;
	TX_ADDRESS address;
} ldStatusDef =
{
		.state = LD_FREE
};


// Static functions prototype definition
static HARDWARE_INIT_STATUS enableGPIO(GPIO_TypeDef *GPIOx);
static HARDWARE_INIT_STATUS enableSPI(SPI_TypeDef *sellSPI);
HARDWARE_INIT_STATUS configSPI(SPI_TypeDef *sellSPI, pfspiCallback pfCallback);
static void displayInitGPIOLd(GPIO_TypeDef* ldGpio, uint16_t ldPin);
static void configTimer(void);
static void generateLdPuls(void);
static void interruptSPICallback_Intarface(void);
static void displayInterruptTx(displayHandlerDef *displayHandler);



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

HARDWARE_INIT_STATUS configSPI(SPI_TypeDef *sellSPI, pfspiCallback pfCallback)
{
	SPI_InitTypeDef spiInitTypedef;
	if(enableSPI(sellSPI) == HARDWARE_INIT_STATUS_ERROR_SPI)
	{
		return HARDWARE_INIT_STATUS_ERROR_SPI;
	}
	// Set SPI interrupt callback function
	switch((uint32_t)sellSPI){
		case (uint32_t)SPI1: spiCallback[0] = pfCallback;
				   break;
		case (uint32_t)SPI2: spiCallback[1] = pfCallback;
				   break;
		default: return HARDWARE_INIT_STATUS_ERROR_SPI;
	}
	if(sellSPI)
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
	return HARDWARE_INIT_STATUS_OK;
}

void SPI1_IRQHandler(void)
{
	volatile uint16_t readData;
	if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET)
	{
		return;
	}
	SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_RXNE);
	readData = SPI_I2S_ReceiveData(SPI2);
	// Call Callback function
	if(spiCallback != NULL)
	{
		spiCallback[0]();
	}
}

void SPI2_IRQHandler(void)
{
	volatile uint16_t readData;
	if(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET)
	{
		return;
	}
	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_RXNE);
	readData = SPI_I2S_ReceiveData(SPI2);
	// Call Callback function
	if(spiCallback != NULL)
	{
		spiCallback[1]();
	}
}

/*
 * \brief stmMaxInitGPIOLD configuration LD pin as output
 * \param[in]  ldGpio       selected GPIO port
 * \param[in]  ldPin        selected GPIO Pin. This parameter can be any value of @ref GPIO_pins_define
 * \param[out] HARDWARE_INIT_STATUS result status
 *
 */
static void displayInitGPIOLd(GPIO_TypeDef* ldGpio, uint16_t ldPin)
{
	GPIO_InitTypeDef gpioLdInit;
	if(enableGPIO(ldGpio) != HARDWARE_INIT_STATUS_OK)
	{
		return;// HARDWARE_INIT_STATUS_ERROR_GPIO;
	}
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
void TIM2_IRQHandler(void)
{
	if(TIM_GetFlagStatus(LD_TIMER, TIM_FLAG_Update) != SET){
		return;
	}
	TIM_ClearFlag(LD_TIMER, TIM_FLAG_Update);
	TIM_Cmd(LD_TIMER, DISABLE);

	if(ldStatusDef.address == TX_ADDRESS_ONE)
	{
		GPIO_ResetBits(LDGenDef.ListOfLD[ldStatusDef.numLD].port, LDGenDef.ListOfLD[ldStatusDef.numLD].pin);
		return;
	}
	else
	{
		uint16_t cnt = 0;
		for(;cnt<LDGenDef.numScreen; cnt++ )
		{
			GPIO_ResetBits(LDGenDef.ListOfLD[cnt].port, LDGenDef.ListOfLD[cnt].pin);
		}
	}
	TIM_Cmd(LD_TIMER, DISABLE);
	TIM_SetCounter(LD_TIMER, 0);

	displayInterruptTx(ldStatusDef.displayHandler);

	ldStatusDef.state = LD_FREE;
}

/**
  * @brief
  * @param
  * @retval
  */
static void generateLdPuls(void){
	if(ldStatusDef.state == LD_BUSY)
	{
		return;
	}
	ldStatusDef.state = LD_BUSY;
	if(ldStatusDef.address == TX_ADDRESS_ONE)
	{
		GPIO_SetBits(LDGenDef.ListOfLD[ldStatusDef.numLD].port, LDGenDef.ListOfLD[ldStatusDef.numLD].pin);
	}
	else
	{
		uint16_t cnt = 0;
		for(;cnt < LDGenDef.numScreen; cnt++ )
		{
			GPIO_SetBits(LDGenDef.ListOfLD[cnt].port, LDGenDef.ListOfLD[cnt].pin);
		}
	}
	TIM_Cmd(LD_TIMER, ENABLE);
}


//              SPI CALLBACK function
/**
  * @brief
  * @param
  * @retval
  */
static void interruptSPICallback_Intarface(void)
{
	displayInterruptTx(displayIntarface);
}

//              INTERFACE FUCTION
/**
  * @brief
  * @param
  * @retval
  */
static void displayInterruptTx(displayHandlerDef *displayHandler)
{
	uint16_t nextSumbol;
	txState txState = displayTxCallback(displayIntarface, &nextSumbol);

	switch(txState)
	{
	case TX_DATA:
		SPI_I2S_SendData(LDGenDef.sellSPI, nextSumbol);
		break;
	case GEN_LD:
		generateLdPuls();
		break;
	case TX_COMPLITE:

		break;
	default: break;
	}
}



//               USER CALL
/**
  * @brief
  * @param
  * @retval
  */
DISPLAY_STATUS displayTxData(displayHandlerDef *displayHandler, uint16_t orderNumberDispl, uint8_t numData, displayBuffDef *pData, TX_ADDRESS txAddress){

	if(displayTx(displayHandler, numData, pData) != DISPLAY_OK)
	{
		return DISPLAY_BUSY;
	};

	ldStatusDef.address = txAddress;
	ldStatusDef.numLD = orderNumberDispl;
	ldStatusDef.displayHandler = displayHandler;

	displayInterruptTx(displayHandler);
	return DISPLAY_BUSY;
}

void initDisplay(displayHandlerDef *displayHandler, LDDescr *LDList, uint16_t numScreen, SPI_TypeDef *selSPI)
{
	uint16_t cnt = 0;
	LDGenDef.sellSPI = selSPI;
	LDGenDef.ListOfLD = LDList;
	LDGenDef.numScreen = numScreen;
	displayInterfaceInit(displayHandler);
	displayIntarface = displayHandler;
	for(;cnt < LDGenDef.numScreen; cnt++)
	{
		displayInitGPIOLd(LDGenDef.ListOfLD[cnt].port, LDGenDef.ListOfLD[cnt].pin);
	}
	configTimer();
	configSPI(selSPI, interruptSPICallback_Intarface);

}


