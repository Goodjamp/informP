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

#define TEST_BRIGHTNES_INDEX   0


typedef struct{

}menuConfigDataDef;

typedef enum{
	event_KEY,
	event_DISPLAY,
	event_UPDATE_BRIGHTNESS
}actionTypeDef;

typedef enum{
	PAR_TEMPERATURE = 0,
	PAR_PRESSURE_PA,
	PAR_PRESSURE_MM,
	PAR_HUMIDITY,
	PAR_DATE_0,
	PAR_TIME_0,
	PAR_DATE_1,
	PAR_TIME_1,
	PAR_DATE_2,
	PAR_TIME_2,
	PAR_DATE_3,
	PAR_TIME_3,
	PAR_FRQ,
	PAR_BUI,
	PAR_QUANTITY
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

typedef union{
	uint8_t                      payload[ACTION_QUEUE_PAYLOAD];
	queueKeyActionItemPayload    KeyAction;
	queueSetBrightnesItemPayload Brightnes;
}queueMesType;

typedef struct{
	uint32_t     type;
	queueMesType queueType;
}actionQueueMember;


//--------------------------------------------------------------------------
//---------MENU PROCESSING FUNCTION PROTOTYPE-------------------------------
//--------------------------------------------------------------------------
uint16_t display_calc_address_oper_reg(S_display_address *ps_sensor_address, u16 adres_start);
void saveMenuConfigData(void);

#endif // PROCESSING_DISPLAY_H_
