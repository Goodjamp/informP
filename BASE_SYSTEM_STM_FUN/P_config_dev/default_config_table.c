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
    //==========default FRQ configuration=========
	ps_mem_data_set->s_config_moduls.s_FRQmetter_user_config.state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_FRQmetter_user_config.frqCorrection = 0;
	//==========default TIME configuration=========
	ps_mem_data_set->s_config_moduls.s_TIME_user_config.state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_TIME_user_config.timeCorection = 2;
	//==========default SENSOR configuration=========
	ps_mem_data_set->s_config_moduls.s_sensor_user_config.state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_sensor_user_config.pressureSens = 3;
	//==========default DISPLAY configuration=========
	ps_mem_data_set->s_config_moduls.s_display_user_config.state = ENABLE;
	ps_mem_data_set->s_config_moduls.s_display_user_config.numScreen = 4;

	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[0].numParamiterPerScreen = 3;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[0].listOfParamiters[0] = 0;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[0].listOfParamiters[1] = 2;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[0].listOfParamiters[2] = 3;

	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[1].numParamiterPerScreen = 2;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[1].listOfParamiters[0] = 4;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[1].listOfParamiters[1] = 5;

	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[2].numParamiterPerScreen = 1;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[2].listOfParamiters[0] = 6;

	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[3].numParamiterPerScreen = 4;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[3].listOfParamiters[0] = 0;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[3].listOfParamiters[1] = 2;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[3].listOfParamiters[2] = 4;
	ps_mem_data_set->s_config_moduls.s_display_user_config.screenConfig[3].listOfParamiters[3] = 6;

	// заполняю НЕ пользовательские настройки
	processing_config_init_not_user_config(&ps_mem_data_set->s_dev_staff);


	//FLASH_OPERATION_write_flash_16b((u16*) &s_mem_data_set,(sizeof(s_mem_data_set)+1) / 2 , address_write);

}





