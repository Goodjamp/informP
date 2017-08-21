/**
  ******************************************************************************
  * @file    processing_display.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */
#include "stdint.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "processing_mem_map_extern.h"
#include "processing_display.h"
#include "max7219.h"
#include "stmMaxHardwareInit.h"


extern S_address_oper_data s_address_oper_data;

static displayBuffDef    displayBuff[maxPerScreen];
static displayHandlerDef displayHandler;
static LDDescr LDList[numberOfScreen] = {
		[SCREEN_1] = {
				.port = PORT_LD_1,
				.pin =  PIN_LD_1
				},
		[SCREEN_2] = {
				.port = PORT_LD_2,
				.pin =  PIN_LD_2
				},
		[SCREEN_3] = {
				.port = PORT_LD_3,
				.pin =  PIN_LD_3
				},
		[SCREEN_4] = {
				.port = PORT_LD_4,
				.pin =  PIN_LD_4
				},
};



displayBuffDef displayBuff_1[2] = {
		{0b0000000100000000,0b0000001011111111,0b0000001100000000,0b0000010011111111,0b0000010100000000,0b0000011011111111,0b0000011111111111,0b0000100011111111,},
		{0b0000000111111111,0b0000001000000000,0b0000001100000000,0b0000010011111111,0b0000010100000000,0b0000011000000000,0b0000011100000000,0b0000100011111111,}
};



/**
  * @brief
  * @param
  * @retval
  */
uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start){
	return adres_start;
}




/**
  * @brief
  * @param
  * @retval
  */
void t_processing_display(void *pvParameters){

	initDisplay(&displayHandler, LDList, numberOfScreen, DISPLAY_SPI);

	displayConfigWorkMode(0, SHUT_DOWN_NORMAL_OPERATION, displayBuff);
	displayConfigWorkMode(1, SHUT_DOWN_NORMAL_OPERATION, displayBuff);
	displayTxData(&displayHandler, 0, 2, displayBuff, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(&displayHandler) == DISPLAY_BUSY){}


	displayConfigScanLimit(0, SCAN_LIM_7, displayBuff);
	displayConfigScanLimit(1, SCAN_LIM_7, displayBuff);
	displayTxData(&displayHandler, 0, 2, displayBuff, TX_ADDRESS_ALL);
	displayTxData(&displayHandler, 0, 2, displayBuff, TX_ADDRESS_ALL);
	while(displayIntarfaceGetStatus(&displayHandler) == DISPLAY_BUSY){}
	while(1){

		displayTxData(&displayHandler, 0, 2, displayBuff_1, TX_ADDRESS_ALL);
	}
}
