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

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_user_setings.h"

#include "debugStuff.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Receive_Buffer[EP_COUNT+1];
__IO uint8_t PrevXferComplete[EP_NUM_MAX] = {1,1,1,1,1,1,1,1};
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

extern uint8_t rxF;
void EP1_OUT_Callback(void)
{
	USB_SIL_Read(EP1_OUT, Receive_Buffer);

	//setFetureCallback();
	rxF = 1;
	SetEPRxStatus(ENDP1, EP_RX_VALID);
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
	static uint8_t cnt = 0;
	if(cnt++ & 0b1)
		debugPin_1_Set;
	else
		debugPin_1_Clear;
  PrevXferComplete[ENDP1] = 1;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




