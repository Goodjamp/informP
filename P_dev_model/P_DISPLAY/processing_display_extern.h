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
// number of possible value for indication
#define NUMBER_OF_VALUE      7
// max number of LCD screen
#define NUMBER_OF_LCD_STRING 4

//#define DEV_1_BIT_STATUS_REG 1


#pragma pack(push,1)

//-----------------------------������ ����������� ��������� �������� sensor---------------------------------------------------------------
typedef struct{
} S_display_address;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- ����������� �������� �������� sensor------------------------------------------------------------------------
typedef struct{
} S_display_oper_data;
//--------------------------------------------------------------------------------------------------------------------------------------


//----------------------------- DISPLAY thread configuration--------------------------------------------------------------------------------
typedef struct{
	uint16_t state;                           // state of module: ENABLE/DISABLE
	uint16_t numScreen;                       // number of screen connected to device
	struct{                                   // configuration parameters of every screen
	       uint16_t numParamiterPerScreen: 3;
	       uint16_t bitsOfParamiters:     (16 - 3);
	}screenConfig[NUMBER_OF_LCD_STRING];
}S_display_user_config;
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(pop)

uint16_t display_calc_address_oper_reg(S_display_address *ps_display_address, uint16_t adres_start);
void t_processing_display(void *pvParameters);


#endif // PROCESSING_SENSOR_EXTERN_H_

