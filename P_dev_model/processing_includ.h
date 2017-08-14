/*
 * processing_includ.h
 *
 *  Created on: May 28, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_INCLUD_H_
#define PROCESSING_INCLUD_H_


#define MAX_T_PRIORITY    4
#define MIN_T_PRIORITY    1


// ���������� ����� (������ �������� FreeRTOS)
// ������ ���� � ��������� MIN_T_PRIORITY-MAX_T_PRIORITY
#define DEV_0_T_PRIORITY  1
#define DEV_1_T_PRIORITY  1
#define DEV_2_T_PRIORITY  1
#define DEV_3_T_PRIORITY  1
#define DEV_4_T_PRIORITY  1
#define DEV_5_T_PRIORITY  1
#define DEV_6_T_PRIORITY  1
#define DEV_7_T_PRIORITY  1
#define DEV_8_T_PRIORITY  1
#define DEV_9_T_PRIORITY  1
#define DEV_10_T_PRIORITY  1
#define DEV_11_T_PRIORITY  1
#define DEV_12_T_PRIORITY  1
#define DEV_13_T_PRIORITY  1
#define DEV_14_T_PRIORITY  1

// ������������ ����� ���������� �������
#include "processing_FRQmetter_extern.h"
#include "processing_modbus_extern.h"
#include "processing_TIME_extern.h"
#include "processing_sensor_extern.h"

#endif
