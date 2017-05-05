/*
 * P_watchdog.c
 *
 *  Created on: Jun 2, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */
#include "processing_reset_control.h"

// ��������� ������� ����������� ��������� (�� processing_mem_map.c)
extern S_address_oper_data s_address_oper_data;
extern S_mem_map s_mem_map;
// ������� ��� ��������� ��������� ������ ��������� ��������
xSemaphoreHandle SemaphoreHandle_watchdog;
// �-�� ������ �������� ������ ������� ���� ��� ������������
u16 count_reset=MAX_TIME_RESET/PERIOD_TASK;
// ������� �-�� ������ ����� ������ ������� ���� �� ������������
BF_processing_reset_control_ bf_counter_reset_control={0,0};
// ����� �������� device_rcr;
u16 data_reset_reg;
//��������� ������� ���������
S_mode_blink s_mode_blink={
		MOD_1_ON*10/PERIOD_TASK,MOD_1_OFF*10/PERIOD_TASK,
		MOD_2_ON*10/PERIOD_TASK,MOD_2_OFF*10/PERIOD_TASK,
		MOD_3_ON*10/PERIOD_TASK,MOD_3_OFF*10/PERIOD_TASK,
		0,WORK_OK
};
// ������������ ����� ��� ��������� ����� �������
u8 a_bit_status[MAX_NUM_MODULS]={[0 ... (MAX_NUM_MODULS-1)]=0};

//-------------p_watchdog_init------------------
//������� p_watchdog_init - ��������� ������������ watchdog
// ������� ���������:
// prescale               - �������� ������� LSI �������� watchdog,
// prelode                - �������� ����� watchdog (���� 4095)
u8 p_watchdog_init(WATCHDOG_PRESCALER prescale, u16 prelode ){
	if(prelode>0x0FFF){return 1;}
	// �������� ��������� ��������������� ��������� LSI
	RCC_LSICmd(ENABLE);
	// ������� LSI ��� �������� ������������ IWD
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	// ������������ WATCHDOG. �������� ��������� ������� LSI, ������� - 40 ���
	// �������� ������ � ��������� PR � RLR
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(prescale);  // ������������ �������������
	IWDG_SetReload(prelode); // ������������ ��������������� �������� ����� (WATCHDOG ������� ���� �� ����� �����)
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable); // �������� ������ � ��������� PR � RLR
	return 0;
}


//-------------p_watchdog_enable------------------
//������� p_watchdog_enable - �������� watchdog
void p_watchdog_enable(void){
	IWDG_Enable();
};


//-------------p_watchdog_reset------------------
//������� p_watchdog_reset - ���������� watchdog
void p_watchdog_reset(void){
	IWDG_ReloadCounter();
}

void p_watchdog_set_flag(void){
	xSemaphoreTake(SemaphoreHandle_watchdog, portMAX_DELAY);
	xSemaphoreGive(SemaphoreHandle_watchdog);
}

//------------������� processing_reset_control_single_registr-----------------------------------
// ������� processing_reset_control_single_registr - ��������� �������� ������� ����� ������ �� �������������� ��������� ������������ ����������
// ����� ������� ������������� �������� ������� �6 modbus
// ������� ���������:
// *p_check_address - ��������� �� ������ ������������������ (u16)address, (u16)num_reg
// �������� ���������:
// REQ_SLAVE_OK - �������� ��������� �������
// ILLEGAL_DATA_ADRESS - ������������ �����
static REZ_REQ_CHEACK_SLAVE processing_reset_control_single_registr(void* p_check_address){
	if(s_address_oper_data.s_address_devise_reset_status.address_device_rcr!=*((u16*)p_check_address)){
		return ILLEGAL_DATA_ADRESS;
	}
	return REQ_SLAVE_OK;
}


//------------������� processing_config_set_dev_error-----------------------------------
// ������� processing_config_set_dev_error - ��������� ��������� ���������� ���� ���������� � ���������� ������ �������� ����������
//                                           � �������� ��������� ������������
// ������� ���������:
//num_bit - ����� ���� ������� ����� ����������
void processing_reset_control_set_dev_error(u8 num_bit, FlagStatus state){
	u16 state_status_reg, counter;
	// ������ ���������� ����� ���� ������, ������� ������ ������ �������
	for(counter=0;counter<MAX_NUM_MODULS;counter++){
		if(a_bit_status[counter]==num_bit){break;}
	}
	// �������� �������� ������ ����������
	processing_mem_map_read_s_proces_object_modbus(&state_status_reg,1, s_address_oper_data.s_address_devise_reset_status.address_device_statys);
	//���������� ������-��� � ���������� ������ ��������
	if(state==SET){
		SET_BIT_(state_status_reg,counter);
	}
	//�������� ������-��� � ���������� ������ ��������
	else if(state==RESET){
		CLEAR_BIT_(state_status_reg,counter);
	}
	// ��������� ����������� ������ ������� ����������
	processing_mem_map_write_s_proces_object_modbus(&state_status_reg,1, s_address_oper_data.s_address_devise_reset_status.address_device_statys);
	if(state_status_reg){
		s_mode_blink.work_mode=ERROR_MODE;
	}
	else{
		s_mode_blink.work_mode=WORK_OK;
	}
}


//--------������� processing_reset_control_reset_control--------------
//������� processing_reset_control_reset_control - ��������� ���������� ���������� �������������
//(������ �� ���������� �������� ������������, � ��� ���������� ������� ������������ -
// ���������� ����������)
// �������� ������������:
// 1) �������� � ������� �� ������ address_device_rcr ����� RESET_FIREST_STEP,
// 2) �� ���������� ������� �� ����� MAX_TIME_RESET ��  �������� � ������� ��
//    ������ address_device_rcr ����� RESET_SECOND_STEP
static void processing_reset_control_reset_control(void){
	u16 zero=0;
	// �������� ������� ���������� �������������
	processing_mem_map_read_s_proces_object_modbus(&data_reset_reg,1,s_address_oper_data.s_address_devise_reset_status.address_device_rcr);
	// ���������� �������� ��������:
	if(bf_counter_reset_control.f_rx_1_step){ // ���� ���� ����������, ��� ������ ������ ���
		bf_counter_reset_control.counter++;
		if(data_reset_reg==RESET_SECOND_STEP){ // ���� ������ ������ ��� ������������
			NVIC_SystemReset();// �������� ���������� ������������ ��� ���������� ���������������� ������
			return;
		}
		if(bf_counter_reset_control.counter>count_reset){ // ���� �������� ����� ������� �������� ������� ����
			bf_counter_reset_control.f_rx_1_step=0;
			bf_counter_reset_control.counter=0;
			processing_mem_map_write_s_proces_object_modbus(&zero,1,s_address_oper_data.s_address_devise_reset_status.address_device_rcr);
		}
		else{
			return;
		}
	}
	else{// ���� ���� �� ����������, ������ ��� �� ������
		if(data_reset_reg==RESET_FIREST_STEP){ // ���� ������ ������� ���
			bf_counter_reset_control.f_rx_1_step=1;
			return;
		}
		return;
	}
}


//-------------������� processing_reset_control_blink----------------
// ������� processing_reset_control_blink - ��������� ���������� ����������
// ������ ������ ����������:
// - ���������� ������������� ���������:
// - ������������ ������������ �� ������ �� ���� �������
// - ������ �� ������ �� ���� �������
static void processing_reset_control_blink(void){
	u8 mode=s_mode_blink.work_mode;
	u8 counter_per=s_mode_blink.counter;
	u8 state=s_mode_blink.state_blink;
	if(state){
		if(counter_per>=s_mode_blink.as_blink_par[mode].time_on){
			// ��������� ���������
			IND_ERROR_RESET;
			s_mode_blink.state_blink=0;
			s_mode_blink.counter=0;
			return;
		}
	}
	else{
		if(counter_per>=s_mode_blink.as_blink_par[mode].time_off){
			// �������� ���������
			IND_ERROR_SET;
			s_mode_blink.state_blink=1;
			s_mode_blink.counter=0;
			return;
		}
	}
	s_mode_blink.counter++;
}

//-------------������� init_a_bit_status----------------
// ������� init_a_bit_status - ��������� ������������ ����� ������-����� ���������� ��������� ��������� �������:
// �� �������� ���� ������� ���������� ����������� ������ ������� ����� �������� � ����� ���������� ������� �
// ��������������� ������������ � ����� a_bit_status. ����� ������� ������������ � NUM_PORT_MODBUS (������ NUM_PORT_MODBUS
// ����� - ��� ���� ������ MODBUS)
void init_a_bit_status(void){
	u8 counter_bit, counter_dev;
	u16 *p_dev_staff=(u16*)&((S_global_config*)s_mem_map.p_start_config_data)->s_dev_staff.bf_dev_staff;
	// ���� � ������� ���������� ������������ MODBUS (������ ��� ��������� ���� ������� ����������), ������ ������ �������
	// ������ ������ - ����� ����� ������� �����
	counter_dev=0;
	counter_bit=0;
	if(((0b1)& (*p_dev_staff))||((0b1000000000000000)& (*p_dev_staff)))
	{
		for(counter_bit=0;counter_bit<NUM_PORT_MODBUS;counter_bit++)
			{
			a_bit_status[counter_bit]=counter_bit;
			}
		counter_bit=1;
		counter_dev=NUM_PORT_MODBUS;
	}
	// �������� ������� ������ - ����� ������ ������������ �������
	// �� counter_bit=1 - ������ ��� �� ������ ����� �������  MODBUS (����������������� ����)
	// �� MAX_NUM_MODUL-1 - ������ ��� �� ��������� ����� ������� MODBUS (����������������� ����)
	for(;counter_bit<(MAX_NUM_MODUL-1);counter_bit++)
	{
		if((1<<counter_bit)& (*p_dev_staff)){
			a_bit_status[counter_dev]=counter_bit+NUM_PORT_MODBUS;
			counter_dev++;
		}
	}
}


//---------------������ t_processing_watchdog---------------
// ������ t_processing_watchdog - ��������� ��������� � ��������� ����������� �������
void t_processing_reset_control(void* in_Data){
	// ����� �-� callback ������� ���������� ��������� modbus_slave ��� �������� �������������� ������� �������� �6
	modbus_callback_address_check(&processing_reset_control_single_registr,PRESET_SINGLE_REGISTERS);
	// �������� ������������ ����� ������-����� ����������
	init_a_bit_status();
	//������ ������� ������� � ������� ��������� ��������
	vSemaphoreCreateBinary(SemaphoreHandle_watchdog);
	// ������������� ���������� ������ (watchdog)
	p_watchdog_init(WDG_PRESCALER_32,4000);
	p_watchdog_enable();
	while(1){
		//xSemaphoreTake(SemaphoreHandle_watchdog, portMAX_DELAY);
		// ���������� ��������� �����
		//xSemaphoreGive(SemaphoreHandle_watchdog);
		vTaskDelay(PERIOD_TASK);
		// �������� ���������� �������� ���������
		processing_reset_control_blink();
		//�������� ���������� �������� ������������
		processing_reset_control_reset_control();
		// ��������� ���������� ������
		p_watchdog_reset();
	}
}
