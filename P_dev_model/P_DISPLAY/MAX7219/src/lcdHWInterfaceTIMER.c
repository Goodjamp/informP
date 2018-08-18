/********************************************************************************
 * @file    lcdHWInterfaceTIMER.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    26-December-2017
 * @brief
 */
#include "stddef.h"
#include "stdint.h"

#ifdef USE_QT

#else

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#include "lcdHWInterfaceTIMER.h"
#endif
#include "LCD.h"

#include "../inc/lcdHWInterfaceTIMER.h"

#ifdef USE_TIMER

static volatile LDDescr LDList[NUMBER_OF_LCD_STRING] = {
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

static struct{
    uint16_t data;
    uint8_t  txState: 2;
    uint8_t  clockState: 2;
    uint8_t  ldState: 4;
    uint8_t  clockCnt;
}txTimerState;


// Static function prototype
static void enableGPIO(GPIO_TypeDef *GPIOx);
static void configGPIOLd(GPIO_TypeDef* ldGpio, uint16_t ldPin);
static inline void startTimer(void);
static inline void stopTimer(void);

static inline void sendBit(void);
static inline void setClocktPin(void);
static inline void clearClockPin(void);
static inline void setDatatPin(void);
static inline void clearDataPin(void);


static inline void sendLD(void);
static inline void setLDPin(void);
static inline void clearLDPin(void);

static inline void sendComplite(void){};


static inline TX_STATE getNextTxState(void){
	switch (getNextData(&txTimerState.data)) {
	case TX_DATA:
		txTimerState.clockCnt = 0;
		txTimerState.clockState = CLOCK_STATE_HIGHT;
		return txTimerState.txState = TX_STATE_BIT;
	case GEN_LD:
		txTimerState.ldState = LD_STATE_SET;
		return txTimerState.txState = TX_STATE_LD;
	case TX_COMPLITE:
		stopTimer();
		return txTimerState.txState = TX_STATE_COMPLITE;
	default:
		stopTimer();
		return txTimerState.txState = TX_STATE_COMPLITE;
	}
}


static inline void sendBit(void){
	if( CLOCK_STATE_HIGHT == txTimerState.clockState )
	{
		clearClockPin();
		txTimerState.clockState = CLOCK_STATE_LOW;

		if( txTimerState.clockCnt < NUMBER_OF_BIT )
		{
		    if( txTimerState.data & ( BIT_SELECTOR_MASK >> txTimerState.clockCnt) )
		    {
			    setDatatPin();
		    }
		    else
		    {
			    clearDataPin();
		    }
		}
		else
		{
			if( TX_STATE_LD == getNextTxState() )
			{

				sendLD();
			}
            return;
		}
		txTimerState.clockCnt++; // should be zero when start
		return;
	}
	else
	{
	    setClocktPin();
	    txTimerState.clockState = CLOCK_STATE_HIGHT;
	    return;
	}
}


static inline void sendLD(void){
    switch(txTimerState.ldState){
    case LD_STATE_SET:
    	setLDPin();
    	txTimerState.ldState = LD_STATE_CLEAR;
    	return;
    case LD_STATE_CLEAR:
    	clearLDPin();

    	if( TX_STATE_COMPLITE == getNextTxState() )
    	{
    		return;
    	}
    	sendBit();
    	return;
    };
}



static inline void transmitCallback(void){

	switch( txTimerState.txState){
	case TX_DATA:
		sendBit();
		return;
	case GEN_LD:
		sendLD();
		return;
	}
}



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


static inline void startTimer(void){
	TIM_SetCounter(CLOCK_TIMER,0);
	TIM_Cmd(CLOCK_TIMER,ENABLE);
}


static inline void stopTimer(void){
	TIM_Cmd(CLOCK_TIMER,DISABLE);
}


static inline void setLDPin(void){
    startGenLD_();
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
}


static inline void clearLDPin(void){
	if(ldTxDef.address == TX_ADDRESS_ONE)
	{
		GPIO_ResetBits(LDList[ldTxDef.numLD].port, LDList[ldTxDef.numLD].pin);
	}
	else
	{
		uint16_t cnt = 0;
		for(;cnt < NUMBER_OF_LCD_STRING; cnt++ )
		{
			GPIO_ResetBits(LDList[cnt].port, LDList[cnt].pin);
		}
	}
}


static inline void setDatatPin(void){
	dataSetPin_()
	GPIO_SetBits(GPIO_DATA_PORT, GPIO_DATA_PIN);
}

static inline void clearDataPin(void){
	dataClearPin_()
	GPIO_ResetBits(GPIO_DATA_PORT, GPIO_DATA_PIN);
}

static inline void setClocktPin(void){
	clockSetPin_()
	GPIO_SetBits(GPIO_CLOCK_PORT, GPIO_CLOCK_PIN);
}

static inline void clearClockPin(void){
	clockClearPin_()
	GPIO_ResetBits(GPIO_CLOCK_PORT, GPIO_CLOCK_PIN);
}



static inline void configTImer(void) {
    RCC_ClocksTypeDef rccClock;
	RCC_GetClocksFreq(&rccClock);
	uint16_t maxCntVal = (rccClock.PCLK1_Frequency * 2) / (CLOCK_FRQ * 2);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_ARRPreloadConfig(CLOCK_TIMER, ENABLE);
	TIM_SetAutoreload(CLOCK_TIMER, maxCntVal);
	TIM_ITConfig(CLOCK_TIMER, TIM_IT_Update, ENABLE);
	TIM_GenerateEvent(CLOCK_TIMER, TIM_EventSource_Update);
	TIM_ClearFlag(CLOCK_TIMER, TIM_FLAG_Update);
	NVIC_EnableIRQ(TIM2_IRQn);
}

/**
 * @brief
 * @param
 * @retval
 */
void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(CLOCK_TIMER, TIM_IT_Update) != SET) {
		return;
	}
	TIM_ClearITPendingBit(CLOCK_TIMER, TIM_FLAG_Update);
	transmitCallback();
}
/**
  * @brief
  * @param
  * @retval
  */
void hwInterfaceInit(void){
	uint8_t cnt;
    GPIO_InitTypeDef gpioConfig;
    gpioConfig.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpioConfig.GPIO_Speed = GPIO_Speed_10MHz;
    // CLOCK GPIO
    enableGPIO(GPIO_CLOCK_PORT);
    gpioConfig.GPIO_Pin   = GPIO_CLOCK_PIN;
    GPIO_Init(GPIO_CLOCK_PORT, &gpioConfig);
    clearClockPin();
    // DATA GPIO
    enableGPIO(GPIO_DATA_PORT);
    gpioConfig.GPIO_Pin   = GPIO_DATA_PIN;
    GPIO_Init(GPIO_DATA_PORT, &gpioConfig);
    clearDataPin();
	// Config LD
	for(cnt = 0; cnt < NUMBER_OF_LCD_STRING; cnt++)
	{
		configGPIOLd(LDList[cnt].port, LDList[cnt].pin);
	}
	//Config timer
	configTImer();
}


/**
  * @brief
  * @param
  * @retval
  */
void hwInterfaceTx(uint16_t orderNumberDispl, TX_ADDRESS txAddress){
	ldTxDef.address = txAddress;
	ldTxDef.numLD   = orderNumberDispl;

	txTimerState.txState =  TX_STATE_BIT;
	txTimerState.clockState = CLOCK_STATE_HIGHT;
    transmitCallback();
    startTimer();
}

#endif
