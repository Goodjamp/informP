/********************************************************************************
  * @file    BME280_source.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    19-Octobre-2017
  * @brief
  */
#include "stdint.h"

#ifndef BME_280_HAL_H_
#define BME_280_HAL_H_

//register ID definition
#define BME280_ID      0x60
// acquisition
//register RESET definition
#define BME280_RESET   0xB6

typedef enum{
	BME280_REG_HUM_LSB    = 0xFE,
	BME280_REG_HUM_MSB    = 0xFD,
	BME280_REG_TEMP_XLSB  = 0xFC,
	BME280_REG_TEMP_LSB   = 0xFB,
	BME280_REG_TEMP_MLSB  = 0xFA,
	BME280_REG_PRES_XLSB  = 0xF9,
	BME280_REG_PRES_LSB   = 0xF8,
	BME280_REG_PRES_MLSB  = 0xF7,
	BME280_REG_CONFIG     = 0xF5,
	BME280_REG_CTRL_MES   = 0xF4,
	BME280_REG_STATUS     = 0xF3,
	BME280_REG_CTRL_HUM   = 0xF2,
	BME280_REG_CALIB26_41 = 0xE1,
	BME280_REG_RESET      = 0xE0,
	BME280_REG_ID         = 0xD0,
	BME280_REG_CALIB00_25 = 0x88,
}BME280_REG;

/*-----------------fields of register CTRL_HUM definition-----------------*/
/*osr_h*/
#define CTRL_HUM_OSR_H_SKIPPED   uint8_t(0x0)
#define CTRL_HUM_OSR_H_OVERx1    uint8_t(0x1)
#define CTRL_HUM_OSR_H_OVERx2    uint8_t(0x2)
#define CTRL_HUM_OSR_H_OVERx4    uint8_t(0x3)
#define CTRL_HUM_OSR_H_OVERx8    uint8_t(0x4)
#define CTRL_HUM_OSR_H_OVERx16   uint8_t(0x5)

//---fields of register STATUS definition---
/*measuring - Automatically set to ‘1’ whenever a conversion is running
and back to ‘0’ when the results have been transferred to the data registers*/
#define STATUS_MEASURING         uint8_t(0x1<<3)
/*im_update - Automatically set to ‘1’ when the NVM data are being
copied to image registers and back to ‘0’ when the copying is done*/
#define STATUS_IM_UPDATE         uint8_t(0x1<<0)

/*-----------------fields of register CTRL_MEAS definition-----------------*/
/*osrs_t - Controls oversampling of temperature data*/
#define  CTRL_MEAS_OSRS_T_SKIPED    uint8_t(0x0<<5)
#define  CTRL_MEAS_OSRS_T_OVERx1    uint8_t(0x1<<5)
#define  CTRL_MEAS_OSRS_T_OVERx2    uint8_t(0x2<<5)
#define  CTRL_MEAS_OSRS_T_OVERx4    uint8_t(0x3<<5)
#define  CTRL_MEAS_OSRS_T_OVERx8    uint8_t(0x4<<5)
#define  CTRL_MEAS_OSRS_T_OVERx16   uint8_t(0x5<<5)
/*osrs_p - Controls oversampling of pressure data*/
#define  CTRL_MEAS_OSRS_P_SKIPED    uint8_t(0x0<<2)
#define  CTRL_MEAS_OSRS_P_OVERx1    uint8_t(0x1<<2)
#define  CTRL_MEAS_OSRS_P_OVERx2    uint8_t(0x2<<2)
#define  CTRL_MEAS_OSRS_P_OVERx4    uint8_t(0x3<<2)
#define  CTRL_MEAS_OSRS_P_OVERx8    uint8_t(0x4<<2)
#define  CTRL_MEAS_OSRS_P_OVERx16   uint8_t(0x5<<2)
/*mode - Controls the sensor mode of the device*/
#define  CTRL_MEAS_MODE_SLEEP       uint8_t(0x0)
#define  CTRL_MEAS_MODE_FORCED      uint8_t(0x1)
#define  CTRL_MEAS_MODE_NORMAL      uint8_t(0x2)

/*-----------------fields of register CONFIG definition-----------------*/
/*t_sb - Controls inactive duration tstandby in normal mode*/
#define  CONFIG_T_SB_O_5    uint8_t(0x0<<5)
#define  CONFIG_T_SB_65_5   uint8_t(0x1<<5)
#define  CONFIG_T_SB_125_0  uint8_t(0x2<<5)
#define  CONFIG_T_SB_250_0  uint8_t(0x3<<5)
#define  CONFIG_T_SB_500_0  uint8_t(0x4<<5)
#define  CONFIG_T_SB_1000_0 uint8_t(0x5<<5)
#define  CONFIG_T_SB_10_0   uint8_t(0x6<<5)
#define  CONFIG_T_SB_20_0   uint8_t(0x7<<5)
/*filter - Controls the time constant of the IIR filter*/
#define  CONFIG_FILTER_OFF  uint8_t(0x0<<2)
#define  CONFIG_FILTER_2    uint8_t(0x1<<2)
#define  CONFIG_FILTER_4    uint8_t(0x2<<2)
#define  CONFIG_FILTER_8    uint8_t(0x3<<2)
#define  CONFIG_FILTER_16   uint8_t(0x4<<2)

#pragma pack(push,1)
typedef struct{
	uint16_t  dig_T1;
	int16_t  dig_T2;
	int16_t  dig_T3;
	uint16_t  dig_P1;
	int16_t  dig_P2;
	int16_t  dig_P3;
	int16_t  dig_P4;
	int16_t  dig_P5;
	int16_t  dig_P6;
	int16_t  dig_P7;
	int16_t  dig_P8;
	int16_t  dig_P9;
	uint8_t   dig_H1;
	int16_t  dig_H2;
	uint8_t   dig_H3;
	int16_t  dig_H4;
	int16_t  dig_H5;
	int8_t   dig_H6;
}BME280_calibCoef;
#pragma pack(pop)


#endif
