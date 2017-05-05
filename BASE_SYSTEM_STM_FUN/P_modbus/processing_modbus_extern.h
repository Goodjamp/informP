/*
 * processing_modbus_extern.h
 *
 *  Created on: May 26, 2015
 *      Author: Gerasimchuk Alexandr
 */
#ifndef P_MODBUS_EXTERN_H_
#define P_MODBUS_EXTERN_H_

#include "stm32f10x.h"
#include "processing_mem_map.h"
#include "processing_USART.h"




// ������������ ���������� �������� modbus �� ����� �����
#define MAX_NUM_MODBUS_REQ       2


// -----���������� ������� MODBUS_(MASTER/SLAVE)--------------------------------
typedef enum{
	READ_COIL_STATUS=0x01,
	READ_INPUT_STATUS=0x02,
	READ_HOLDING_STATUS=0x03,
	READ_INPUT_REGISTERS=0x04,
	FORCE_SINGLE_COIL=0x05,
	PRESET_SINGLE_REGISTERS=0x06,
	FORSE_MULTIPLE_COILS=0x0f,
	PRESET_MULTIPLE_REGISTERS=0x10
}MODBUS_FUNCTION;

//------------���������� �������� ������� �����-----------------
typedef enum{
	REQ_SLAVE_OK=0,
	ILLEGAL_FUNCTION=1,
	ILLEGAL_DATA_ADRESS=2,
	ILLEGAL_DATA_VALUE=3,
	SLAVE_DEVICE_FALIURE=4,
	ACKNOWLEDGE=5,
	SLAVE_DEVICE_BUSY=6,
	NEGATIVE_ACKNOWLEDGE=7,
	MEMORY_PARITY_ERROR=8,
	REQ_SLAVE_ERROR=9
}REZ_REQ_CHEACK_SLAVE;


// -----------�������� ������ �����--------------------
typedef enum {
	PROTOCOL_MODBUS_MASTER,
	PROTOCOL_MODBUS_SLAVE,
} PORT_PROTOCOL;

#pragma pack(push, 1)

// ������ ����������� ��������� �������
typedef struct{
	u16 status_PORT;
} S_modbus_address;

// ����������� �������� ������
typedef struct{
	S_proces_object_modbus status_PORT[NUM_PORT_MODBUS];
} S_modbus_oper_data;


// ��������� ������ �������
typedef struct {
	u16	pribor;         // ??
	u8	adress;         // ����� ��
	u8	function;       // ������� ������
	u16	start_adres;    // ���� ����� ������
	u16	number;         // �-�� ������ � ������
	u16	adress_pmz;     //����� � ����� ������ ���� ����� ������ �������� ������� (������ "pmz", ��� �����.)
	u16	adres_status;   // ����� � ������� ������ ������� �������
}S_ulist_modbus;


// ��������� �������� Modbus Master|Slave
typedef struct {
	u16 state;         // ��������� ����������� ������: ENABLE/DISABLE
	S_port_config s_port_config;   //
	PORT_PROTOCOL type;            // (A)    �������� ��� �����

	//---��������� Master (���������� ��� ���� �������� �� ����� �����)

	u16 waitresp;                  // (M)    ����� �������� ������
	//unsigned int nextmessage;    // (S)    ������� �� ���������� ������ ������� � �-� ������� ������ ������
	u8 number_of_pribor;           // (M)    ���������� �������� (�������� � ���� �-�� ������ ��������, � �� ��������, � ��� ������ 1)
	u8 number_no_answer;           // (M)    ���������� �-�� ����������� �������� ��� ��������� ������ ������ ����

	//---��������� Slave

	u8 adress_kp;                   // (S)    ����� ����� ��

} S_connectmodbus;

#pragma pack(pop)


#endif /* MODBUS_H_ */
