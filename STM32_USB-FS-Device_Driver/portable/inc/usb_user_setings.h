/**
  ******************************************************************************
  * @file    usb_user_setings.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    24-January-2017
  * @brief   Main setings of USB
  * */
#ifndef USB_USER_SETINGS_H_
#define USB_USER_SETINGS_H_

/* EP_NUM */
#define EP_NUM     (4)
#define EP_NUM_INTERFACE_1     (4)
#define EP_NUM_INTERFACE_2     (2)

/* Selected EP definition  (uncoment selected)*/
/*8n - EPn_IN  endpoint (host IN)
 *0n - EPn_OUT endpoint (host OUT)
 */
/*----EP 1----*/
#define EP_01
#define EP_81
/*----EP 2----*/
#define EP_02
#define EP_82
/*----EP 3----*/
//#define EP_03
//#define EP_83
/*----EP 4----*/
//#define EP_04
//#define EP_84
/*----EP 5----*/
//#define EP_05
//#define EP_85
/*----EP 6----*/
//#define EP_06
//#define EP_86
/*----EP 7----*/
//#define EP_07
//#define EP_87

/* EP1  */
/* tx buffer base address */
#define ENDP1_TXADDR        (0x100)
#define ENDP1_RXADDR        (0x108)

/* EP2  */
/* tx buffer base address */
#define ENDP2_TXADDR        (0x116)
#define ENDP2_RXADDR        (0x124)


#endif