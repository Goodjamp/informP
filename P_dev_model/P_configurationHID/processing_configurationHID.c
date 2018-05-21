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

#include "stm32f10x.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"

#include "usbHIDInterface.h"
#include "usb_user_setings.h"

#include "processing_mem_map.h"



#define NUM_QUEUE_MESSAGE   3
#define QUEUE_MESSAGE_SIZE  64

QueueHandle_t inMessageQueue;
uint8_t messageBuffer[QUEUE_MESSAGE_SIZE];

typedef enum
{
	COMUNICATION_GET_REG = (uint8_t)0x0,
	COMUNICATION_SET_REG = (uint8_t)0x1,
	COMUNICATION_RESET   = (uint8_t)0x2,
}COMUNICATION_COMMAND;

#pragma pack(push,1)

typedef struct
{
	uint8_t commandType;
	uint8_t payload[];
}headOfMessage_t;

typedef struct
{
	uint8_t commandType;
	uint8_t payload[];
}getRegComman_t;


#pragma pack(pop)








void rxDataCB(uint8_t epNumber, uint8_t numRx, uint8_t* rxData)
{
	xQueueSendToBackFromISR(inMessageQueue, (void*)&rxData, 0);
}

void txDataComplete(void)
{

}



void t_processing_configurationHID(void *in_Data)
{
    /*init USB HID */
    usbHIDInit();
    usbHIDAddRxCB(rxDataCB);
    usbHIDAddTxCompleteCB(txDataComplete);

    inMessageQueue = xQueueCreate(NUM_QUEUE_MESSAGE, QUEUE_MESSAGE_SIZE);

	while(1){
		xQueueReceive( inMessageQueue, (void*)&messageBuffer,  portMAX_DELAY);

		switch( (((headOfMessage_t)(messageBuffer))->commandType)  )
		{
		case 	COMUNICATION_GET_REG: break;
		case 	COMUNICATION_SET_REG:

			    break;
		case 	COMUNICATION_RESET:
			    NVIC_SystemReset();
			    break;
		default: break;
		}
	}
}
