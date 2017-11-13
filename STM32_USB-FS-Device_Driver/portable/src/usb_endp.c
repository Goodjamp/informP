/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
  */


/* Includes ------------------------------------------------------------------*/

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"

#include "debugStuff.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Receive_Buffer[10];
__IO uint8_t PrevXferComplete[EP_NUM_MAX] = {0,0,0,0,0,0,0,0};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
typedef void (*usbReceivFun)(uint8_t*);
typedef struct{
	usbReceivFun setFetureCallback;
	uint8_t *recBuffer;
}endPointReceiv;


void EP1_OUT_Callback(void)
{
	USB_SIL_Read(EP1_OUT, Receive_Buffer);

	//setFetureCallback();

	SetEPRxStatus(ENDP1, EP_RX_VALID);
}


void EP2_OUT_Callback(void)
{
  USB_SIL_Read(EP2_OUT, Receive_Buffer);

  SetEPRxStatus(ENDP2, EP_RX_VALID);

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


void EP2_IN_Callback(void)
{
	debugPin_1_Set;
	PrevXferComplete[ENDP2] = 1;
	debugPin_1_Clear;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




