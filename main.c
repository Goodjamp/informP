/*
    FreeRTOS V7.0.2 - Copyright (C) 2011 Real Time Engineers Ltd.

*/
/* Kernel includes. */
#include "misc.h"

#include <stdlib.h>
#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

#include "funct.h"
#include "USART_fifo_operation.h"
#include "init_system.h"
#include "global_dev.h"
#include "processing_config_dev.h"
#include "processing_mem_map.h"
#include "processing_modbus.h"
#include "processing_simple_gpio.h"
#include "processing_reset_control.h"
#include "HIDInterface.h"
#include "debugStuff.h"

#include "GPSprocessing.h"


// ���� � �������� ��������� ����������
#include "processing_includ.h"


/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		1

/* The rate at which data is sent to the queue, specified in milliseconds, and
converted to ticks using the portTICK_RATE_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 200 / portTICK_RATE_MS )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

//static void prvSetupHardware( void );

//��������� �� ��������� � ����������� �� ������ ������ � �������� USART. �� processing_USART.�
extern S_Task_parameters *ptask_parameters;
extern S_connectmodbus *ps_connectmodbus_global;
//��������� �������� ����� ������ (��  mem_map_processing.c)
extern S_mem_map s_mem_map;
//����� ���������� �� USART ������� ������������ �������� ������������ (��  processing_USART.c)
extern USART_TypeDef *present_usart[NUMBER_USART];
S_config_moduls s_config_moduls; // ���������� ��������� �������� ������� ����������, ������� ������������� ����������� �������� � ������� ������ ��������,
// � ������� �� ����� ������ (� ����� ������ ������� �������� ����� ��������, �� �� ������������ ��������� ��
// ��������������� !! )


void delay_loop(){
	u32 coutner=0;
	while(coutner<1000){
		coutner++;
	}
}


S_modbus_tsk_par s_modbus_tsk_par[NUM_PORT_MODBUS];
//---------------------------------------------------------------------------
int main(void)
{
	u8 k1;
   debugPinConfig();
	//Run HID (usb)
	USB_HIDInit();
	//t_processing_display((void*)&k1);
	t_processing_sensor((void*)&k1);



	delay_loop(); // ������ ���� ���������������� �������
	// ��������� ������ ����������� ����������
	processing_simple_gpio();
	// ���������� ����� ������ � �������� ������� � ���
	processing_mem_map_init();
	// -----------------------------���� ������ ���������------------------------------------
	// ���� ������������ �� ��������� � ������������ ������������ �� �������� - ���������
	processing_config_firest_on();
	// �������� ���������������� ������ �� FLESH ������
	if(STATE_JAMPER1) // ���� ������� �1 �� ���������� (��� 1 �� �����) - ������� ������������ ������������
	{
		// �������� ������������ ������������
		INIT_MBB_read_addjust_table(s_mem_map.p_start_config_data,sizeof(S_global_config), PAGE_USER_CONFIG);
	}
	else // ���� ������� ���������� ((��� 0 �� �����))- ������� ������������ �� ���������
	{
		// �������� ������������ ��-���������
		INIT_MBB_read_addjust_table(s_mem_map.p_start_config_data,sizeof(S_global_config), PAGE_DEFAULT_CONFIG);

	}


	// ���������� ������������ ���������� (��. �������� �������) (������� ��������� �� ����� ������ � ���������,
	// ������� ����� �������������� ��� ������������� � ������������ �������)
	processing_config_dev_init();
	// ���� ���������� ���������� � ������ "������������ �� ���������", ����� � ����� ������ ������� �-��� ������������
	// ��� ������� ��� ����, ����� ��� ���������� ������������, ����� ���������� ���������� � ������
	// "������������ �� ���������", ������������ �������� �-��� ������������, � �� �-��� �� ��������� (�-���
	// �� ��������� �������� �� ������������ �� ����������).
	if(!STATE_JAMPER1){ // ���� ������� �1 ���������� (��� 0 �� �����) - ������� ������������ ������������
		INIT_MBB_read_addjust_table(s_mem_map.p_start_config_data,sizeof(S_global_config), PAGE_USER_CONFIG);
	}

	//Run HID (usb)
	USB_HIDInit();

	/*******************************************************************************/
	//---------------�������� ������ ���������� �������� ��������--------------------
	//*******************************************************************************/
	// �������� ���������������� ������ ��������� ������ � ����������
	modbus_fill_S_connectmodbus((u8*)&s_config_moduls.s_connectmodbus_global);
	modbus_fill_S_modbus_req((u8*)&s_config_moduls.s_ulist_modbus);
	u8 count_modbus=0;
	for(k1=0;k1<NUMBER_USART;k1++){

		// ���� � ������������ �������� ���� �������, �������� ��������� modbus
		if(MODBUS_USART & (1<<k1)){

			if(!(s_config_moduls.s_connectmodbus_global[count_modbus].state)){// ���� � ������������ �������� ���� ��������
				count_modbus++;
				continue;
			}
			// 	���������� ���������� ����� USART
			ConfigureUSART(&P_SHIFT(ps_connectmodbus_global,count_modbus)->s_port_config, present_usart[k1]); // ���������� USART
			// �������� ��������� ������
			s_modbus_tsk_par[count_modbus].num_modbus=count_modbus;
			s_modbus_tsk_par[count_modbus].ps_task_parameters=ptask_parameters+k1;
			// �������� ������ ��� �������� �����
			if(s_config_moduls.s_connectmodbus_global[count_modbus].type==PROTOCOL_MODBUS_MASTER){
				xTaskCreate( t_Modbus_MASTER, ( const char * ) "Modbus_MASTER", 230, (void*)(&s_modbus_tsk_par[count_modbus]), mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );
			}
			else if(s_config_moduls.s_connectmodbus_global[count_modbus].type==PROTOCOL_MODBUS_SLAVE){
				xTaskCreate( t_Modbus_SLAVE, ( const char * ) "Modbus_SLAVE", 230, (void*)(&s_modbus_tsk_par[count_modbus]), mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );
			}
			count_modbus++;
		};
	}
	//------------------------------------------------------------------------------------------------------------------------------------

	/*******************************************************************************/
	//---------------�������� ������ ���������� �������� �������� ��������----------/
	/*******************************************************************************/

#ifdef DEV_0
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_0).state){// ���� � ������������ �������� ���� ��������
		xTaskCreate(  TASK_PROCESSING(DEV_0), ( const char * ) TASK_IDENT(DEV_1), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_0), TASK_PRIORITY(DEV_0), NULL );
	}
#endif

#ifdef DEV_1
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_1).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_1), ( const char * ) TASK_IDENT(DEV_1), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_1), TASK_PRIORITY(DEV_1), NULL );
	}
#endif

#ifdef DEV_2
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_2).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_2), ( const char * ) TASK_IDENT(DEV_2), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_2), TASK_PRIORITY(DEV_2), NULL );
	}
#endif

#ifdef DEV_3
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_3).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_3), ( const char * ) TASK_IDENT(DEV_3), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_3), TASK_PRIORITY(DEV_3), NULL );
	}
#endif

#ifdef DEV_4
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_4).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_4), ( const char * ) TASK_IDENT(DEV_4), 700,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_4), TASK_PRIORITY(DEV_4), NULL );
	}
#endif

#ifdef DEV_5
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_5).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_5), ( const char * ) TASK_IDENT(DEV_5), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_5), TASK_PRIORITY(DEV_5), NULL );
	}
#endif

#ifdef DEV_6 // FRQ processing
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_6).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_6), ( const char * ) TASK_IDENT(DEV_6), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_6), TASK_PRIORITY(DEV_6), NULL );
	}
#endif

#ifdef DEV_7 // GPS processing
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_7).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_7), ( const char * ) TASK_IDENT(DEV_7), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_7), TASK_PRIORITY(DEV_7), NULL );
	}
#endif

#ifdef DEV_8 // sensor processing
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_8).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_8), ( const char * ) TASK_IDENT(DEV_8), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_8), TASK_PRIORITY(DEV_8), NULL );
	}
#endif

#ifdef DEV_9 // display processing
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_9).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_9), ( const char * ) TASK_IDENT(DEV_9), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_9), TASK_PRIORITY(DEV_9), NULL );
	}
#endif

#ifdef DEV_10
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_10).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_10), ( const char * ) TASK_IDENT(DEV_10), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_10), TASK_PRIORITY(DEV_10), NULL );
	}
#endif

#ifdef DEV_11
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_11).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_11), ( const char * ) TASK_IDENT(DEV_11), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_11), TASK_PRIORITY(DEV_11), NULL );
	}
#endif

#ifdef DEV_12
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_12).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_12), ( const char * ) TASK_IDENT(DEV_12), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_12), TASK_PRIORITY(DEV_12), NULL );
	}
#endif

#ifdef DEV_13
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_13).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_13), ( const char * ) TASK_IDENT(DEV_13), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_13), TASK_PRIORITY(DEV_13), NULL );
	}
#endif

#ifdef DEV_14
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_14).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_14), ( const char * ) TASK_IDENT(DEV_14), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_14), TASK_PRIORITY(DEV_14), NULL );
	}
#endif

#ifdef DEV_15
#if (TASK_PRIORITY(DEV_1)<MIN_T_PRIORITY)||(TASK_PRIORITY(DEV_1)>MAX_T_PRIORITY)
#error  Inavalide task DEV_1 priopity (Ger)
#endif
	if(s_config_moduls.USER_CONFIG_FIELD(s,DEV_15).state){// ���� � ������������ �������� ������ ��������
		xTaskCreate(  TASK_PROCESSING(DEV_15), ( const char * ) TASK_IDENT(DEV_15), 100,(void *)&s_config_moduls.USER_CONFIG_FIELD(s,DEV_15), TASK_PRIORITY(DEV_15), NULL );
	}
#endif

// ������ ���������� �������� ���������� ����������� � �������� ��������� ������ ������
	//xTaskCreate(  t_processing_reset_control, ( const char * ) "WatcDogTask", 70,	NULL, 4, NULL );

	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	return;
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that 
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
