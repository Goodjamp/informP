/*
 * processing_reset_control.h
 *  *
 *  Created on: December 2, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_rcc.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "processing_simple_gpio.h"
#include "processing_mem_map_extern.h"
#include "processing_config_dev.h"
#include "funct.h"

#ifndef P_WATCHDOG_H_
#define P_WATCHDOG_H_

// ������ �������� ������ ��� ������������
#define RESET_FIREST_STEP 0x55FF
// ������ �������� ������ ��� ������������
#define RESET_SECOND_STEP 0xFF55
// ����������� - ���������� ����� ����� �������� �������� ������������, ��
#define MAX_TIME_RESET    10000
// ������ ��������� ������ t_processing_reset_control
#define  PERIOD_TASK 50


//****************************************************************
//--------------- ��������� ������� ���������--------------------*
//****************************************************************
// �-�� ������� ����������
#define  NUM_MOD_IND  3
// ����� �1
// ����� ���������� ���������, *10��
#define  MOD_1_ON  200
// ����� ����������� ���������, *10��
#define  MOD_1_OFF 200
// ����� �2
// ����� ���������� ���������, *10��
#define  MOD_2_ON  10
// ����� ����������� ���������, *10��
#define  MOD_2_OFF 10
// ����� �3
// ����� ���������� ���������, *10��
#define  MOD_3_ON  20
// ����� ����������� ���������, *10��
#define  MOD_3_OFF 20

//������ ��� ��������������� ������ �-� ��������� ���� ������� � ���������� ������-��������
#define SET_GLOBAL_STATUS(X)   processing_reset_control_set_dev_error(X##_CONTROL_BIT,SET);
//������ ��� ��������������� ������ �-� ������� ���� ������� � ���������� ������-��������
#define RESET_GLOBAL_STATUS(X)   processing_reset_control_set_dev_error(X##_CONTROL_BIT,RESET);

// ��������� �������� ������������
typedef enum{
	WDG_PRESCALER_4=0x00,
	WDG_PRESCALER_8=0x01,
	WDG_PRESCALER_16=0x02,
	WDG_PRESCALER_32=0x03,
	WDG_PRESCALER_64=0x04,
	WDG_PRESCALER_128=0x05,
	WDG_PRESCALER_256=0x06
}WATCHDOG_PRESCALER;

// ������ ������ ����������
typedef enum{
	WORK_OK=0,
	ERROR_MODE=1
}WORK_MODE;

#pragma pack(push, 1)

typedef struct{
	u16 f_rx_1_step:1;
	u16 counter: 15;
}BF_processing_reset_control_;

typedef struct{
	u8 time_on;
	u8 time_off;
}S_blink_par;

typedef struct{
	S_blink_par as_blink_par[NUM_MOD_IND];
	u8 counter;          // ������� �-�� ������ � ������� ��������� �� ���������� ����� ���������
	u8 state_blink: 1;   // � ����� ��������� ���������� ��������� � ������ ������ 1/0 MOD_�_ON/MOD_�_OFF
	u8 work_mode: 7;
}S_mode_blink;


#pragma pack(pop)

void p_watchdog_enable(void);
void p_watchdog_reset(void);
u8 p_watchdog_init(WATCHDOG_PRESCALER prescale, u16 prelode);
void processing_reset_control_set_dev_error(u8 num_bit, FlagStatus state);
void t_processing_reset_control(void* in_Data);


#endif
