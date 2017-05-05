/*
 * processing_mem_map.h
 *
 *  Created on: December 29, 2014
 *      Author: Gerasimchuk
 *      Versin: 1
 */
#ifndef PROCESSING_MEM_MAP_H_
#define PROCESSING_MEM_MAP_H_

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f10x_usart.h"

//***********************************  ���������������� ��������� �����������  ******************************
//--------------------------------------------------������----------------------------------------------------------

//--------����� ����� �-�� � ��������� USART-��� � ������������ ��---------
// ����� ����������� ������ ���� ������� ������� ���� ����� (� �� � processing_USART.h),
// ��� ��� processing_USART.h - ��� ����� ��������, � � ���� ������ ������� ���������������� �������� ������������
//�-�� USART
#define	NUMBER_USART		3
//������ USART
#define	PRESENT_USART     USART1,\
	                      USART2,\
	                      USART3\

//---����� �-�� � ������� USART ������� ������������� ��� ��������/������ ������ � ��������� MODBUS---------
// �-�� ������ �� ������� �������� ������������� ��������
#define NUM_PORT_MODBUS     1
//������ USART-���
#define MODBUS_USART        (WORKED_USART1)

// ����������� - ��������� �-�� ������������ �������������� ����������� �������
#define MAX_NUM_MODULS     NUM_PORT_MODBUS+3

//����� ���������� ��������� �����/������ ������ ����������
#define NUM_REG_DATA              0

//--------------------------------------------------�����----------------------------------------------------------
//***********************************  ���������������� ��������� �����������  ******************************

// ������������� (������� ������������) ����� ������ ������� ��������� �����/������ ������ ����������
#define USER_ADDRESS_DATA         0
// ������������� (������� ������������) ����� ������ ������� ����������� ���������
#define USER_ADDRESS_OPER_DATA    500
// ������������� (������� ������������) ����� ������ ������� ��������
#define USER_ADDRESS_CONFIG_DATA  1000


// ������ �������� proces_object
#define SIZE_PROCES_OBJECT        sizeof(S_proces_object_modbus)
// ������ ���� DATA ��������� S_proces_object_modbus
#define MAX_DATA_OBJ_MODBUS					0x02




// ��������������� �������
#define PPROCOBJ_2DATA(x)        *(S_proces_object_modbus*)(x)
#define POINT_TO_USER_ADDRESS(X) ((u16)X)/sizeof(S_proces_object_modbus)
//+USER_ADDRESS_OPER_DATA


//------�������� ����������� ����� ������-----------------------
// ��������� ��������� � ����� ������
typedef enum{
	MEM_ERROR,
	MEM_OK
}ERROR_mem;

// ���� �������������� ��������� �� ���� ������
typedef enum{
	ERROR_DATA=0,
	OPER_DATA=1,
	PMZ_DATA=2,
	CONFIG_DATA=3
}TYPE_REG_MEM_MAP;



//----------�������� ����� ���������� �� �-�--------------------------
typedef ERROR_mem (*P_mem_map_processing_check_address)(void*);


//------�������� �������������������� ��������-----------------------

#pragma pack(push, 1)

// ������� ���������� ����� ������ (������ ����� ��� ���� (��, ��, ��) ����� ������)
typedef struct
{
	unsigned char date[MAX_DATA_OBJ_MODBUS];
	//unsigned char flag;
} S_proces_object_modbus;


// ������������ ����� ������
typedef struct{
	u16 size_mem_map;  // ������ ����� ������= ���� �������� ������ (�������� �����/������), ������ (����������� ��������) � ������ (������������) ������ ����� ������
	u8 *ps_proces_object_mem_map; // ��������� �� ������ ����� ������
	u8 *p_start_DATA;             // ����� ������ ������������� ����� ����� ������
	u8 *p_start_oper_data;        // ����� ������ ����������� ���������
	u8 *p_start_config_data;      // ����� ������ ������ ������������
} S_mem_map;



#pragma pack(pop)
//--------------------------------------------------------------------------------------------------------------------------------

void processing_mem_map_init(void);
u8 processing_mem_map_init_mem_map(void);
ERROR_mem processing_mem_map_write_s_proces_object_modbus( const u16 *p_data,const u16 num_data, const u16 address);
ERROR_mem processing_mem_map_read_s_proces_object_modbus( u16 *p_data, u16 num_data,u16 address);
TYPE_REG_MEM_MAP processing_mem_map_detect(u16 data_user_addres, u8 **p_addres_mem_map);

#endif
