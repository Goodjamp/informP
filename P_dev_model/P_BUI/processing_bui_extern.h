/**
  ******************************************************************************
  * @file    processing_bui_extern.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    26-Jun-2021
  * @brief
  */

#ifndef PROCESSING_BUI_EXTERN_H_
#define PROCESSING_BUI_EXTERN_H_

#include "stdint.h"

#include "processing_mem_map.h"

#define DEV_9    bui

#pragma pack(push,1)

typedef struct{
	uint16_t  buiStatus;
	uint16_t  buiMatrixSymbol;
	uint16_t  buiSegmentSymbol0;
	uint16_t  buiSegmentSymbol1;
	uint16_t  buiSegmentSymbol2;
	uint16_t  buiSegmentSymbol3;
} S_bui_address;

typedef struct{
	S_proces_object_modbus  buiStatus;
	S_proces_object_modbus  buiMatrixSymbol;
	S_proces_object_modbus  buiSegmentSymbol0;
	S_proces_object_modbus  buiSegmentSymbol1;
	S_proces_object_modbus  buiSegmentSymbol2;
	S_proces_object_modbus  buiSegmentSymbol3;
} S_bui_oper_data;

typedef struct {
    uint8_t state;
    uint16_t baudrate;
    uint8_t parity;
    uint8_t address;
}S_bui_user_config;
#pragma pack(pop)

uint16_t bui_calc_address_oper_reg(S_bui_address *ps_bui_address, uint16_t adres_start);
void t_processing_bui(void *pvParameters);

#endif // PROCESSING_BUI_EXTERN_H_

