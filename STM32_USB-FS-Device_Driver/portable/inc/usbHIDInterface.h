/* file:   HIDIntarface.h
 * Author: Gerasimchuk
 * Date:   13 jun 2017
 * Version: 1.0
 *
 * Inarface between USB FW and application
 */

#ifndef HIDINTARFACE_H_
#define HIDINTARFACE_H_

#include "stdint.h"

typedef void (*rxHIDHandler)(uint8_t epNumber, uint8_t numRx, uint8_t* rxData);


void USB_HIDInit(void);

/*
 * Add callback for Rx data from Host
 */
void addHIDEndPointOutCallBack(uint8_t EndPointNum,rxHIDHandler rxHandler, HIDRxBuff *pBuff);

/*
 * Tx Data to host
 */
uint8_t usbTx(uint8_t endPointNum,uint8_t *data, uint8_t dataSize);

#endif
