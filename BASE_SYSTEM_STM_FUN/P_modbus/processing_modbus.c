/*
 * modbus.c
 *
 *  Created on: Apr 10, 2012
 *      Author: Vitaliy Tikhenko
 */

#include <stddef.h>
#include "processing_modbus.h"
#include "processing_mem_map_extern.h"

extern S_proces_object_modbus *p_proces_object;
// ��������� ����������� ��������� (�� mem_map_processing.c)
//extern S_oper_data *ps_oper_data;
// ��������� ������� ����������� ��������� (�� mem_map_processing.c)
extern S_address_oper_data s_address_oper_data;
// ����� ���������� �� ������� �������� ���������� �������, (�� mem_map_processing.c)
extern P_mem_map_processing_check_address p_mem_map_processing_check[8];
//����� ����
const u16 ZERO=0;
// ����� �������� �������� ������� �����
S_connectmodbus *ps_connectmodbus_global; //[NUM_PORT_MODBUS];
// ��������� �������� �������� modbas_master  ��� ������� �� ������ (���������� ����� ����� - (������ ������+1 ))
S_list_madbus_req s_list_madbus_req[NUM_PORT_MODBUS];
// ����� ���������� �� ������� �������� ��������� ��������
PF_procesing_req_slave p_procesing_slave[17] = { &func_0102, &func_0102,
		&func_0304, &func_0304, &func_05, &func_06, 0, 0, 0, 0, 0, 0, 0, 0, &func_16,
		&func_16 };
//-------------------------

// ����� �������� ��� ������� �������� ������� � ������� (���� callback, ������ ������������)
S_modbus_callback_check s_modbus_check_address_req[16];
// ����� ���������� �� ������� �������������� ��������
S_modbus_callback_check s_modbus_check_add_check[16];
// ����� ���������� �� ������� ����������� �������� ������
S_add_procesing_req s_modbus_add_procesing_req[16];

//------------������� INIT_MBB_fill_S_connectmodbus---------------
// ������� INIT_MBB_fill_S_connectmodbus - ��������� ��������� ��������� ������
u8 modbus_fill_S_connectmodbus(u8 *p_read_data) {
	u8 k1;
	ps_connectmodbus_global = (S_connectmodbus*) p_read_data;

	for (k1 = 0; k1 < NUM_PORT_MODBUS; k1++) {
		if (!((ps_connectmodbus_global + k1)->state)) {
			continue;
		} // ���� ���� ��������, �������� � ������� ���������� �����
		//������ ������������ �����
	}

	return MBB_INIT_OK;
}

// -----------�������  INIT_MBB_fill_S_modbus_master------------
// �������  INIT_MBB_fill_S_modbus_master - ���������� �������� ��������� �������� modbus_master
INIT_MBB_Rezult modbus_fill_S_modbus_req(u8* p_req_config_data) {
	u8 k1, k2;
	S_ulist_modbus* ps_ulist_modbus = (S_ulist_modbus*) p_req_config_data;
	for (k1 = 0; k1 < NUM_PORT_MODBUS; k1++) {
		for (k2 = 0; k2 < MAX_NUM_MODBUS_REQ; k2++) {
			s_list_madbus_req[k1].s_ulist_req_modbus[k2].ps_ulist_modbus =	ps_ulist_modbus;
			ps_ulist_modbus++;
		}
	}

	return MBB_INIT_OK;
}

//-------------------------END CONST-------------------------------------------------
//----------------------MODBUS MASTER---------------------------------------------------------------------------
//-----������� Put_MODBUS--------------------------------
// ������� Put_MODBUS - ��������� ��������� ������, ��������� �������� ������ � ������ ��������
// ������� ���������:
//
//�������� ���������:
//
u16 bit_value;
u8 Put_MODBUS(u8 *buf_in, S_connectmodbus *s_connectmodbus_port,S_ulist_req_modbus *ps_ulist_req_modbus, u16 status) { // �����  ��� �������� ������ ������ ������, ��� ���������� ������ ��������
	u16 *p_payload;
	u8 counter;

	u16 rez_status;
	if (ps_ulist_req_modbus->rez_zapros) {
		ps_ulist_req_modbus->non_req_count++; // ���������� ������� �������� ��� �������
		if (ps_ulist_req_modbus->non_req_count >= s_connectmodbus_port->number_no_answer) //���� �-�� �������� ��� ������ ��������� ���������� - ��������� ��������������� ������
		{
			//rez_status = (1<<ps_ulist_req_modbus->rez_zapros);
			rez_status = (ps_ulist_req_modbus->rez_zapros);
			//��������� ������ �������
			processing_mem_map_write_s_proces_object_modbus(&rez_status, 1,	ps_ulist_req_modbus->ps_ulist_modbus->adres_status);
		}
		return 1;
	}
	// ���� �������������� ������ ��� �� ����������� �������� (slave ����� ��������) ������ ��� �������� ������
	if(status){
		ps_ulist_req_modbus->non_req_count=0;
		processing_mem_map_write_s_proces_object_modbus(&ZERO, 1,	ps_ulist_req_modbus->ps_ulist_modbus->adres_status);
	}
	//--------------���� ����� ������------------------------------------------------
	ps_ulist_req_modbus->non_req_count = 0; // ������� ������� �������� ��� ������� ��� ������� �������
	//-------------------------------------------------------------------------------
	p_payload=(u16*) &buf_in[3];  //��������� �� ������ �������� ������
	// �������� �������� �������� �������� ���������� �������
	switch (ps_ulist_req_modbus->ps_ulist_modbus->function) {
	case (READ_COIL_STATUS): // ���� ���������� ������� 1 ��� 2
	case (READ_INPUT_STATUS):
		for(counter=0;counter<ps_ulist_req_modbus->ps_ulist_modbus->number;counter++){
			bit_value=(u16)get_array_bit((u8*)p_payload,counter);
			processing_mem_map_write_s_proces_object_modbus((u16*)&bit_value,(u16)1,ps_ulist_req_modbus->ps_ulist_modbus->adress_pmz+counter);
		}
	break;
	case (READ_HOLDING_STATUS): // ���� ���������� ������� 3 ��� 4
	case (READ_INPUT_REGISTERS):
	// ����� ������� �����: Hi->Lo, Lo-Hi
		for(counter=0;counter<ps_ulist_req_modbus->ps_ulist_modbus->number;counter++){
			Convert_ShortTooCharHiLo((*p_payload),(u8*)p_payload);
			p_payload++;
		}
		processing_mem_map_write_s_proces_object_modbus((u16*) &buf_in[3],ps_ulist_req_modbus->ps_ulist_modbus->number,	ps_ulist_req_modbus->ps_ulist_modbus->adress_pmz);
	break;
	default:
		break;
	};
	return 0;
}

//-----------������� ModMasterResTrs---------------------
// ������� ModMasterResTrs - ��������� �������� ������� ������������� � ������� ������, ����� ������ � ��������� ��������� ��������� �� ������� ������ (�RC)
// ������� ���������:
//                 *ptaskparameters     - ��������� �� ��������� � ��������� ����� � ������� ������������ ������
//                 *buf_in              - ����� ������
//                 *pconnectmodbus      - ��������� ������ ������� � ��������� �������� �����
//                 *ps_ulist_req        - ��������� ��������� �������
// �������� ���������

u16 ModMasterResTrs(S_Task_parameters *ptaskparameters, u8 *buf_in,
		S_connectmodbus *pconnectmodbus, S_ulist_req_modbus *ps_ulist_req) {
	u16 NumberByteResponse;
	u16 NumberRead;
	S_modbus_make_res *ps_modbus_res = (S_modbus_make_res*) buf_in;
	S_modbus_req_write s_modbus_req;
	union {
		u8 rez_u8[2];
		u16 rez_u16;
	} resCRC16;

	//�������� ������
	s_modbus_req.address_dev = ps_ulist_req->ps_ulist_modbus->adress; //����� �����
	s_modbus_req.function = ps_ulist_req->ps_ulist_modbus->function; //����� �������
	Convert_ShortTooCharHiLo(ps_ulist_req->ps_ulist_modbus->start_adres,&s_modbus_req.address_mem_L);
	//����� ������
	Convert_ShortTooCharHiLo(ps_ulist_req->ps_ulist_modbus->number,	&s_modbus_req.forse_data_L);
	// �-�� ���� ������
	modus_add_CRC((u8*) &s_modbus_req, 8); // ����������� � �������� ����������� �����
	// ������� ������
	write_Usart_Buf(ptaskparameters->USARTx, (u8*) &s_modbus_req, 8,ptaskparameters->WrUSART); // ������� ������


	// ���� ������������� ������� 1 ��� 2, ������� ����� � �-�� ������ �������� ��������
	if ((s_modbus_req.function == READ_COIL_STATUS) || (s_modbus_req.function == READ_INPUT_STATUS)) // ���� ����������� ������� 1 ��� 2
	{
		NumberByteResponse = ps_ulist_req->ps_ulist_modbus->number / 8;
		if ((NumberByteResponse * 8) != ps_ulist_req->ps_ulist_modbus->number) {
			NumberByteResponse++;
		}
	}
	else{
		// ���������� ������ �������� �������� ������ � ������ ��� �-� � 3-6
		NumberByteResponse = ps_ulist_req->ps_ulist_modbus->number * 2; // �-�� ���� �������� �������� � ������
	}

	NumberByteResponse += 5; // ����� ����� ������ � ������ � ������ ����� � ����������� �����

	// ������ ����� ������ � ��������� �-�� �������� ����
	//--------!!!!!!!!!!!!!!!!!!!!!!�������� ������ �� ����� ���������� !!!!!!!!!!!!!!!!!!!!!!------------------
	NumberRead= ReadUSART(ptaskparameters->RdUSART, (u8*) ps_modbus_res,NumberByteResponse, pconnectmodbus->waitresp);
	if ((NumberByteResponse!= NumberRead)&&(SIZE_EXCEPTION!= NumberRead)) {// ���� ���������� �-�� ��������� ���� �� ����� �������� ���������, ��� ��������:
		//������
		return REQ_MASTER_ERROR; // ����� - ������
	};

	// �������� ���� ������
	if ((ps_modbus_res->address_dev != ps_ulist_req->ps_ulist_modbus->adress)||
			((ps_modbus_res->function!= ps_ulist_req->ps_ulist_modbus->function)&&
			 (ps_modbus_res->function-0x80!= ps_ulist_req->ps_ulist_modbus->function)) // � ������-���������� ���� "�������" ����������� � ������� "������� + 0�80"
			 ) // ���� � �������� ������ �� ���������� ���� ����� ��� ���� �������
	{
		return REQ_MASTER_FUN_ADDRES_ERROR;
	}

	// ���������� � �������� ����������� ����
	resCRC16.rez_u16 = CRC16((u8*) ps_modbus_res, NumberRead - 2);
	if (resCRC16.rez_u8[1] != *((u8*) ps_modbus_res + NumberRead - 2)&& (resCRC16.rez_u8[0]!= *((u8*) ps_modbus_res + NumberRead - 1))) // ���� ����������� ����  �������
	{
		return REQ_MASTER_CRC_ERROR; // ������ ����������� �����
	}
	//--------------------------------------------
	// �������� �� �����-����������
	if(NumberRead==SIZE_EXCEPTION){
		return ps_modbus_res->un_excep_num.type_exception; // ����� SLAVE - ����������
	}
	return REQ_MASTER_OK; // ����� �� ������ ������
}



void t_Modbus_MASTER(void *p_task_par) {
	u8 k1;
	u16 req_status, port_status;
	u8 number_port, number_req;
	//u16 check_no_answer = 0;
	u8 buf_in[MODBUS_SIZE_BUFF_IN]; // ���������� ������ ������
	S_Task_parameters *ptaskparameters;
	ptaskparameters = ((S_modbus_tsk_par*)p_task_par)->ps_task_parameters;
	number_port =  ((S_modbus_tsk_par*)p_task_par)->num_modbus;
	// �������� ���� ������
	vTaskDelay(500); //������� ������ ������� ���� �����, ��� ��������� ������ � ����� ������
	while (1) {

		for (number_req = 0;number_req< ps_connectmodbus_global[number_port].number_of_pribor;number_req++) //����������� ����� ��������� �����
		{
			// ������� ������ �������
			processing_mem_map_read_s_proces_object_modbus(&req_status, 1,s_list_madbus_req[number_port].s_ulist_req_modbus[number_req].ps_ulist_modbus->adres_status);

			if (req_status && (s_list_madbus_req[number_port].s_ulist_req_modbus[number_req].error_req_counter<TIME_BETWEEN_ERROR_REQ_MASTER)) { // ���� �� ������� ������� ��������� ����������� ������ - ������ �� ����������
				s_list_madbus_req[number_port].s_ulist_req_modbus[number_req].error_req_counter++; // ���������� ������� �-�� ���������
				continue;
			};
			s_list_madbus_req[number_port].s_ulist_req_modbus[number_req].error_req_counter=0;

			Clrinbuf(ptaskparameters, 200); // ������ ������� ����� ������ (����������� - ���� ����� -> ���� ������), ������� � (���� �������� -> ���� ������)

			// ������� ������ � ���������� �����
			// ��������� ������� ��������� � ��������� �������� �������
			s_list_madbus_req[number_port].s_ulist_req_modbus[number_req].rez_zapros = ModMasterResTrs(ptaskparameters, &buf_in[0],	P_SHIFT(ps_connectmodbus_global,number_port),
							&s_list_madbus_req[number_port].s_ulist_req_modbus[number_req]); // ��������� ������, �������, ��� � ������� �����

			Put_MODBUS(&buf_in[0], P_SHIFT(ps_connectmodbus_global,number_port),&s_list_madbus_req[number_port].s_ulist_req_modbus[number_req],req_status);

		}
		// ���������� ������� ���� �������� � �������� ������ ����� (����� �������������� ���� ������ ���� ��������� �� �����)
		port_status = 0xffff;
		for (k1 = 0; k1 < ps_connectmodbus_global[number_port].number_of_pribor; k1++) // ��������� ������� ���� �������� �����
		{
			processing_mem_map_read_s_proces_object_modbus(&req_status, 1,s_list_madbus_req[number_port].s_ulist_req_modbus[k1].ps_ulist_modbus->adres_status);
			port_status = (req_status & port_status);
		}

		// ���� ��� ��������� �� ������ ����� �� ��������, ��������� ������ �����
		if (port_status) {
			processing_mem_map_write_s_proces_object_modbus(&port_status, 1,s_address_oper_data.s_modbus_address.status_PORT + number_port);
		//��������� ������ � ����� ������-������� ��������� ����������
			processing_reset_control_set_dev_error(number_port,SET);
		}
		else{
			processing_mem_map_write_s_proces_object_modbus(&ZERO, 1,s_address_oper_data.s_modbus_address.status_PORT + number_port);
			//��������� ������ � ����� ������-������� ��������� ����������
			processing_reset_control_set_dev_error(number_port,RESET);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------START MODBUS SLAVE------------------------------------
//----------------------------------------------------------------------------
//---------������� MODBUS
// - �. 1 - ������ ��������� ������� �������
// - �. 2 - ������ ��������� ���������� ������
// - �. 3 - ������ ��������� �������� ������ (������������)
// - �. 4 - ������ ������� ��������� ������
// - �. 5 - ��������� ��������� �������
// - �. 6 - ������ ���������� ��������
// - �. 15 - ������ �������
// - �. 16 - ������ ������ ���������

//---------------������� func_0102---------------
// ������� func_0102 - ��������� ����� �� ������ �� ������� 1 � 2, ������� ������������� �������� �� ����� ������ � �������� �������
// ������� ���������:
// S_modbus_req *ps_modbus_req  - ��������� �� ������ � �������� �� �������
// S_modbus_make_res *buf_respons - ��������� �� ������ � ������������� �������
// �������� ���������:
// u16 - �-�� ���� ������ (������ ������)
u16 func_0102(void *req, S_modbus_make_res *buf_respons) {
	S_modbus_req_read *ps_modbus_req = req;
	u8 *p_buf_rez_req = &buf_respons->resp_payload;
	u8 k1;
	u8 coun_TS;
	u8 numTS, num_byte_TS;
	u16 val_TS, address_TS;

	address_TS = CharTooshort(ps_modbus_req->address_mem_L,
			ps_modbus_req->address_mem_H); // �����
	numTS = CharTooshort(ps_modbus_req->num_reg_L, ps_modbus_req->num_reg_H); // �-��
	buf_respons->address_dev = ps_modbus_req->address_dev; // ��������� ����� - ����� ��
	buf_respons->function = ps_modbus_req->function; // ��������� ����� - ����� �������

	// ������������ �-�� ���� ������� �������� ��
	num_byte_TS = numTS / 8;
	if ((num_byte_TS * 8) != numTS) {
		num_byte_TS++;
	}
	//������� ����� �� � ������
	for (k1 = 0; k1 < num_byte_TS; k1++) {
		*(p_buf_rez_req + k1) = 0;
	}
	// �������� ������� ���������� ��
	p_buf_rez_req = &buf_respons->resp_payload;
	*(p_buf_rez_req) = 0;
	for (coun_TS = 0; coun_TS < numTS; coun_TS++) {
		if (processing_mem_map_read_s_proces_object_modbus(&val_TS, 1,address_TS) != MEM_OK)
		{// ���� ���������� � ��������������� ������ - �������������� ��������
			break;
		}
		if (val_TS >= 1) { // ����  �������� �������� ������ >0 - ���������� 1, ���� 0 - ���������� 0
			*(p_buf_rez_req + (u8) (coun_TS / 8)) |= (1
					<< (coun_TS - (u8) (coun_TS / 8) * 8));
		}
		address_TS++;
	}
	// ���� �����������
	buf_respons->un_excep_num.num_byte_res = num_byte_TS;
	modus_add_CRC((u8*) buf_respons, num_byte_TS + 5);
	return buf_respons->un_excep_num.num_byte_res + 5;
}

//---------------������� func_0304---------------
// ������� func_0304 - ��������� ����� �� ������ �� ������� 3 � 4, ������� ������������� �������� �� ����� ������
// ������� ���������:
// S_modbus_req *ps_modbus_req  - ��������� �� ������ � �������� �� �������
// S_modbus_make_res *buf_respons - ��������� �� ������ � ������������� �������
// �������� ���������:
// u16 - �-�� ���� ������ (������ ������)
u16 func_0304(void *req, S_modbus_make_res *p_buf_respons) {
	S_modbus_req_read *ps_modbus_req = req;
	u16 num_reg_TI;
	u16 address_TI;
	u8 counter;
	u16 *p_payload;

	address_TI = CharTooshort(ps_modbus_req->address_mem_L,	ps_modbus_req->address_mem_H); // �����
	num_reg_TI = CharTooshort(ps_modbus_req->num_reg_L,	ps_modbus_req->num_reg_H); // �-�� ��������� ��
	p_buf_respons->address_dev = ps_modbus_req->address_dev; // ����� ��
	p_buf_respons->function = ps_modbus_req->function; // �������
	// ������� ������ �� ����� ������
	processing_mem_map_read_s_proces_object_modbus((u16*) &p_buf_respons->resp_payload, num_reg_TI, address_TI);
	// ����� ������� �����: Hi->Lo, Lo-Hi
	p_payload=(u16*)&p_buf_respons->resp_payload;
	for(counter=0;counter<num_reg_TI;counter++){
		Convert_ShortTooCharHiLo((*p_payload),(u8*)p_payload);
		p_payload++;
	}

	// �������� ���� "�-��" � ������
	p_buf_respons->un_excep_num.num_byte_res = (num_reg_TI * 2);
	modus_add_CRC((u8*) p_buf_respons,
			p_buf_respons->un_excep_num.num_byte_res + 5);
	return (p_buf_respons->un_excep_num.num_byte_res + 5);
}

//---------------������� func_05---------------
// ������� func_05 - ��������� ��������� �������
// ������� ���������:
// S_modbus_req *ps_modbus_req  - ��������� �� ������ � ������� �� ��� ���������
// S_modbus_make_res *buf_respons - ��������� �� ������ � ����������� ���������� ��
// �������� ���������:
// u16 - �-�� ���� ������ (������ ������)
u16 func_05(void *req, S_modbus_make_res *buf_respons) {
	S_modbus_req_write *ps_modbus_req = req;
	u16 payload_modbus_req[2];
	//u32 k1=0;
	// ���� ������ �������������� ������� - ���������� �������
	if (s_modbus_add_procesing_req[FORCE_SINGLE_COIL - 1].flag_req_procesing) {
		// ���������� �������� ��������
		payload_modbus_req[0] = CharTooshort(ps_modbus_req->address_mem_L,
				ps_modbus_req->address_mem_H);
		payload_modbus_req[1] = CharTooshort(ps_modbus_req->forse_data_L,
				ps_modbus_req->forse_data_H);
		// �������� ���. ���������
		s_modbus_add_procesing_req[FORCE_SINGLE_COIL - 1].f_add_req_procesing(
				(void*) &payload_modbus_req[0],2,\
				CharTooshort(ps_modbus_req->address_mem_H,ps_modbus_req->address_mem_L)); // ��������� ������� ����������
	}
	else{
		// �������� � ����� ������ pay_load ��������� �������  !!!!!!!!!!!!!! �������� !!!!!!!!!!!!

	}

	// ���e� ��������� �� ��������, ������� ������ � ������ ������
	memcpy(buf_respons, ps_modbus_req, sizeof(S_modbus_req_write));
	return sizeof(S_modbus_req_write);
}

//---------------������� func_06---------------
// ������� func_06 - ��������� ������ ��������
// ������� ���������:
// S_modbus_req *ps_modbus_req  - ��������� �� ������ � ������� ��� ������
// S_modbus_make_res *buf_respons - ��������� �� ������ � ����������� ������
// �������� ���������:
// u16 - �-�� ���� ������ (������ ������)
u16 func_06(void *p_req, S_modbus_make_res *p_buf_respons) {
	S_modbus_req_input *ps_req=(S_modbus_req_input*)p_req;
	u16 temp_store;
	// ����� �������� ������ ������� � ����������� ����: ������� ���, ������� ����
	Convert_CharTooShortHiLo((u16*)&(ps_req->number_reg_H),&temp_store);

	// ������� �������� ������ � ����� ������
	processing_mem_map_write_s_proces_object_modbus( &temp_store,
			1,
			CharTooshort(ps_req->address_mem_H,ps_req->address_mem_L)
			);

	// ���� ������ �������������� ������� - ���������� �������
	if (s_modbus_add_procesing_req[ps_req->function - 1].flag_req_procesing) {
		// �������� ���. ���������
		s_modbus_add_procesing_req[ps_req->function - 1].f_add_req_procesing(
				(void*) &(ps_req->req_field_var.s_req_f16.payload),\
				CharTooshort(ps_req->number_reg_H,ps_req->number_reg_L),\
				/*ps_req->req_field_var.s_req_f16.byte_count,\*/
				CharTooshort(ps_req->address_mem_H,ps_req->address_mem_L));
	}

	// �������� ���e�
	memcpy(p_buf_respons, (u8*) p_req, sizeof(S_modbus_req_write));
	//�������� ����������� �����
	//modus_add_CRC((u8*) p_buf_respons, sizeof(S_modbus_req_write));
	return sizeof(S_modbus_req_write);
}



//---------------������� func_16---------------
// ������� func_16 - ��������� ����� ���������
// ������� ���������:
// S_modbus_req *ps_modbus_req  - ��������� �� ������ � ������� ��� ������
// S_modbus_make_res *buf_respons - ��������� �� ������ � ����������� ������
// �������� ���������:
// u16 - �-�� ���� ������ (������ ������)
u16 func_16(void *p_req, S_modbus_make_res *p_buf_respons) {
	S_modbus_req_input *ps_req=(S_modbus_req_input*)p_req;
	u8 *p_peyload = (u8*)&(ps_req->req_field_var.s_req_f16.payload);
	u8 temp_store, k1;
	// ����� �������� ������ ������� � ����������� ����: ������� ���, ������� ����
	for (k1 = 0; k1 < ps_req->number_reg_L; k1++) {
		temp_store=*p_peyload;
		*p_peyload=*(p_peyload+1);
		p_peyload++;
		*p_peyload=temp_store;
		p_peyload++;
	}
	// ������� �������� ������ � ����� ������
	processing_mem_map_write_s_proces_object_modbus( (u16*)&ps_req->req_field_var.s_req_f16.payload,ps_req->number_reg_L,
			CharTooshort(ps_req->address_mem_H,ps_req->address_mem_L)
			);

	// ���� ������ �������������� ������� - ���������� �������
	if (s_modbus_add_procesing_req[ps_req->function - 1].flag_req_procesing)
	{
		// �������� ���. ���������
		s_modbus_add_procesing_req[ps_req->function - 1].f_add_req_procesing((void*) &(ps_req->req_field_var.s_req_f16.payload),\
				CharTooshort(ps_req->number_reg_H,ps_req->number_reg_L),\
				/*ps_req->req_field_var.s_req_f16.byte_count,\*/
				CharTooshort(ps_req->address_mem_H,ps_req->address_mem_L));
	}

	// �������� ���e�
	memcpy(p_buf_respons, (u8*) p_req, sizeof(S_modbus_req_write) - 2); //-2 ����� ����������� �����
	//�������� ����������� �����
	modus_add_CRC((u8*) p_buf_respons, sizeof(S_modbus_req_write));
	return sizeof(S_modbus_req_write);
}

void t_Modbus_SLAVE(void *p_task_par) {
	S_modbus_req_input *ps_modbus_req_input; // ��������� �� ������ ������ ������

	u16 num_TSTI;
	u8 buf_out_slave[MODBUS_SIZE_BUFF_OUT]; // ���������� ������ ��������
	u8 buf_in_slave[MODBUS_SIZE_BUFF_IN]; // ���������� ������ ������
	REZ_REQ_CHEACK_SLAVE rez_chack_req;

    u8 address_KP = P_SHIFT(ps_connectmodbus_global,((S_modbus_tsk_par*)p_task_par)->num_modbus)->adress_kp;
	S_Task_parameters *ptaskparameters= ((S_modbus_tsk_par*)p_task_par)->ps_task_parameters;;

	ps_modbus_req_input = (S_modbus_req_input*) &buf_in_slave[0];


	while (ReadUSART(ptaskparameters->RdUSART, (u8*) ps_modbus_req_input, 1, 1)) {	} // ������ ������ ������
	vTaskDelay(500); //������� ������ ������� ���� �����, ��� ��������� ������ � �����������
	while (1) {

		if (ReadUSART(ptaskparameters->RdUSART, (u8*) ps_modbus_req_input, 8,20) != 8) { //�������� �� ����� USART. ���� ������ ���������� ������� �� 8 ����, �������� ������
			vTaskDelay(50);
			continue;
		}

		// �������� ��������
		rez_chack_req = cheack_req_modbus(ptaskparameters, ps_modbus_req_input,address_KP); //
		// ���������� ���������� ��������
		if (rez_chack_req == REQ_SLAVE_ERROR)
		{ // ������ ����� ��� ����������� �����
			continue;
		}
		else if (rez_chack_req)
		{ // ������ �� ����� ����������
			num_TSTI = modbus_exception(ps_modbus_req_input,(S_modbus_make_res*) buf_out_slave, rez_chack_req);
		}
		else
		{ // ������ ���������, ������� �-� ��������� �������
			if((ps_modbus_req_input->function - 1)==16)
			{
				ps_modbus_req_input->function =ps_modbus_req_input->function;
			}
			num_TSTI = p_procesing_slave[ps_modbus_req_input->function - 1]((void*) ps_modbus_req_input,(S_modbus_make_res*) buf_out_slave);
		}

		write_Usart_Buf(ptaskparameters->USARTx, buf_out_slave, num_TSTI,
				ptaskparameters->WrUSART); // �������� ������ ��� ����������

		vTaskDelay(10);
	}
}

// ----------------������� cheack_req_modbus------------------------
// ������� cheack_req_modbus ��������� �������� ��������� ������� �� ��������� �������:
// - ����� ��
// - ���������� �������� ���� �������
// - ���������� �������� ������� ��������� ��� ������� �-� ����� ������� callback ������������
// - ������������ ����������� �����
REZ_REQ_CHEACK_SLAVE cheack_req_modbus(S_Task_parameters *p_iparameters,
		S_modbus_req_input *ps_modbus_req_cheack, u8 address_kp) {
	u8 rez_addition_chek;
	u16 rez_CRC16, *p_req_CRC16;
	u16 payload_modbus_req[2]; //����� ������ ������������� ��������� - payload_modbus_req[0],
	                           //���������� ������������� ���������   - payload_modbus_req[1],
	u16 byte_count; // �-�� ����� ��������� ��������

	//   ----������� ����� �������� (��� ���� ����� ��������)-------------
	// �������� �� ���������� ������� �������
	if ((ps_modbus_req_cheack->function != READ_COIL_STATUS)
			& (ps_modbus_req_cheack->function != READ_INPUT_STATUS)
			& (ps_modbus_req_cheack->function != READ_HOLDING_STATUS)
			& (ps_modbus_req_cheack->function != READ_INPUT_REGISTERS)
			& (ps_modbus_req_cheack->function != FORCE_SINGLE_COIL)
			& (ps_modbus_req_cheack->function != FORSE_MULTIPLE_COILS)
			& (ps_modbus_req_cheack->function != PRESET_SINGLE_REGISTERS)
			& (ps_modbus_req_cheack->function != PRESET_MULTIPLE_REGISTERS)) {
		return REQ_SLAVE_ERROR;
	}
	// �������� ����� ���� ���������� ���������
	if (ps_modbus_req_cheack->address_dev != address_kp) {
		return REQ_SLAVE_ERROR;
	}
	//��� ��������: ��� ���� ������� ������ ������� �15-16 ��� ������ ����
	if ((ps_modbus_req_cheack->function == FORSE_MULTIPLE_COILS) // ���� ������� ������ ������� 15 ��� 16
			|| (ps_modbus_req_cheack->function == PRESET_MULTIPLE_REGISTERS)) {
		// �-�� ���� � �����
		byte_count = CharTooshort(ps_modbus_req_cheack->number_reg_H,ps_modbus_req_cheack->number_reg_L) * 2;
		// ������ �������� �������
		if ((u16) (ps_modbus_req_cheack->req_field_var.s_req_f16.byte_count)!= byte_count) {
			return REQ_SLAVE_ERROR;
		}
		// �������� ��������� ����� �������
		if (ReadUSART(p_iparameters->RdUSART, (u8*) (ps_modbus_req_cheack + 1),	byte_count - 1 + 2, 20) != (byte_count - 1 + 2)) { // -1: ���� ���� �������� �������� ��� ������. +2: ��� ����� ����������� �����
			return REQ_SLAVE_ERROR;
		};
		byte_count = byte_count + 7; // ����� ������� ��� ����������� �����
	} else {
		byte_count = 8 - 2; // ����� ������� ��� ����������� �����
	}

	// ����������� ����������� ����� ���������� �������
	rez_CRC16 = CRC16((u8*) ps_modbus_req_cheack, byte_count); // +7 ��� ��������� ������� ������� �16
	Convert_ShortTooCharHiLo(rez_CRC16, (u8*)&rez_CRC16);
	// ����� ������� ����� � CRC ����� ���� ��� � �������
	p_req_CRC16 = (u16*) ((u8*) ps_modbus_req_cheack + byte_count); // ���� ��������� �� ��������� ����������� �����
	// �������� ����������� ����� �������
	if ((*p_req_CRC16) != rez_CRC16) {
		return REQ_SLAVE_ERROR;
	}
    // ---------����������: ������� ���������� ������ ����������, ������ ������� �������� � ��� ������, �������� �������� ��������

    //*********************************************************************************************************
	//  -----------------------��������� ����������� ��������� ��������� �������------------------------------*
	//*********************************************************************************************************

	// ������� �� ������� ����� � �-�� ������������� ���������
	payload_modbus_req[0] = CharTooshort(ps_modbus_req_cheack->address_mem_H,
			ps_modbus_req_cheack->address_mem_L);
	payload_modbus_req[1] = CharTooshort(ps_modbus_req_cheack->number_reg_H,
			ps_modbus_req_cheack->number_reg_L);

	// �������� �������������� ������ ������������� ��������� ������� ������� ���������� ������� �������� ������ �������
	// ����� ������� callback ������������
	if (!s_modbus_check_address_req[ps_modbus_req_cheack->function - 1].flag_callback_check) { // ���� ��� ������������� ������� ������� �� ������
		return ILLEGAL_DATA_ADRESS;                                                              // ������ ������ �� ������ ������� �� ��������
	}
	else{
		if( s_modbus_check_address_req[ps_modbus_req_cheack->function - 1].pf_callback_check(&payload_modbus_req[0])){
			return ILLEGAL_DATA_ADRESS;
		};
	}

	//*********************************************************************************************************
	//  -----------------------������������ ����� ��������----------------------------------------------------*
	//*********************************************************************************************************

	// ���� ��_����� ��������� �������� ����������������� ��� ������ ������ �� ������� �-�
	if (!s_modbus_check_add_check[ps_modbus_req_cheack->function - 1].flag_callback_check) {
		return REQ_SLAVE_OK;
	}
	// ���� ����� ��������� �������������� ����������� ������ ������ �� ������� �-� (��� ������ �������� ��������� ����� ������ �����)
	rez_addition_chek = s_modbus_check_add_check[ps_modbus_req_cheack->function - 1].pf_callback_check(&payload_modbus_req[0]);

	// ���� ������� �������������� �������� ���������� ������������ ��������
	if ((rez_addition_chek != REQ_SLAVE_OK)
			&& (rez_addition_chek != REQ_SLAVE_ERROR)
			&& (rez_addition_chek != ILLEGAL_FUNCTION)
			&& (rez_addition_chek != ILLEGAL_DATA_ADRESS)
			&& (rez_addition_chek != ILLEGAL_DATA_VALUE)
			&& (rez_addition_chek != SLAVE_DEVICE_FALIURE)
			&& (rez_addition_chek != ACKNOWLEDGE)
			&& (rez_addition_chek != SLAVE_DEVICE_BUSY)
			&& (rez_addition_chek != NEGATIVE_ACKNOWLEDGE)
			&& (rez_addition_chek != MEMORY_PARITY_ERROR)) {
		return REQ_SLAVE_ERROR;
	}
	//----------------------------------------------------------------------------------------------------------------------


	// ��������� ��� ����������
	if (rez_addition_chek) {
		return rez_addition_chek;
	}
	//�����
	return REQ_SLAVE_OK;
}

// ----------------������� modbus_exception------------------------
// ������� modbus_exception ��������� �����-����������
u8 modbus_exception(S_modbus_req_input *ps_modbus_req,
		S_modbus_make_res *buf_respons, u8 exeption) {
	buf_respons->address_dev = ps_modbus_req->address_dev;
	buf_respons->function = ((0b10000000) | (ps_modbus_req->function)); // ������������ "1" � ������� ��� ���� �-�, �������� �� �������� ����������
	buf_respons->un_excep_num.type_exception = exeption;
	modus_add_CRC((u8*) buf_respons, SIZE_EXCEPTION);
	return SIZE_EXCEPTION;
}

// ----------------������� modbus_callback_add_check------------------------
// ������� modbus_callback_add_check - ������ �-� ��� �������������� �������� ���������� �������
// ������� ���������:
// p_modbus_callback_add_check - ����������� �� ������� �������������� ���������
// number_req - ���� ������� (�����)
void modbus_callback_add_check(PF_modbus_callback_check p_modbus_callback_add_check, u8 number_req) {
	s_modbus_check_add_check[number_req - 1].pf_callback_check =
			p_modbus_callback_add_check;
	s_modbus_check_add_check[number_req - 1].flag_callback_check = 1;
}


// ----------------������� modbus_callback_address_check------------------------
// ������� modbus_callback_address_check - ������ �-� ��� ��� �������� ������ �������
// ������� ���������:
// pf_modbus_callback_address_check - ����������� �� ������� �������������� ���������
// number_req - ���� ������� (�����)
void modbus_callback_address_check(PF_modbus_callback_check pf_modbus_callback_address_check, u8 number_req) {
	s_modbus_check_address_req[number_req - 1].pf_callback_check =
			pf_modbus_callback_address_check;
	s_modbus_check_address_req[number_req - 1].flag_callback_check = 1;
}


// ----------------������� modbus_add_processing_callback------------------------
// ������� modbus_add_processing_callback - ������ �������������� �-� ��������� �������
// ������� ���������:
// p_modbus_callback_req - ����������� �� ������� �������������� ���������
// number_req - ���� ������� (�����)
void  modbus_callback_add_processing(PF_add_processing_req_slave p_modbus_callback_req, u8 number_req) {
	s_modbus_add_procesing_req[number_req - 1].f_add_req_procesing =
			p_modbus_callback_req;
	s_modbus_add_procesing_req[number_req - 1].flag_req_procesing = 1;
}

