/********************************************************************************
  * @file:   RTCProcessing.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    19 jun 2017
  * @brief
  */

#include "stdint.h"
#include "stddef.h"

#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"

#include "clockProcessing.h"

static rtcCalbackDef secondCallback = NULL;
static rtcCalbackDef alarmCallback = NULL;

void clockInit(void){
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
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
	RTC_WaitForLastTask();
	//Enter fron config RTC mode
	RTC_ExitConfigMode();
	RTC_WaitForLastTask();

	NVIC_EnableIRQ(RTC_IRQn);
}


void clockSetCallback(rtcCalbackDef secondCallbackIn, rtcCalbackDef alarmCallbackIn){
	secondCallback = secondCallbackIn;
	alarmCallback  = alarmCallbackIn;
}


// RTC interrup one second
void RTC_IRQHandler(void){
	if( !(RTC_GetITStatus(RTC_IT_SEC) || RTC_GetITStatus(RTC_IT_ALR)) ) return;

	if(   RTC_GetITStatus(RTC_IT_ALR) )
	{
		if( secondCallback != NULL)
		{
			secondCallback(RTC_GetCounter());
		}
		RTC_ClearITPendingBit(RTC_IT_SEC);
	}
	else if( RTC_GetITStatus(RTC_IT_SEC) )
	{
		if( alarmCallback != NULL)
		{
			alarmCallback(RTC_GetCounter());
		}
		RTC_ClearITPendingBit(RTC_IT_ALR);
	}
}


void clockSetTime(uint32_t UTCtime){
	RTC_SetCounter(UTCtime);
}


void clockSetAllarmTime(uint32_t UTCtime){
	 RTC_SetAlarm(UTCtime);
}


uint32_t clockGetTime(void){
	return RTC_GetCounter();
}

