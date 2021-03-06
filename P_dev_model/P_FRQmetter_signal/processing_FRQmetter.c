/**
  ******************************************************************************
  * @file    processing_FRQmetter.hprocessing_config_dev_init
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
#include "processing_FRQmetter_extern.h"
#include "processing_mem_map_extern.h"
#include "flash_operation.h"
#include "funct.h"
#include "processing_reset_control.h"

// All data address
extern S_address_oper_data s_address_oper_data;

xSemaphoreHandle semaphoreUpdateFRQ;
//static uint16_t status;
static S_FRQmetter_user_config *s_FRQConfig;

struct{
	uint16_t rezArray[NUMBER_OF_RESULTS];
	uint32_t summ;
	uint16_t cnt;
}frqMiddleRez;

uint16_t updateMiddleRez(uint16_t newFrq){
	frqMiddleRez.summ = frqMiddleRez.summ - frqMiddleRez.rezArray[ frqMiddleRez.cnt ] + newFrq;
	frqMiddleRez.rezArray[ frqMiddleRez.cnt++ ] = newFrq;
    frqMiddleRez.cnt &= (MAX_INDEX_OF_RESULTS);
    return (uint16_t)( frqMiddleRez.summ/NUMBER_OF_RESULTS );
}

volatile struct{
	uint8_t updateCNT;
	uint32_t inputCaptureCNT;
	volatile uint8_t f_ICinterrupt;
	double df;
	uint32_t rezMesFRQ;
	uint32_t Fosc;
} frqRezMes = {
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
	for(uint32_t cnt=1; cnt < 65535 ; cnt++){
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
/*
#define LOG_ARRAY_SIZE  100
static uint16_t rez[LOG_ARRAY_SIZE] = {[0 ... (LOG_ARRAY_SIZE - 1)] = 0};
static uint32_t rezC[LOG_ARRAY_SIZE] = {[0 ... (LOG_ARRAY_SIZE - 1)] = 0};
static void logFRQ(uint32_t newCNT, uint16_t newF){

	static uint8_t rezCNT = 0;
	rezC[rezCNT] = newCNT;
	rez[rezCNT] = newF;
	rezCNT++;
	if(rezCNT >= LOG_ARRAY_SIZE) rezCNT=0;
}
*/

/* @brief
 *
 */
u16 FRQmetter_calc_address_oper_reg(S_FRQmetter_address *ps_FRQmetter_address, u16 adres_start){
	ps_FRQmetter_address->status_FRQmetter = adres_start;
	ps_FRQmetter_address->rez_FRQmetter = ps_FRQmetter_address->status_FRQmetter + structFieldRegSize(S_FRQmetter_oper_data,status_FRQmetter);
	return ps_FRQmetter_address->rez_FRQmetter + structFieldRegSize(S_FRQmetter_oper_data,rez_FRQmetter);
}


static void updateFrqStatus(FRQ_STATUS newStatus){
	static FRQ_STATUS currentStatus = FRQ_STATUS_NOT_SET;
	if( newStatus == currentStatus )
	{
		return;
	}
	// cast status
	castStatus status = {
		.statusUser = newStatus
	};
	//selected operation
	switch( newStatus){
	    case FRQ_STATUS_OK:
        	// update global status
	    	if( currentStatus == FRQ_STATUS_ERROR )
	    	{
		    	RESET_GLOBAL_STATUS(DEV_5);
	    	}
	    	processing_mem_map_write_s_proces_object_modbus(&status.statusU16, 1, s_address_oper_data.s_FRQmetter_address.status_FRQmetter);
	    	currentStatus = FRQ_STATUS_OK;
	    	break;
	    case FRQ_STATUS_ALLARM:
	        if( currentStatus == FRQ_STATUS_ERROR )
	        {
	        	// update global status
	        	RESET_GLOBAL_STATUS(DEV_5);
	        }
	        processing_mem_map_write_s_proces_object_modbus(&status.statusU16, 1, s_address_oper_data.s_FRQmetter_address.status_FRQmetter);
	        currentStatus = FRQ_STATUS_ALLARM;
	        break;
	    case FRQ_STATUS_ERROR:
        	// update global status
	    	SET_GLOBAL_STATUS(DEV_5);
	        processing_mem_map_write_s_proces_object_modbus(&status.statusU16, 1, s_address_oper_data.s_FRQmetter_address.status_FRQmetter);
	    	currentStatus = FRQ_STATUS_ERROR;
	    	break;
	    default: break;
	}

}

/* @brief
 *
 */
void t_processing_FRQmetter(void *pvParameters){
	uint32_t totatalCNT;
	uint16_t middleFrq;
	uint16_t momentaryFrq;
	uint8_t outOfRangeCounter = 0;
	s_FRQConfig =(S_FRQmetter_user_config*)pvParameters;

    // init state - ERROR (up to obtain first result)
	updateFrqStatus(FRQ_STATUS_OK);
	//stop task
	if(s_FRQConfig->state == DISABLE)
	{
		vTaskDelete(NULL);
	}

	//	Configure all peripherals
	frqGPIOConfig();
	frqTIMConfigure();
	vSemaphoreCreateBinary(semaphoreUpdateFRQ);

	while(1){

		if( xSemaphoreTake(semaphoreUpdateFRQ,ERROR_TIMEOUT_MS ) == pdFALSE){
            //set local error (in module register)
			updateFrqStatus(FRQ_STATUS_ERROR);
			continue;
		}
        // processing result of measurement
 		frqRezMes.f_ICinterrupt = 0;
		totatalCNT = frqRezMes.updateCNT * TIM_MAX_CNT + frqRezMes.inputCaptureCNT;
		frqRezMes.updateCNT = 0;
		frqRezMes.inputCaptureCNT = 0;
		// calculate middleFrq + correction
		momentaryFrq = ((float)(frqRezMes.df/(float)totatalCNT)*1000.f) + s_FRQConfig->frqCorrection;
		//logFRQ(totatalCNT, momentaryFrq);
	    if ((momentaryFrq > FRQ_MAX_MES) || (momentaryFrq < FRQ_MIN_MES)){
	    	if(outOfRangeCounter < OUT_OF_RANGE_THRESHOLD) {
	    		outOfRangeCounter++;
	    		continue;
	    	}
	    } else {
	    	outOfRangeCounter = 0;
	    }
		middleFrq = updateMiddleRez(momentaryFrq);
		if((middleFrq < FRQ_MAX) && (middleFrq > FRQ_MIN)){
			updateFrqStatus(FRQ_STATUS_OK);
		} else {
			updateFrqStatus(FRQ_STATUS_ALLARM);
		}
		processing_mem_map_write_s_proces_object_modbus(&middleFrq, 1, s_address_oper_data.s_FRQmetter_address.rez_FRQmetter);
	}
}
