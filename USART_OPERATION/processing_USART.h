/*
 *processing_USART.h
 *
 *  Created on: Feb 6, 2012
 *      Author: Vitaliy
 */

#ifndef PROCESSING_USART_H_
#define PROCESSING_USART_H_


#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "USART_fifo_operation.h"
#include "processing_mem_map.h"


// ��������� ������ � ����� USART
// USART1
#define USART1_PORT_TX GPIOA
#define USART1_PORT_RX GPIOA
#define USART1_PIN_TX  GPIO_Pin_9
#define USART1_PIN_RX  GPIO_Pin_10
// USART2
#define USART2_PORT_TX GPIOA
#define USART2_PORT_RX GPIOA
#define USART2_PIN_TX  GPIO_Pin_2
#define USART2_PIN_RX  GPIO_Pin_3
// USART3
#define USART3_PORT_TX GPIOB
#define USART3_PORT_RX GPIOB
#define USART3_PIN_TX  GPIO_Pin_10
#define USART3_PIN_RX  GPIO_Pin_11

// ��������� ������� ������ USART
#define SIZE_OUT_BUFER						30
#define REAL_SIZE_OUT_BUFER 				29
#define SIZE_WAIT_POINT_OUT_READ			25

// ��������� ������� �������� USART
#define SIZE_READ_BUFER						100
#define REAL_SIZE_READ_BUFER 				100
#define SIZE_WAIT_POINT_READ_READ			100


#define WORKED_USART1						(1<<0)
#define WORKED_USART2						(1<<1)
#define WORKED_USART3						(1<<2)
#define WORKED_USART4						(1<<3)
#define WORKED_USART5						(1<<4)
#define WORKED_USART6						(1<<5)


//----------DEFINE FOR PROCESS----------------------------------------
#define NUMBER_MY_PROCES					NUMBER_USART


// ---------������������ ������-----------
typedef enum{
	USART_OK=0,
	USART_ERROR_CONFIG=1
}USART_errort;

#pragma pack(push, 1)
// ��� ������� ��������� ������� ��������� ��������� �������, ������� �������� � USART
typedef struct
{
	USART_TypeDef                   *USARTx;            // ��������� �� USART � ������� �������� ������ ���������
	struct point_wrdirobject 		*WrUSART;           // ��������� ������� ��������
	struct point_wrdirobject 		*RdUSART;           // ��������� ������� ������
 }S_Task_parameters;

// ��� ������� ��������� ������� ��������� ��������� �������, ������� �������� � USART
typedef struct {
	u16 baudrate;              // (A)    ��������
	u8 stopbits;               // (A)    �-�� �������� �����
	u8 parity;                 // (A)    ��������� ��������
	u8 amountBit;             // (A)    ������ �-�� ��� � ����� �����
	u8 controlpotok;           // (S)    ��������� ���������� ����������� ������������� �����
 } S_port_config;


#pragma pack(pop)

void init_timer();
void init_USART(USART_TypeDef *USART_in, u8 num_usart);
USART_errort  ConfigureUSART(const S_port_config *ps_port_unit_config, USART_TypeDef *USART_init,
		                     S_Task_parameters **usartHandler);
void Clrinbuf(S_Task_parameters *ptaskparameters,int MaxTime);
void Clrinbuf_without_time(S_Task_parameters *ptaskparameters);

#endif /* MYPROG_H_ */
