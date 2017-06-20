/*
 * file:   RTCProcessing.c
 * Authot: Gerasimchuk A.
 * Date:   19 jun 2017
 * Version 1.0
 */

#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"

//include FreeR

//include FreeRtos header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

// include my header
#include "processing_TIME.h"
#include "processing_mem_map_extern.h"
#include "processing_simple_gpio.h"

void initRTC(void){
	uint32_t lseRedyCNT = 0;
	// Enable Power interface and Back Up domain
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	//enable access to BuckUpDomain registers
	PWR_BackupAccessCmd(ENABLE);
	// Config oscilation RTC
	RCC_LSEConfig(RCC_LSE_ON);
	for(;;lseRedyCNT++){
		if(RCC_GetFlagStatus(RCC_FLAG_LSERDY)){break;};
	}
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	//Enter to config RTC mode
	RTC_EnterConfigMode();
	RTC_WaitForLastTask();
	// Set RTC prescaler
	RTC_SetPrescaler(0x7FFF); // 1 seconds period
	RTC_WaitForLastTask();

	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	RTC_WaitForLastTask();
	//Enter fron config RTC mode
	RTC_ExitConfigMode();
	RTC_WaitForLastTask();

	NVIC_EnableIRQ(RTC_IRQn);
}


// RTC interrup one second
void RTC_IRQHandler(void){
	if( !RTC_GetITStatus(RTC_IT_SEC) ) return;
	RTC_ClearITPendingBit(RTC_IT_SEC);
	P6_PORT->ODR ^= GPIO_ODR_ODR3;
}
