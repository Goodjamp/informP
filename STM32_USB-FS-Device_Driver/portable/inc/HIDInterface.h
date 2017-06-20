/* file:   HIDIntarface.h
 * Author: Gerasimchuk
 * Date:   13 jun 2017
 * Version: 1.0
 *
 * Inarface between USB FW and application
 */

#ifndef HIDINTARFACE_H_
#define HIDINTARFACE_H_

typedef void (*rxHIDHandler)(uint8_t);

typedef struct {
	uint8_t *pBuff;
	uint8_t sizeBuff;
}HIDRxBuff;



void USB_HIDInit(void);

/*
 * Add callback for Rx data from Host
 */
void addHIDEndPointOutCallBack(uint8_t EndPointNum,rxHIDHandler rxHandler, HIDRxBuff *pBuff);

/*
 * Tx Data to host
 */
void txDataToHost(uint8_t endPointNum,uint8_t *data, uint8_t dataSize);


#endif
