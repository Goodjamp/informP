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

void clockInit(void);
void clockSetCallback(rtcCalbackDef secondCallbackIn, rtcCalbackDef alarmCallbackIn);
void clockSetTime(uint32_t UTCtime);
uint32_t clockGetTime(void);




#endif
