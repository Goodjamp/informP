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

#include "usbHIDInterface.h"



void t_processing_configurationHID(void *in_Data)
{
    debugPinConfig();
	//Run HID (usb)
	USB_HIDInit();
	uint8_t usbHIDBuff[EP_COUNT] = {0,1,1,1,1,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
// CustomHID_init

	while(1){
		usbTx( 1, usbHIDBuff, sizeof(usbHIDBuff))
		{
			usbHIDBuff[0]++;
		}
	}
}
