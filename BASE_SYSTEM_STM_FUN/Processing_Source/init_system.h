/*
 * init_system.h
 *
 *  Created on: December 25, 2014
 *      Author: Gerasimchuk
 *      Versin: 1
 */
#ifndef INIT_SYSTEM_H_
#define INIT_SYSTEM_H_

#include "stm32f10x.h"
#include "flash_operation.h"
#include "core_cm3.h"

//----------------------------------------------------------------------------------------------------------------------
//    ***********************************  ���������������� ��������� �����������  ******************************
//----------------------------------------------------------------------------------------------------------------------
// ����� ������ � ������ ������� ����� � ������� ����������� ������ � ��������� (������ ������������� ������ ������ !!!!)
#define PAGE_DEFAULT_CONFIG             126
#define PAGE_USER_CONFIG                127
#define START_ADDRESS_DATA PAGE(PAGE_USER_CONFIG )
#define START_ADDRESS_CONFIG_DATA START_ADDRESS_DATA+4

// -----------��������� ������������� ��� �� ����������� ���������� �� ����
typedef enum {
	MBB_INIT_OK=0,
	MBB_INIT_ERROR=1
} INIT_MBB_Rezult;


// ----------------��������� ������� ����� ������������------------------
void INIT_MBB_read_addjust_table(u8 *p_read_array, u16 size_read_array,u8 num_read_page);

#endif
