/*
 * file:   RTCProcessing.h
 * Authot: Gerasimchuk A.
 * Date:   19 jun 2017
 * Version 1.0
 */


#ifndef RTCPROCESSING_H_
#define RTCPROCESSING_H_

#include "stdint.h"

typedef void (*rtcCalbackDef)(uint32_t);

#define RTC_LSB_MASK     ((uint32_t)0x0000FFFF)  /*!< RTC LSB Mask */
#define PRLH_MSB_MASK    ((uint32_t)0x000F0000)  /*!< RTC Prescaler MSB Mask */

void clockInit(void);
void clockSetCallback(rtcCalbackDef secondCallbackIn, rtcCalbackDef alarmCallbackIn);
void clockSetTime(uint32_t UTCtime);
void clockSetAlarmTime(uint32_t UTCtime);
uint32_t clockGetTime(void);




#endif
