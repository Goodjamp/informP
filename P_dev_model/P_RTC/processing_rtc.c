#include "stdio.h"
#include "string.h"

#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rtc.h"


//include FreeRtos header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

// include my header
#include "processing_USART.h"
#include "USART_fifo_operation.h"


u8 cheak_summ_head=0;
S_port_config s_port_config_gps;
u8 s_gprmc[]={'$','G','P','R','M','C',','};
u8 time_string[15];
S_Task_parameters *ptaskparameters;
u32 rtc_isr_state;

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
		if(RCC_GetFlagStatus(RCC_FLAG_LSERDY));
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
}


void t_processing_rtc(void *pvParameters){

	initRTC();

	while(1){

	}
}

