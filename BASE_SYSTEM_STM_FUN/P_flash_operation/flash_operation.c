/*
 * flash_operation.c
 *
 *  Created on: December 25, 2014
 *      Author: Gerasimchuk
 *      Versin: 1
 */
 
#include "flash_operation.h"


//------- ������� FLASH_OPERATION_read_flash_8b -------------
// ������� FLASH_OPERATION_read_flash_8b - ���������� �������� �-�� 8-�� ������� ����� �� flash-������
// ������� ���������:
//                 *p_buff - ��������� �� ������ ��������� 8-�� ������ ��������
//                  length - �-�� ������ � ����������
//                 address - ��������� ����� ����������� ������
void FLASH_OPERATION_read_flash_8b(u8 *p_buff, u32 length, u32 address) {
	u32 k1;

	for (k1 = 0; k1 < length; k1++) {
		(*p_buff) = (*(__IO u8*) address);
		address += 1;
		p_buff++;
	}
	//state_read_write=DATA_NON;
}


//------- ������� FLASH_OPERATION_read_flash_16b -------------
// ������� FLASH_OPERATION_read_flash_16b - ���������� �������� �-�� 8-�� ������� ����� �� flash-������
// ������� ���������:
//                 *p_buff - ��������� �� ������ ��������� 16-�� ������ ��������
//                  length - �-�� ������ � ����������
//                 address - ��������� ����� ����������� ������
void FLASH_OPERATION_read_flash_16b(u16 *p_buff, u32 length, u32 address) {
	u32 k1;

	for (k1 = 0; k1 < length; k1++) {
		(*p_buff) = (*(__IO u16*) address);
		address += 2;
		p_buff++;
	}
	//state_read_write=DATA_NON;
}

//------- ������� FLASH_OPERATION_write_flash_16b -------------
// ������� FLASH_OPERATION_write_flashb - ������ � flash-������ �������� �-�� 16-�� ������ ��������,
//                                 ������ ������������ ������ � ������ ��������. ����� ������� �������� ����������.
// ������� ���������:
//                 *data - ��������� �� ������������ ������
//                  length - �-�� ������ � ������
//                  num_page - ����� �������� ������, ��������� �������� 0-127
FLASH_Status FLASH_OPERATION_write_flash_page_16b(u16 *data, u32 length, u16 num_page) {
	FLASH_Status rez_write;
	u32 address;
	u32 k1;
	address = PAGE_ABS_ADDRESS(num_page);
	FLASH_Unlock();
	FLASH_ErasePage(address);
	for (k1 = 0; k1 < length; k1++) {
		rez_write = FLASH_ProgramHalfWord(address, (*data));
		data++;
		address += 2;
	}
	FLASH_Lock();
	return rez_write;
}
 

//------- ������� FLASH_OPERATION_write_flash_16b -------------
// ������� FLASH_OPERATION_write_flashb - ������ � flash-������ �������� �-�� 16-�� ������ ��������
// ������� ���������:
//                 *data - ��������� �� ������������ ������
//                  length - �-�� ������ � ������
//                 address - ��������� ����� ������, ��������� �������� START_MEM_ADRESS - STOP_MEM_ADRESS
u32 k1_count;
FLASH_Status FLASH_OPERATION_write_flash_16b(u16 *data, u32 length, u32 address) {
	FLASH_Status rez_write;
	FLASH_Unlock();
	for (k1_count = 0; k1_count < length; k1_count ++) {
		rez_write = FLASH_ProgramHalfWord(address, (*data));
		data++;
		address += 2;
	}
	FLASH_Lock();
	return rez_write;
}


//------- ������� FLASH_OPERATION_erase_page -------------
// ������� FLASH_OPERATION_erase_page - �������� ��������
// ������� ���������:
//                 num_page  - ����� �������� ������, ��������� �������� 0-127
void FLASH_OPERATION_erase_page(u16 num_page) {
	u32 address;
	address = PAGE_ABS_ADDRESS(num_page);
	FLASH_Unlock();
	FLASH_ErasePage(address);
	FLASH_Lock();
}

