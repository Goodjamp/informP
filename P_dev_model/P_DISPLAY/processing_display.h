/**
  ******************************************************************************
  * @file    processing_display.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    15-Agust-2017
  * @brief
  */

#ifndef PROCESSING_DISPLAY_H_
#define PROCESSING_DISPLAY_H_
#include "processing_display_extern.h"
#include "menuProcessing.h"


//
#define LCD_UPDATE_HIGHT_MS 600
#define LCD_UPDATE_LOW_MS   400
//MAX length of LCD string
#define LCD_STR_LENGTH         20
//USER KEYS PARAMITERS
#define ACTION_QUEUE_QANTITY   5
#define ACTION_QUEUE_PAYLOAD   2
//
#define BUTTON_PROCESSING_MS   5

#define BUTTON_ESC_PERIOD_MS   5000

#define FLASH_PAGE_MENU_DATA   125

#define TEST_BRIGHTNES_UNDEX   0


typedef struct{

}menuConfigDataDef;

typedef enum{
	event_KEY,
	event_DISPLAY,
	event_Brightnes
}actionTypeDef;

typedef enum{
	PAR_TEMPERATURE = 0,
	PAR_PRESSURE_PA,
	PAR_PRESSURE_MM,
	PAR_HUMIDITY,
	PAR_DATE ,
	PAR_TIME,
	PAR_FRQ
}paramOrderDef;

typedef enum{
	BLINK_STATE_HIGHT,
	BLINK_STATE_LOW
}BLINK_STATE;

//#pragma pack(push,1)
typedef struct{
    uint8_t action;
    uint8_t periodIndex;
}queueKeyActionItemPayload;
//#pragma pack(pop)

// item of payload queue in case of Set brightnes
//#pragma pack(push,1)
typedef struct{
    uint8_t action;
    uint8_t reserved;
}queueSetBrightnesItemPayload;
//#pragma pack(pop)


typedef struct{
	uint8_t type;
	uint8_t payload[ACTION_QUEUE_PAYLOAD];
}actionQueueMember;


//--------------------------------------------------------------------------
//---------MENU PROCESSING FUNCTION PROTOTYPE-------------------------------
//--------------------------------------------------------------------------
uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start);

#endif // PROCESSING_DISPLAY_H_
