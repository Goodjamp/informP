/* file:    HIDIntarface.c
 * Author:  Gerasimchuk
 * Date:    13 jun 2017
 * Version: 1.0
 *
 * Inarface between USB FW and application
 */

#include "stdint.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"

#include "HIDInterface.h"
extern __IO uint8_t PrevXferComplete[EP_NUM_MAX];
extern __IO uint32_t bDeviceState;


void USB_HIDInit(void){
	USB_Interrupts_Config();
    Set_USBClock();
    USB_Init();
}


/*
 * Add callback for Rx data from Host
 */
void addHIDEndPointOutCallBack(uint8_t EndPointNum,rxHIDHandler rxHandler, HIDRxBuff *pBuff){

};

/*
 * Tx Data to host
 */
void txDataToHost(uint8_t endPointNum,uint8_t *data, uint8_t dataSize){
	if((PrevXferComplete[ENDP2] != 1) || (bDeviceState != CONFIGURED))
	{
		return;
	}
	USB_SIL_Write(endPointNum, data, dataSize);
	SetEPTxValid(endPointNum);
	PrevXferComplete[endPointNum]=0;

}
