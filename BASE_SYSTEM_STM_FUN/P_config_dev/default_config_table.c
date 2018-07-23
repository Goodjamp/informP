/*
 * default_config_table.c
 *
 *  Created on:  4 January, 2016
 *      Author: Gerasimchuk
 *      Versin: 1
 */

//#include "check_init.h"

#include "processing_config_dev.h"


// -----------функция  config_check------------
// функция  config_check - для записи конфигурации по умолчанию.
// все поля конфигурации заполняються вручную
void default_config_table(S_global_config *ps_mem_data_set) {
	// очистка конфигурационной области флеш
	//FLASH_OPERATION_erase_page(num_page);


	// Поля настройки состава устройства
	ps_mem_data_set->s_dev_staff=(S_dev_staff){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	                                0,
	                                0,
	                                0,
	                                0,
	                                0
	};
    // дата конфигурации по умолчанию
	ps_mem_data_set->bf_date_config=(BF_date_config){21,4,15,14,13,12};

	//-------------------------------------------------------------------------------
	//----------------настройки ПОРТОВ И ПРОТОКОЛОВ----------------------------------
	//-------------------------------------------------------------------------------

	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.baudrate= 57600;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.stopbits = 1;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.parity = 0;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.amountbyte = 8;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].s_port_config.controlpotok=0;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].type = PROTOCOL_MODBUS_SLAVE; // master
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].waitresp = 0;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].number_of_pribor = 0;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].number_no_answer = 1;
	ps_mem_data_set->s_config_moduls.s_connectmodbus_global[0].adress_kp = 2;
    //==========default FRQ configuration=========
	ps_mem_data_set->s_config_moduls.s_FRQmetter_user_config.state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_FRQmetter_user_config.frqCorrection = 230;
	//==========default TIME configuration=========
	ps_mem_data_set->s_config_moduls.s_TIME_user_config.state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_TIME_user_config.timeCorection = 120;
	ps_mem_data_set->s_config_moduls.s_TIME_user_config.isDaylightSaving = 0;
	ps_mem_data_set->s_config_moduls.s_TIME_user_config.synchronizationSource = 0;
	//==========default SENSOR configuration=========
	ps_mem_data_set->s_config_moduls.s_sensor_user_config.state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_sensor_user_config.source = 0;
	//==========default DISPLAY configuration=========
	ps_mem_data_set->s_config_moduls.s_display_user_config.state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_display_user_config.numScreen = 4;

	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[0].numParamiterPerScreen = 3;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[0].bitsOfParamiters      =0b111;

	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[1].numParamiterPerScreen = 2;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[1].bitsOfParamiters      =0b1110;

	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[2].numParamiterPerScreen = 1;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[2].bitsOfParamiters      =0b1000;

	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[3].numParamiterPerScreen = 4;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[3].bitsOfParamiters      =0b1110000;
    // add crc16 off user configuration part of memory map
	ps_mem_data_set->s_config_moduls.configurationCRC16 = CRC16((u8*)&ps_mem_data_set->s_config_moduls, sizeof(S_config_moduls) - 2);
	// add default configuration that did not change
	processing_config_init_not_user_config(&ps_mem_data_set->s_dev_staff);
}
