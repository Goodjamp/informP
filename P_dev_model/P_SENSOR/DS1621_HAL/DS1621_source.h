/*
*  DS1621_sourse.h
 *
 *  Created on: July 30, 2017
 *      Author: Gerasimchuk
 *      Versin: 1
 */
#ifndef DS1621_SOURCE_H_
#define DS1621_SOURCE_H_

#include "stdint.h"

typedef enum{
	DS1621_STATUS_OK = 0,
	DS1621_STATUS_BUSS_ERROR,
	DS1621_STATUS_TEMP_NOT_REDY
}DS1621status;

typedef enum{
	DS1621_MESS_REDY = 0,
	DS1621_MESS_NOT_REDY
}DS1621MessStatus;

typedef struct{
	void     *I2Cdef;
	uint8_t  addressMS4B;
}interfaceAddressDef;

//   DS1621 command list
#define DS1621ComandReadTemperature   (uint8_t)0xAA
#define DS1621ComandAccessTH          (uint8_t)0xA1
#define DS1621ComandAccessTL          (uint8_t)0xA2
#define DS1621ComandAccessConfig      (uint8_t)0xAC
#define DS1621ComandReadCounter       (uint8_t)0xA8
#define DS1621ComandReadSlope         (uint8_t)0xA9
#define DS1621ComandStartConvertT     (uint8_t)0xEE
#define DS1621ComandStopConvertT      (uint8_t)0x22

#define DS1621_ADDRESS                 (uint8_t)10010000

typedef struct {
	uint8_t DONE:1;
	uint8_t THF:1;
	uint8_t THL:1;
	uint8_t NVB:1;
	uint8_t Reserved1:1;
	uint8_t Reserved2:1;
	uint8_t POL:1;
	uint8_t SHORT_1:1;
}configReg;

#define  BIT_DONE  	   (uint8_t)(1<<7)
#define  BIT_THF       (uint8_t)(1<<6)
#define  BIT_THL       (uint8_t)(1<<5)
#define  BIT_NVB       (uint8_t)(1<<4)
#define  BIT_POL       (uint8_t)(1<<1)
#define  BIT_SHORT_1   (uint8_t)(1<<0)

DS1621status ds1621GetConfig(interfaceAddressDef *interfaceAddress, uint8_t *DSConfig);
DS1621status ds1621SetConfig(interfaceAddressDef *interfaceAddress, uint8_t *DSConfig);
DS1621status ds1621OneMesSet(interfaceAddressDef *interfaceAddress);
DS1621status ds1621ContinueMesSet(interfaceAddressDef *interfaceAddress);
DS1621status ds1621ReadTemp(interfaceAddressDef *interfaceAddress, uint16_t *temperature_C);
DS1621status ds1621MesDone(interfaceAddressDef *interfaceAddress, DS1621MessStatus *messStatus);
DS1621status ds1621StartMess(interfaceAddressDef *interfaceAddress);
DS1621status ds1621ClearDONE(interfaceAddressDef *interfaceAddress);
DS1621status ds16211SHOT(interfaceAddressDef *interfaceAddress);

#endif /* HP03SA_SOURCE_H_ */
