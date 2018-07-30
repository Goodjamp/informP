/*
 * default_configuration_description.h
 *
 *  Created on: 29 July, 2018
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#include "stdint.h"
#include "processing_mem_map.h"

#ifndef  DEFAULT_CONFIGURATION_dESCRIPTION_H_
#define  DEFAULT_CONFIGURATION_dESCRIPTION_H_

#define FIRST_POWER_ON_ATTRIBUTE  "FIRST_POWER_ON"

#define DEFAULT_CONFIG_DESCRIPTION                                                       \
	.s_config_moduls.s_connectmodbus_global[0].state = ENABLE,                           \
	.s_config_moduls.s_connectmodbus_global[0].s_port_config.baudrate= 57600,            \
	.s_config_moduls.s_connectmodbus_global[0].s_port_config.stopbits = 1,               \
	.s_config_moduls.s_connectmodbus_global[0].s_port_config.parity = 0,                 \
	.s_config_moduls.s_connectmodbus_global[0].s_port_config.amountbyte = 8,             \
	.s_config_moduls.s_connectmodbus_global[0].s_port_config.controlpotok=0,             \
	.s_config_moduls.s_connectmodbus_global[0].type = PROTOCOL_MODBUS_SLAVE,             \
	.s_config_moduls.s_connectmodbus_global[0].waitresp = 0,                             \
	.s_config_moduls.s_connectmodbus_global[0].number_of_pribor = 0,                     \
	.s_config_moduls.s_connectmodbus_global[0].number_no_answer = 1,                     \
	.s_config_moduls.s_connectmodbus_global[0].adress_kp = 2,                            \
	/*==========default FRQ configuration=========*/                                     \
	.s_config_moduls.s_FRQmetter_user_config.state = ENABLE,                             \
	.s_config_moduls.s_FRQmetter_user_config.frqCorrection = 230,                        \
	/*==========default TIME configuration========*/                                     \
	.s_config_moduls.s_TIME_user_config.state = ENABLE,                                  \
	.s_config_moduls.s_TIME_user_config.timeCorection = 120,                             \
	.s_config_moduls.s_TIME_user_config.isDaylightSaving = 0,                            \
	.s_config_moduls.s_TIME_user_config.synchronizationSource = 0,                       \
	/*==========default SENSOR configuration========*/                                   \
	.s_config_moduls.s_sensor_user_config.state = ENABLE,                                \
	.s_config_moduls.s_sensor_user_config.source = 0,                                    \
	/*==========default DISPLAY configuration=======*/                                   \
	.s_config_moduls.s_display_user_config.state = ENABLE,                               \
	.s_config_moduls.s_display_user_config.numScreen = 4,                                \
                                                                                         \
	.s_config_moduls.s_display_user_config.screenConfig[0].numParamiterPerScreen = 3,    \
	.s_config_moduls.s_display_user_config.screenConfig[0].bitsOfParamiters =0b111,      \
                                                                                         \
	.s_config_moduls.s_display_user_config.screenConfig[1].numParamiterPerScreen = 2,    \
	.s_config_moduls.s_display_user_config.screenConfig[1].bitsOfParamiters =0b110,      \
                                                                                         \
	.s_config_moduls.s_display_user_config.screenConfig[2].numParamiterPerScreen = 1,    \
	.s_config_moduls.s_display_user_config.screenConfig[2].bitsOfParamiters =0b1000,     \
                                                                                         \
	.s_config_moduls.s_display_user_config.screenConfig[3].numParamiterPerScreen = 3,    \
	.s_config_moduls.s_display_user_config.screenConfig[3].bitsOfParamiters =0b1110000,  \

#endif