/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
  */


/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_user_setings.h"
#include "usb_pwr.h"

#include "usbHIDInterface.h"


extern __IO uint32_t bDeviceState;

uint8_t Receive_Buffer[EP_COUNT+1];
__IO uint8_t PrevXferComplete[EP_NUM_MAX] = {[0 ... EP_NUM_MAX -1 ] = 1};
/* user callback function*/
rxHIDHandler rxCB;
rxHIDHandler txCompleteCB;


/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/


void EP1_OUT_Callback(void)
{
	uint32_t numRx = USB_SIL_Read(EP1_OUT, Receive_Buffer);

	SetEPRxStatus(ENDP1, EP_RX_VALID);
	//user CallbackFunction
	if(rxCB == NULL)
	{
		return;
	}
	rxCB( ENDP1, numRx, Receive_Buffer);
	// Call user processing function
}


/*******************************************************************************
* Function Name  : EP1_IN_Callback.
* Description    : EP1 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
  PrevXferComplete[ENDP1] = 1;
}



void USB_HIDInit(void){
	USB_Interrupts_Config();
    Set_USBClock();
    USB_Init();
}



void usbHIDAddRxCB(rxHIDHandler inRxCB)
{
	rxCB = inRxCB;
}


void usbHIDAddTxCompleteCB(rxHIDHandler inTxCompleteCB)
{
	txCompleteCB = inTxCompleteCB;
}


uint8_t usbHIDTx(uint8_t endPointNum, uint8_t *data, uint8_t dataSize)
{

	if((PrevXferComplete[endPointNum] != 1) || (bDeviceState != CONFIGURED))
	{
		return 0;
	}
	PrevXferComplete[endPointNum]=0;

	USB_SIL_Write( (0b10000000 | endPointNum) , data, dataSize);
	SetEPTxValid(endPointNum);

	return 1;
}


uint8_t usbHIDEPIsReadyToTx(uint8_t numEP)
{
	return (PrevXferComplete[numEP] == 1) ? (1) : (1);
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




