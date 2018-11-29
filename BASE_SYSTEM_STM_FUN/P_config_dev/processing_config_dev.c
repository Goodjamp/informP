/*
 * processing_config_dev.c
 *
 *  Created on: April 25, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "processing_config_dev.h"
#include "default_configuration_description.h"
#include "processing_TIME_extern.h"

static uint8_t          userConfiguration[]  __attribute__((section("userConfigurationSection")))    = FIRST_POWER_ON_ATTRIBUTE;
static S_global_config  defaultConfiguration __attribute__((section("defaultConfigurationSection"))) = {
		.s_dev_staff = {
			.bf_dev_staff = {

		        .MODULE_0=ENABLE,   /*Modbus master*/

#ifdef DEV_1
		        .MODULE_1=ENABLE,
#endif
#ifdef DEV_2
		        .MODULE_2=ENABLE,
#endif
#ifdef DEV_3
		        .MODULE_3=ENABLE,
#endif
#ifdef DEV_4
		        .MODULE_4=ENABLE,
#endif
#ifdef DEV_5
		        .MODULE_5=ENABLE,
#endif
#ifdef DEV_6
		        .MODULE_6=ENABLE,
#endif
#ifdef DEV_7
		        .MODULE_7=ENABLE,
#endif
#ifdef DEV_8
		        .MODULE_8=ENABLE,
#endif
#ifdef DEV_9
		        .MODULE_9=ENABLE,
#endif
#ifdef DEV_10
		        .MODULE_10=ENABLE,
#endif
#ifdef DEV_11
		        .MODULE_11=ENABLE,
#endif
#ifdef DEV_12
		        .MODULE_12=ENABLE,
#endif
#ifdef DEV_13
		        .MODULE_13=ENABLE,
#endif
#ifdef DEV_14
		        .MODULE_14=ENABLE,
#endif
#ifdef DEV_15
		        .MODULE_15=ENABLE,
#endif
			},
		    .num_use_uart    = NUM_PORT_MODBUS,
		    .num_reg_data    = NUM_REG_DATA,
		    .modbus_req_user = MAX_NUM_MODBUS_REQ,
		    .program_version = 1,
	   },
		.bf_date_config = {
		    .year   = 18,
			.mounth = 7,
			.day    = 29,
			.hour   = 16,
			.minute = 39,
			.second = 0,
		},
		DEFAULT_CONFIG_DESCRIPTION
};

/*This structure containe address of all modulus and fild of modulus*/
extern S_address_oper_data s_address_oper_data;
/*This structure contain configuration of all modulus and use by modulus*/
extern S_config_moduls s_config_moduls;
/*This register containe of absolute pointers on all modulus*/
extern S_mem_map s_mem_map;


void processing_config_first_on(void){
	if(memcmp(FIRST_POWER_ON_ATTRIBUTE, userConfiguration, strlen(FIRST_POWER_ON_ATTRIBUTE)) != 0) //first power on detect
	{
        return;
	}
	//calc CRC, eritw default configuration
	S_global_config tempUserConfigBuff;
	union dataBuff
	{
		uint8_t         *bufPoint;
		uint16_t        *bufPointShort;
		S_global_config *dataPoint;
	};
	union dataBuff defauldDataBuff = {.dataPoint = &defaultConfiguration};
	union dataBuff userdDataBuff   = {.dataPoint = &tempUserConfigBuff};
	memcpy(userdDataBuff.bufPoint, defauldDataBuff.bufPoint, (sizeof(S_global_config)-2));
	tempUserConfigBuff.configurationCRC16 = CRC16(defauldDataBuff.bufPoint, (sizeof(S_global_config)-2));
	//write default configuration on user configuration space
	FLASH_OPERATION_erase_page(PAGE_USER_CONFIG);
	FLASH_OPERATION_write_flash_16b(userdDataBuff.bufPointShort, NUM_REG_CONFIG, PAGE_ABS_ADDRESS(PAGE_USER_CONFIG ));
}


bool processing_config_init(bool isUserConfig)
{
	union
	{
		uint8_t         *bufPoint;
		S_config_moduls *dataPoint;
	}userdConfigBuff =
	{
		.dataPoint = &s_config_moduls
	};
    // user memory map include only user configuration data
	memcpy(s_mem_map.p_start_config_data, userConfiguration, sizeof(S_global_config));
	//configuration for that will be use by modulus
	memcpy(userdConfigBuff.bufPoint,
			(isUserConfig) ? &(((S_global_config*)PAGE_ABS_ADDRESS(PAGE_USER_CONFIG ))->s_config_moduls) :
					         &(((S_global_config*)PAGE_ABS_ADDRESS(PAGE_DEFAULT_CONFIG ))->s_config_moduls),
		    (sizeof(S_global_config)-2));
	// calculation CRC for detect configuration error
	uint16_t calcCRC = CRC16((uint8_t*)PAGE_ABS_ADDRESS(PAGE_USER_CONFIG ), (sizeof(S_global_config)-2));
    return (calcCRC == ((S_global_config*)s_mem_map.p_start_config_data)->configurationCRC16) ? (true) : (false);
}


//------------функция processing_config_is_holding_reg-----------------------------------
// функция processing_config_is_holding_reg - выполняет проверку адресса карты памяти на принадлежность регистрам конфигурации устройства
// даная функция соответствует проверке команде №3 modbus
// входные аргументы:
// *p_check_address - указатель на начало последовательности (u16)address, (u16)num_reg
// выходные аргументы:
// REQ_SLAVE_OK - проверка выполнена успешно
// ILLEGAL_DATA_ADRESS - недопустимый адрес
REZ_REQ_CHEACK_SLAVE processing_config_check_is_holding_reg(uint16_t addressReg, uint16_t numberOfReg)
{
	// адрес принадлежит области оперативных регистров
	if((addressReg >= USER_ADDRESS_OPER_DATA) && ((addressReg + numberOfReg - 1) < (USER_ADDRESS_OPER_DATA + NUM_REG_OPER_DATA)))
	{
		return REQ_SLAVE_OK;
	}
	// адрес принадлежит области настроек
	if((addressReg >= USER_ADDRESS_CONFIG_DATA) && ((addressReg + numberOfReg - 1) < (USER_ADDRESS_CONFIG_DATA + NUM_REG_CONFIG)))
	{
		return REQ_SLAVE_OK;
	}
	// адрес находиться за границами региcтров настройки
	return ILLEGAL_DATA_ADRESS;
}


//------------функция processing_config_check_is_preset_multiple_register-----------------------------------
// функция processing_mem_map_is_input_reg - выполняет проверку адресса карты памяти на принадлежность регистрам
// даная функция соответствует проверке команде №16 modbus
// входные аргументы:
// *check_data - указатель на начало последовательности (u16)address
// выходные аргументы:
// REQ_SLAVE_OK - проверкаа выполнена успешно
// ILLEGAL_DATA_ADRESS - недопустимый адрес

REZ_REQ_CHEACK_SLAVE processing_config_check_is_preset_multiple_reg(uint16_t addressReg, uint16_t numberOfReg)
{
    if( (addressReg == s_address_oper_data.s_TIME_address.serverYear) && ((addressReg + numberOfReg - 1) == s_address_oper_data.s_TIME_address.serverSeconds ) )
    {
    	return REQ_SLAVE_OK;
    }
    else if((addressReg >= USER_ADDRESS_CONFIG_DATA) && ((addressReg + numberOfReg - 1) <= (sizeof(s_address_oper_data) + 1) / 2) )
    {
    	return REQ_SLAVE_OK;
    }
	return REQ_SLAVE_ERROR;
}


//-------------update_config_data------------------
//функция update_config_data - выполняет управление конфигурацией.
// входные аргументы:
// req               - указатель на информационную часть сообщения с правильным следованием байт
// num_peyload_data  -
// После приема всех конфигурационных сообщений, выполняеться проверка конфигурации
// и запись во FLESH.
// Процес приема конфигурации:
// 1 - жду приема сообщения с адресом равным адресу начала области данных конфигурации, устанавливаю флаг начала приема конфигурации
// 2 - после каждого приннятого пакета увеличиваю следующий адреса записи
// 3 - если адрес в следующем пакете не совпадает со счетчиком адресов (п.2) перехожу к п.1
// 4 - после приема всей конфигурационной информации, выполняю проверку и записываю к-цию во флеш
uint8_t saveConfiguration(void* req, u8 num_peyload_data, u16 addres_data)
{
	static u16 count_config_data=0;
	static u8 f_rx_config_data=0; // флаг начаоа приема к-ции
	// если принята случайная посылка
	if((f_rx_config_data==0)&(addres_data!=USER_CONFIG_START_ADRESS))
	{
		return 1;
	}
	//если принят первый пакет посылки конфигурации
	if(f_rx_config_data==0)
	{
		f_rx_config_data=1;
		count_config_data=num_peyload_data;
		return 0;
	}
	// если приннятый пакет данных конфигурации не соответствует проверочным точкам
	if((addres_data-USER_CONFIG_START_ADRESS)!=count_config_data)
	{
		f_rx_config_data=0;
		count_config_data=0;
		return 1;
	}
	// увеличиваю счетчик адреса следующей посылки
	count_config_data+=num_peyload_data;
	// если принята вся конфигурационная посылка - выполнить ПРОВЕРКУ и запись во FLESH
	if(count_config_data==(USER_CONFIG_DATA_SIZE))
	{
		vTaskSuspendAll(); // остановить планировщик
		count_config_data=0;
		f_rx_config_data=0; //
		FLASH_OPERATION_erase_page(PAGE_USER_CONFIG);
		FLASH_OPERATION_write_flash_16b((u16*)s_mem_map.p_start_config_data,sizeof(S_global_config) / 2 + 1, PAGE_ABS_ADDRESS(PAGE_USER_CONFIG ));
		xTaskResumeAll();
		// ---------здесь выполнить проверку приннятой посылки----------

		// можно так: вызов функций конфигурации из "main.c" сгрупировать в одну функцию перенести
	}
	if(count_config_data>NUM_REG_CONFIG)
	{
		return 1;
	}
	return 0;
}


static uint8_t processingPresetMultipleRegisters(void* req, u8 numberOfReg, u16 addressReg)
{
    if( (addressReg == s_address_oper_data.s_TIME_address.serverYear) && ((addressReg + numberOfReg - 1) == s_address_oper_data.s_TIME_address.serverSeconds ) )
    {
    	return serverUpdatteTime();
    }
    else if((addressReg >= USER_ADDRESS_CONFIG_DATA) && ((addressReg + numberOfReg - 1) <= (sizeof(s_address_oper_data) + 1) / 2) )
    {
    	return saveConfiguration(req, numberOfReg, addressReg);
    }
    return 0;
}


void processing_config_write_configuration(void)
{
	union
	{
		uint8_t         *bufPoint;
		uint16_t        *bufPointShort;
		S_global_config *dataPoint;
	} userdDataBuff =
	{
        .dataPoint = (S_global_config*)s_mem_map.p_start_config_data,
	};
	userdDataBuff.dataPoint->configurationCRC16 = CRC16(userdDataBuff.bufPoint, sizeof(S_global_config) - 2 );
	FLASH_OPERATION_erase_page(PAGE_USER_CONFIG);
	FLASH_OPERATION_write_flash_16b(userdDataBuff.bufPointShort,
			                        NUM_REG_CONFIG,
			                        PAGE_ABS_ADDRESS(PAGE_USER_CONFIG ));
}



//-------------processing_config_dev_init------------------
//функция processing_config_dev_init - выполняет заполнение битового поля состава устройства согласно настройкам пользователя
//                                     копирует данные конфигурации в рабочую структуру для доступа програмных модулей,
//                                     задает callback приема функции 16 modbus
void processing_config_add_modbus_callback(void){
	// задаю ф-ю callback которая вызываться процессом modbus_slave для проверки запрашиваимого адресса запросом №3
	modbus_callback_address_check(&processing_config_check_is_holding_reg, READ_HOLDING_STATUS);
	// задаю ф-ю callback которая вызываться процессом modbus_slave для проверки запрашиваимого адресса запросом №16
	modbus_callback_address_check(&processing_config_check_is_preset_multiple_reg,PRESET_MULTIPLE_REGISTERS);
	// задаю ф-ю callback которая вызываться процессом modbus_slave после приема сообщения  №16
	modbus_callback_add_processing(&processingPresetMultipleRegisters,PRESET_MULTIPLE_REGISTERS);
}
