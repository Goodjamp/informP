/*
 * processing_simple_gpio.h
 *
 *  Created on: Jun 8, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#ifndef PROCESSING_SIMPLE_GPIO_H_
#define PROCESSING_SIMPLE_GPIO_H_
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


// ��������� ���������
// ����� ��������� ������ ������ ������������ (��_���������/����������������)
#define PORT_JAMP1         GPIOB
// ����� ��������� ������ ������ �������� ��(���_�������� ������� �����/�� ����������)
#define PORT_JAMP2         GPIOA

// ���� ���������
#define PIN_JAMP1           GPIO_Pin_3
#define PIN_JAMP2           GPIO_Pin_15

// ��������� ������������ ���������
// ����� ����������� ���������
#define PORT_IND_ERROR         GPIOB

// ���� ����������� ���������
#define PIN_IND_ERROR          GPIO_Pin_8


// ---------- ������� ������ ��������� --------------------------------------
#define STATE_JAMPER1          GPIO_ReadInputDataBit(PORT_JAMP1,PIN_JAMP1)
#define STATE_JAMPER2          GPIO_ReadInputDataBit(PORT_JAMP2,PIN_JAMP2)

// ---------- ������� ���������� ���������� --------------------------------------
#define IND_OUT_INVERSE
#ifdef IND_OUT_INVERSE
// ���� ���������� ��������� (��� 0 ������������� ��� �����)
#define IND_ERROR_RESET          GPIO_SetBits(PORT_IND_ERROR,PIN_IND_ERROR);
#define IND_ERROR_SET            GPIO_ResetBits(PORT_IND_ERROR,PIN_IND_ERROR);
#else
// ���� ���������� ���������� (��� 1 ������������� ���� �����)
#define IND_ERROR_RESET          GPIO_ResetBits(PORT_IND_ERROR,PIN_IND_ERROR);
#define IND_ERROR_SET            GPIO_SetBits(PORT_IND_ERROR,PIN_IND_ERROR);
#endif





//-------------��������� �������--------------
void processing_simple_gpio(void);
void work_jamper_init(void);
void work_ind_init(void);
void remap_out_pin(void);

#endif
