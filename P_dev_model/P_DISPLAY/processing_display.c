/**
  ******************************************************************************
  * @file    processing_display.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */
#include "stdint.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#include "processing_mem_map_extern.h"
#include "processing_display.h"
#include "max7219.h"
#include "stmMaxHardwareInit.h"



extern S_address_oper_data s_address_oper_data;

struct {
	displayBuffDef    displayBuff[2];
	displayHandlerDef displayHandler;
	struct {
		GPIO_TypeDef* port;
		uint16_t     pin;
	} ldDef[numberOfScreen];
} displayDiscr = {
		.ldDef = {
		[SCREEN_1] = {
				.port = GPIOA,
				.pin = 16
				},
		[SCREEN_2] = {
				.port = GPIOA,
				.pin = 16
				},
		[SCREEN_3] = {
				.port = GPIOA,
				.pin = 16
				},
		[SCREEN_4] = {
				.port = GPIOA,
				.pin = 16
				},
		},
};


/*!
 * \brief stmMaxInitGPIOLD configuration LD pin as output
 * \param[in]  ldGpio       selected GPIO port
 * \param[in]  ldPin        selected GPIO Pin. This parameter can be any value of @ref GPIO_pins_define
 * \param[out] HARDWARE_INIT_STATUS result status
 *
 */
void displayInitGPIOLd(void *ldGpio, void *ldPin)
{
	GPIO_InitTypeDef gpioLdInit;
	if(enableGPIO((GPIO_TypeDef*)ldGpio) != HARDWARE_INIT_STATUS_OK)
	{
		return HARDWARE_INIT_STATUS_ERROR_GPIO;
	}
	gpioLdInit.GPIO_Speed = GPIO_Speed_10MHz;
	gpioLdInit.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpioLdInit.GPIO_Pin   = *((uint16_t*)ldPin);
	GPIO_Init((GPIO_TypeDef*)ldGpio, &gpioLdInit);

	return HARDWARE_INIT_STATUS_OK;
}



/* @brief
 *
 */
uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start){
	return adres_start;
}

static void initTimerForLDPuls(){
	RCC_ClocksTypeDef rccClock;
	uint16_t psc = (rccClock.PCLK1_Frequency/1000000 - 1);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_GetClocksFreq(&rccClock);

	TIM_PrescalerConfig(LD_TIMER, (rccClock.PCLK1_Frequency/1000000 - 1), TIM_PSCReloadMode_Immediate );
	TIM_ARRPreloadConfig(LD_TIMER, ENABLE);
	TIM_SetAutoreload(LD_TIMER ,(psc+1)*LD_LENGT_uS );
	TIM_ITConfig(LD_TIMER, TIM_IT_Update, ENABLE);
	TIM_GenerateEvent(LD_TIMER, TIM_EventSource_Update);
	TIM_ClearFlag(LD_TIMER, TIM_FLAG_Update);
	NVIC_EnableIRQ(TIM2_IRQn);
	TIM_Cmd(LD_TIMER, ENABLE);
}

void TIM2_IRQHandler(void){
	if(TIM_GetFlagStatus(LD_TIMER, TIM_FLAG_Update) != SET){
		return;
	}

}

void generateLdPuls(){

}

void t_processing_display(void *pvParameters){

	initTimerForLDPuls();
	while(1){
	}
}
