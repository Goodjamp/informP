/*
 * default_config_table.c
 *
 *  Created on:  4 January, 2016
 *      Author: Gerasimchuk
 *      Versin: 1
 */

//#include "check_init.h"

#include "processing_config_dev.h"


// -----------�������  config_check------------
// �������  config_check - ��� ������ ������������ �� ���������.
// ��� ���� ������������ ������������ �������
void default_config_table(S_global_config *ps_mem_data_set) {
	// ������� ���������������� ������� ����
	//FLASH_OPERATION_erase_page(num_page);


	// ���� ��������� ������� ����������
	ps_mem_data_set->s_dev_staff=(S_dev_staff){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	                                0,
	                                0,
	                                0,
	                                0,
	                                0
	};
    // ���� ������������ �� ���������
	ps_mem_data_set->bf_date_config=(BF_date_config){21,4,15,14,13,12};

	//-------------------------------------------------------------------------------
	//----------------��������� ������ � ����������----------------------------------
	//-------------------------------------------------------------------------------


	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.baudrate= 9600;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.stopbits = 1;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.parity = 0;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.amountbyte = 8;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.controlpotok=0;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].type = PROTOCOL_MODBUS_SLAVE; // master
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].waitresp = 20;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].number_of_pribor = 1;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].number_no_answer = 10;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].adress_kp = 1;

	ps_mem_data_set->s_config_moduls.s_FRQmetter_user_config.state= ENABLE;

	// �������� �� ���������������� ���������
	processing_config_init_not_user_config(&ps_mem_data_set->s_dev_staff);


	//FLASH_OPERATION_write_flash_16b((u16*) &s_mem_data_set,(sizeof(s_mem_data_set)+1) / 2 , address_write);

}





