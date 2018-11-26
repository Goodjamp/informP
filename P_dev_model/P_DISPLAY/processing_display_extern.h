/**
  ******************************************************************************
  * @file    processing_display_extern.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */


#ifndef PROCESSING_DISPLAY_EXTERN_H_
#define PROCESSING_DISPLAY_EXTERN_H_

#include "stdint.h"

#include "processing_mem_map.h"

#define DEV_8                display
// max number of LCD screen
#define NUMBER_OF_LCD_STRING    4


#pragma pack(push,1)

//-----------------------------Адреса оперативных регистров процесса sensor---------------------------------------------------------------
typedef struct{
} S_display_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- Оперативные регистры процесса sensor------------------------------------------------------------------------
typedef struct{
} S_display_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- DISPLAY thread configuration--------------------------------------------------------------------------------
typedef struct{
	uint16_t state;                           // state of module: ENABLE/DISABLE
	uint16_t numScreen;                       // number of screen connected to device
	struct{                                   // configuration parameters of every screen
	       uint32_t numParamiterPerScreen: 8;
	       uint32_t bitsOfParamiters:     (32 - 8);
	}screenConfig[NUMBER_OF_LCD_STRING];
}S_display_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t display_calc_address_oper_reg(S_display_address *ps_display_address, uint16_t adres_start);
void t_processing_display(void *pvParameters);


#endif // PROCESSING_SENSOR_EXTERN_H_

