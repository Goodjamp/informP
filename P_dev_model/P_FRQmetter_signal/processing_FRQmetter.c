/**
  ******************************************************************************
  * @file    processing_FRQmetter.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    3-Mach-2017
  * @brief
  */
#include "stdint.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"


#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#include "processing_FRQmetter.h"
#include "processing_mem_map_extern.h"
#include "flash_operation.h"
#include "funct.h"

// All data address
extern S_address_oper_data s_address_oper_data;

xSemaphoreHandle semaphoreUpdateFRQ;
//static uint16_t status;
static S_FRQmetter_user_config *s_FRQConfig;


volatile struct{
	uint8_t updateCNT;
	uint32_t inputCaptureCNT;
	volatile uint8_t f_ICinterrupt;
	double df;
	uint32_t rezMesFRQ;
	uint32_t Fosc;
}frqRezMes = {
		0,
		0,
		0,
		0
};

static void frqGPIOConfig(void){
	GPIO_InitTypeDef timGpioInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    timGpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    timGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    timGpioInit.GPIO_Pin = FREQ_INPUT_PIN;
    GPIO_Init(GPIOA,&timGpioInit);
}

static uint16_t frqTIMCalcPSC(void){
	RCC_ClocksTypeDef rccClock;
	uint32_t f50Hz = 50;
	uint32_t FoscIdeal = (f50Hz*f50Hz*10000)/FRQ_ACCURACY - f50Hz;
	RCC_GetClocksFreq(&rccClock);
	uint32_t cnt=1;
	for( ; cnt<65535 ; cnt++){
		if( GET_TIM_F(rccClock)/cnt <= FoscIdeal)
		{
			return cnt-1;
		}
	}
  return 0xFFFF;
}


static void frqTIMConfigure(void)
{
	TIM_TimeBaseInitTypeDef timBaseInit;
	TIM_ICInitTypeDef timICInit;
	RCC_ClocksTypeDef rccClock;
    // timer input init
    // selected and configure input channel
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
    RCC_GetClocksFreq(&rccClock);
    //Base TIM init
    timBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
    timBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
    timBaseInit.TIM_Period = TIM_MAX_CNT;
    timBaseInit.TIM_Prescaler = frqTIMCalcPSC()-1;
    timBaseInit.TIM_RepetitionCounter = 0;
    // calculate variable for calculate frq
    double timFRQ =  GET_TIM_F(rccClock);
    double timPSC = timBaseInit.TIM_Prescaler+1;
    frqRezMes.df = timFRQ/timPSC;
    //frqRezMes.df = (GET_TIM_F(rccClock)/(timBaseInit.TIM_Prescaler+1));

    TIM_TimeBaseInit(FREQ_TIMER,&timBaseInit);
    // Input mode init
    timICInit.TIM_Channel = TIM_Channel_1;
    timICInit.TIM_ICFilter = 0x3;
    timICInit.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    timICInit.TIM_ICPrescaler = 0;
    timICInit.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(FREQ_TIMER,&timICInit);
    TIM_CCxCmd(FREQ_TIMER, TIM_Channel_1, TIM_CCx_Enable);

    TIM_SetCounter(FREQ_TIMER, 0);

    // Set Coupte/compare interrupt
    TIM_ITConfig(FREQ_TIMER, TIM_IT_CC1, ENABLE);
    // Set update interrupt
    TIM_ITConfig(FREQ_TIMER, TIM_IT_Update, ENABLE);
    //
    //TIM_DMACmd(FREQ_TIMER, TIM_DMA_CC1, ENABLE);
    // Enable all needded interrupt
    NVIC_EnableIRQ(TIM1_CC_IRQn);
    NVIC_EnableIRQ(TIM1_UP_IRQn);//TIM1_UP_IRQn
    // Enable counters
    TIM_Cmd(FREQ_TIMER,ENABLE);

    __enable_irq();
}


/* @brief
 *
 */
void TIM1_CC_IRQHandler(void)
{
	portBASE_TYPE temp;
	TIM_ClearFlag(FREQ_TIMER, TIM_FLAG_CC1);
	FREQ_TIMER->CNT = 0;
	frqRezMes.f_ICinterrupt = 1;
	frqRezMes.inputCaptureCNT = FREQ_TIMER->CCR1;
	xSemaphoreGiveFromISR(semaphoreUpdateFRQ, &temp);
}


/* @brief
 *
 */
void TIM1_UP_IRQHandler(void)
{
	TIM_ClearFlag(FREQ_TIMER,TIM_FLAG_Update);
	frqRezMes.updateCNT++;
}


static uint16_t rez[100] = {[0 ... 19] = 0};
static uint32_t rezC[100] = {[0 ... 19] = 0};
static void logFRQ(uint32_t newCNT, uint16_t newF){

	static uint8_t rezCNT = 0;
	rezC[rezCNT] = newCNT;
	rez[rezCNT] = newF;
	rezCNT++;
	if(rezCNT >= 100) rezCNT=0;
}


/* @brief
 *
 */
u16 FRQmetter_calc_address_oper_reg(S_FRQmetter_address *ps_FRQmetter_address, u16 adres_start){
	ps_FRQmetter_address->status_FRQmetter = adres_start;
	ps_FRQmetter_address->rez_FRQmetter = ps_FRQmetter_address->status_FRQmetter + structFieldRegSize(S_FRQmetter_oper_data,status_FRQmetter);
	return ps_FRQmetter_address->rez_FRQmetter + structFieldRegSize(S_FRQmetter_oper_data,rez_FRQmetter);
}




static void updateFrqStatus(FRQ_STATUS newStatus){
	uint16_t status = (uint16_t)newStatus;
	processing_mem_map_write_s_proces_object_modbus(&status, 1, s_address_oper_data.s_FRQmetter_address.status_FRQmetter);
}

/* @brief
 *
 */
void t_processing_FRQmetter(void *pvParameters){
	uint32_t totatalCNT;
	uint16_t frq;
	s_FRQConfig =(S_FRQmetter_user_config*)pvParameters;

    // init state - ERROR (up to obtain first result)
	updateFrqStatus(FRQ_STATUS_ERROR);

	//	Configure all peripherals
	frqGPIOConfig();
	frqTIMConfigure(); //8842
	vSemaphoreCreateBinary(semaphoreUpdateFRQ);
	//xQueueSendToBackFromISR()
	while(1){

		if(xSemaphoreTake(semaphoreUpdateFRQ,ERROR_TIMEOUT_MS ) == pdFALSE){
            //error
			updateFrqStatus(FRQ_STATUS_ERROR);
			continue;
		}

		frqRezMes.f_ICinterrupt = 0;
		totatalCNT = frqRezMes.updateCNT * TIM_MAX_CNT + frqRezMes.inputCaptureCNT;
		// calculate frq + correction
		frq = ((float)(frqRezMes.df/(float)totatalCNT)*1000);
		frq = frq  + s_FRQConfig->frqCorrection*10;
		if((frq < FRQ_MAX) && (frq > FRQ_MIN)){
			updateFrqStatus(FRQ_STATUS_OK);
		}
		else{
			updateFrqStatus(FRQ_STATUS_ALLARM);
		}
		processing_mem_map_write_s_proces_object_modbus(&frq, 1, s_address_oper_data.s_FRQmetter_address.rez_FRQmetter);
		logFRQ(totatalCNT, frq);
		frqRezMes.updateCNT = 0;
		frqRezMes.inputCaptureCNT = 0;
	}
}
