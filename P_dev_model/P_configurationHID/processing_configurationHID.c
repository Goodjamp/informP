/**
  ******************************************************************************
  * @file    processing_configurationHID.c
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    20-May-2018
  * @brief
  */

#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

#include "stm32f10x.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"

#include "usbHIDInterface.h"
#include "usb_user_setings.h"

#include "processing_mem_map.h"
#include "processing_config_dev.h"



#define NUM_QUEUE_MESSAGE   3
#define QUEUE_MESSAGE_SIZE  64

QueueHandle_t inMessageQueue;
QueueHandle_t outMessageQueue;
uint8_t reqBuf[QUEUE_MESSAGE_SIZE];
uint8_t respBuf[QUEUE_MESSAGE_SIZE];
//buffer inside usb ready Tx CB
uint8_t respBufCB[QUEUE_MESSAGE_SIZE];

typedef enum
{
	COMUNICATION_GET_REG = (uint8_t)0x0,
	COMUNICATION_SET_REG = (uint8_t)0x1,
	COMUNICATION_RESET   = (uint8_t)0x2,
}REQ_COMMAND;

typedef enum
{
	STATUS_RESP_OK = (uint8_t)0x0,
	STATUS_RESP_ERROR = (uint8_t)0x1,
}REQ_STATUS;

#pragma pack(push,1)

/****INPUT MESSAGE DESCRIPTION*******/


typedef struct
{
	uint16_t reAddress;
	uint8_t  numberOfReg;
}getRegReq_t;

typedef struct
{
	uint16_t reAddress;
	uint8_t  numberOfReg;
	uint8_t  payload[];
}setRegReq_t;

typedef struct
{
	uint8_t reqType;
	union
	{
		getRegReq_t getRegReq;
		setRegReq_t setRegReq;
	}payload;;
}headOfReq_t;


/****OUTPUT MESSAGE DESCRIPTION*******/
typedef struct
{
	uint16_t reAddress;
	uint8_t  numberOfReg;
	uint8_t  payload[];
}getRegResp_t;


typedef struct
{
	uint8_t reqType;
	uint8_t respStatus;
	union
	{
		getRegResp_t getRegResp;
	}payload;;
}headOfRes_t;

#pragma pack(pop)


void rxDataCB(uint8_t epNumber, uint8_t numRx, uint8_t* rxData)
{
	BaseType_t  rezPush = xQueueSendToBackFromISR(inMessageQueue, (void*)rxData, NULL);
}


void txDataComplete(void)
{
	if( pdTRUE != xQueueReceiveFromISR( outMessageQueue, (void*)respBufCB, NULL ) )
	{
		return;
	}
	usbHIDTx( EP_01, respBufCB, sizeof(respBufCB));
}


void t_processing_configurationHID(void *in_Data) {
	/*init USB HID */
	usbHIDInit();
	usbHIDAddRxCB(rxDataCB);
	usbHIDAddTxCompleteCB(txDataComplete);

	inMessageQueue  = xQueueCreate(NUM_QUEUE_MESSAGE, QUEUE_MESSAGE_SIZE);
	outMessageQueue = xQueueCreate(NUM_QUEUE_MESSAGE, QUEUE_MESSAGE_SIZE);
	while (1) {
		//Wait for receive message
		xQueueReceive( inMessageQueue, (void*)reqBuf, portMAX_DELAY);

		switch ( ((headOfReq_t*) reqBuf)->reqType)
		{
		case COMUNICATION_GET_REG:
		{
			((headOfRes_t*) respBuf)->reqType = COMUNICATION_GET_REG;
			((headOfRes_t*) respBuf)->payload.getRegResp.numberOfReg = ((headOfReq_t*) reqBuf)->payload.getRegReq.numberOfReg;
			((headOfRes_t*) respBuf)->payload.getRegResp.reAddress   = ((headOfReq_t*) reqBuf)->payload.getRegReq.reAddress;
			((headOfRes_t*) respBuf)->respStatus = (MEM_ERROR == processing_mem_map_read_s_proces_object_modbus(
									                                           (uint16_t*)((headOfRes_t*) respBuf)->payload.getRegResp.payload,
									                                           (uint16_t) ((headOfReq_t*) reqBuf)->payload.getRegReq.numberOfReg,
									                                           (uint16_t) ((headOfReq_t*) reqBuf)->payload.getRegReq.reAddress) )
									                                           ? (STATUS_RESP_ERROR)
									                                           : (STATUS_RESP_OK);
			if( usbHIDEPIsReadyToTx(EP_01) )
			{
				usbHIDTx( EP_01, respBuf, sizeof(respBuf));
			}
			else
			{
				xQueueSendToBack(inMessageQueue, (void*)&respBuf, 0);
			}
			break;
		}
		case COMUNICATION_SET_REG:
		{
			S_global_config *configData = (S_global_config*)((headOfReq_t*) reqBuf)->payload.setRegReq.payload;
			((headOfRes_t*) respBuf)->reqType = COMUNICATION_GET_REG;
			((headOfRes_t*) respBuf)->respStatus = (MEM_ERROR == processing_mem_map_write_s_proces_object_modbus(
									                                           (uint16_t*)((headOfReq_t*) reqBuf)->payload.setRegReq.payload,
									                                           (uint16_t) ((headOfReq_t*) reqBuf)->payload.getRegReq.numberOfReg,
									                                           (uint16_t) ((headOfReq_t*) reqBuf)->payload.getRegReq.reAddress ) )
									                                           ? (STATUS_RESP_ERROR)
									                                           : (STATUS_RESP_OK);
			//write new configuration data
			vTaskSuspendAll();
			FLASH_OPERATION_erase_page(PAGE_USER_CONFIG);
			processing_config_write_configuration();
			xTaskResumeAll();
			if( usbHIDEPIsReadyToTx(EP_01) )
			{
				usbHIDTx( EP_01, respBuf, sizeof(respBuf));
			}
			else
			{
				xQueueSendToBack(inMessageQueue, (void*)&respBuf, 0);
			}
			break;
	    }
		case COMUNICATION_RESET:
		{
			NVIC_SystemReset();
			break;
		}
		default:
			break;
		}
	}
}
