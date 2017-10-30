/*
*  DS1621_sourse.c
 *
 *  Created on: July 30, 2017
 *      Author: Gerasimchuk
 *      Versin: 1
 */
#include "stdint.h"

#include "DS1621_source.h"
#include "i2c_source.h"

DS1621status ds1621GetConfig(interfaceAddressDef *interfaceAddress, uint8_t *DSConfig){
	if( i2c_read_data(interfaceAddress->I2Cdef,
			DS1621_ADDRESS | interfaceAddress->addressMS4B,
			DS1621ComandAccessConfig, 1,DSConfig) != I2C_STATUS_OK)
	{
		return DS1621_STATUS_BUSS_ERROR;
	}
	return DS1621_STATUS_OK;
}

DS1621status ds1621SetConfig(interfaceAddressDef *interfaceAddress, uint8_t *DSConfig){
	if( i2c_write_data(interfaceAddress->I2Cdef,
			DS1621_ADDRESS | interfaceAddress->addressMS4B,
			DS1621ComandAccessConfig, 1,DSConfig) != I2C_STATUS_OK)
	{
		return DS1621_STATUS_BUSS_ERROR;
	}
	return DS1621_STATUS_OK;
}

DS1621status ds1621OneMesSet(interfaceAddressDef *interfaceAddress){
	uint8_t currentConfig = 0;
	DS1621status rezStatus = DS1621_STATUS_OK;
	rezStatus = ds1621GetConfig(interfaceAddress, &currentConfig);
	if(rezStatus != DS1621_STATUS_OK){
		return DS1621_STATUS_BUSS_ERROR;
	}

	currentConfig |= BIT_SHORT_1;

	rezStatus = ds1621SetConfig(interfaceAddress, &currentConfig);
	if(rezStatus != DS1621_STATUS_OK){
		return DS1621_STATUS_BUSS_ERROR;
	}
	return DS1621_STATUS_OK;
}

DS1621status ds1621ContinueMesSet(interfaceAddressDef *interfaceAddress){
	uint8_t currentConfig = 0;
	DS1621status rezStatus = DS1621_STATUS_OK;
	rezStatus = ds1621GetConfig(interfaceAddress, &currentConfig);
	if(rezStatus != DS1621_STATUS_OK){
		return DS1621_STATUS_BUSS_ERROR;
	}

	currentConfig &= ~BIT_SHORT_1;

	rezStatus = ds1621SetConfig(interfaceAddress, &currentConfig);
	if(rezStatus != DS1621_STATUS_OK){
		return DS1621_STATUS_BUSS_ERROR;
	}
	return DS1621_STATUS_OK;
}


DS1621status ds16211SHOT(interfaceAddressDef *interfaceAddress){
	uint8_t currentConfig = 0;
	DS1621status rezStatus = DS1621_STATUS_OK;
	rezStatus = ds1621GetConfig(interfaceAddress, &currentConfig);
	if(rezStatus != DS1621_STATUS_OK){
		return DS1621_STATUS_BUSS_ERROR;
	}

	currentConfig |= BIT_SHORT_1;

	rezStatus = ds1621SetConfig(interfaceAddress, &currentConfig);
	if(rezStatus != DS1621_STATUS_OK){
		return DS1621_STATUS_BUSS_ERROR;
	}
	return DS1621_STATUS_OK;
}

DS1621status ds1621ClearDONE(interfaceAddressDef *interfaceAddress){
	uint8_t currentConfig = 0;
	DS1621status rezStatus = DS1621_STATUS_OK;
	rezStatus = ds1621GetConfig(interfaceAddress, &currentConfig);
	if(rezStatus != DS1621_STATUS_OK){
		return DS1621_STATUS_BUSS_ERROR;
	}
	currentConfig &= ~(BIT_DONE);

	rezStatus = ds1621SetConfig(interfaceAddress, &currentConfig);
	if(rezStatus != DS1621_STATUS_OK){
		return DS1621_STATUS_BUSS_ERROR;
	}
	return DS1621_STATUS_OK;
}

DS1621status ds1621ReadTemp(interfaceAddressDef *interfaceAddress, uint16_t *temperature_C){
	if( i2c_read_data(interfaceAddress->I2Cdef,
			DS1621_ADDRESS | interfaceAddress->addressMS4B,
			DS1621ComandReadTemperature,
			2,
			(uint8_t*)temperature_C) != I2C_STATUS_OK)
	{
		return DS1621_STATUS_BUSS_ERROR;
	}

	*temperature_C = (((uint8_t*)temperature_C)[1] >0 ) ? ( ((uint8_t*)temperature_C)[1] = 0, (*temperature_C * 10 + 5) ) : ( *temperature_C * 10  );
	return DS1621_STATUS_OK;
}

DS1621status ds1621MesDone(interfaceAddressDef *interfaceAddress, DS1621MessStatus *messStatus){
	uint8_t configReg;
	if( i2c_read_data(interfaceAddress->I2Cdef,
			DS1621_ADDRESS | interfaceAddress->addressMS4B,
			DS1621ComandAccessConfig,
			1,
			&configReg) != I2C_STATUS_OK)
	{
		return DS1621_STATUS_BUSS_ERROR;
	}

	*messStatus = (configReg & BIT_DONE) ? DS1621_MESS_REDY : DS1621_MESS_NOT_REDY;

	return DS1621_STATUS_OK;
}

DS1621status ds1621StartMess(interfaceAddressDef *interfaceAddress){
	if( i2c_read_data(interfaceAddress->I2Cdef,
			DS1621_ADDRESS | interfaceAddress->addressMS4B,
			DS1621ComandStartConvertT,
			0,
			(uint8_t*)0) != I2C_STATUS_OK)
	{
		return DS1621_STATUS_BUSS_ERROR;
	}

	return DS1621_STATUS_OK;
}














