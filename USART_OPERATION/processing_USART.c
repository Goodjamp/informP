/*
 * processing_USART.c
 *
 *  Created on: Feb 6, 2012
 *      Author: Gerasimchuk
 */
#include "stddef.h"

#include "processing_USART.h"



//----USART Tx/Rx bufers---------------------------------------------------
unsigned char Buf_USART_Transmit[NUMBER_USART][SIZE_OUT_BUFER];
unsigned char Buf_USART_Read[NUMBER_USART][SIZE_READ_BUFER];
// -------��������� ������������ ����� ������/��������---------------------------------
struct point_wrdirobject WrUSART_CODE[NUMBER_USART]; // ����� �������� ������������ ����� ��������. ������ ������������� ������ ������
struct point_wrdirobject RdUSART_CODE[NUMBER_USART]; //����� �������� ������������ ����� ������. ������ ������������� ������ ������

//------------��������� �� ������ ��������� �� ��������� ��������� �� ������ �������� �������
struct point_wrdirobject *WrUSART=&WrUSART_CODE[0];
struct point_wrdirobject *RdUSART=&RdUSART_CODE[0];

//------------����� �������� � ������� �������� ��������� �� ������ ������/�������� ---------------
S_Task_parameters task_parameters[NUMBER_MY_PROCES];
S_Task_parameters *ptask_parameters=&task_parameters[0];
//--------------------------------------------------------------------

//------------����� ���������� �� USART ������� ������������ �������� ������������, � ��� ������� ���������� ������ ��� ������ �������/��������-----------------------
USART_TypeDef *present_usart[NUMBER_USART]={PRESENT_USART};

volatile u32 counter_ms=0;
static u8 f_tim_init=0;

//--------------������� init_USART()-----------
// ������� init_USART() - ��������� ���������� ��������� ������ USART - ��������� ���������� ���������� USART
// �� ��� ������� ������/��������
void init_USART(USART_TypeDef *USART_in, u8 num_usart){

	// �������� ��������� ������� ������ (������ ������� ��������� �����������)
	(WrUSART+num_usart)->pBuf_UsartOut=Buf_USART_Transmit[num_usart];
	(WrUSART+num_usart)->size_wait_point_read=SIZE_WAIT_POINT_OUT_READ;
	(WrUSART+num_usart)->size_buffer=REAL_SIZE_OUT_BUFER;
	(WrUSART+num_usart)->pointwrite=0;
	(WrUSART+num_usart)->pointread=0;
	(WrUSART+num_usart)->num_write=0;
	(WrUSART+num_usart)->num_read=0;


	(RdUSART+num_usart)->pBuf_UsartOut=Buf_USART_Read[num_usart];
	(RdUSART+num_usart)->size_wait_point_read=SIZE_WAIT_POINT_READ_READ;
	(RdUSART+num_usart)->size_buffer=REAL_SIZE_READ_BUFER;
	(RdUSART+num_usart)->pointwrite=0;
	(RdUSART+num_usart)->pointread=0;
	(RdUSART+num_usart)->num_write=0;
	(RdUSART+num_usart)->num_read=0;

   //��������� ������ � ���������� ������� (������� ������, ������� ������)
	(ptask_parameters+num_usart)->USARTx=USART_in;
	(ptask_parameters+num_usart)->WrUSART=WrUSART+num_usart;
	(ptask_parameters+num_usart)->RdUSART=RdUSART+num_usart;

 // ��������� ������� ��� USART
	if(!f_tim_init){
		init_timer();
	}

};


void init_timer(){

	TIM_TimeBaseInitTypeDef user_timer_1;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/*�������� 72 (� ������ �������� �������� �������/���������=999 �������� ������ ���������� = 1 ��) */

	user_timer_1.TIM_Period = 999;			 //����� �������� ������ ��������� ���������� 1000 ���, ���������� ��������	������ ���� 999 ???
	user_timer_1.TIM_Prescaler = 72 - 1;	 //�������� ��������� ��� TIM_Prescaler+1  !!!
	user_timer_1.TIM_ClockDivision = TIM_CKD_DIV1;	//�� ���� ������� ������� ���� APB1 �� �������
	user_timer_1.TIM_CounterMode = TIM_CounterMode_Up;
	user_timer_1.TIM_RepetitionCounter = 0x0000;

	TIM_TimeBaseInit(TIM4, &user_timer_1);

	/* ���������� �� ������������ �������� TIM4*/
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4, ENABLE);

	NVIC_EnableIRQ(TIM4_IRQn);
}

//------------------------------------MY HANDLER INTERRAPTS---------------------------------------------
void USART1_IRQHandler(void)
{
    uint16_t	usingle;
    unsigned char singlchar;
	//--Proveryaem TXE

	if(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == SET)
	{
		if((read_fastdirstruct_usart(WrUSART,&singlchar))!=0)
		{
			usingle=((uint16_t)singlchar)&0x00ff;
			USART_SendData(USART1,usingle);
		}
		else
		{
			UsartClearStatusCR1(USART1,IT_TXEIE_USART);			//Nado zakrit port
		}
	}

	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET)
	{
		usingle=USART_ReceiveData(USART1);
		singlchar=(uint8_t)(usingle&0xff);
	    write_fastdirstruct_usart(RdUSART,singlchar);
	}

}

void USART2_IRQHandler(void)
{
	uint8_t singlchar;
	uint16_t	usingle;
	if(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == SET)
	{
		if((read_fastdirstruct_usart(WrUSART+1,&singlchar))!=0)
		{
			usingle=((uint16_t)singlchar)&0x00ff;
			USART_SendData(USART2,usingle);
		}
		else
			UsartClearStatusCR1(USART2,IT_TXEIE_USART);			//Nado zakrit port
	}

	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE) == SET)
	{
		usingle=USART_ReceiveData(USART2);
		singlchar=(uint8_t)(usingle&0xff);
		write_fastdirstruct_usart(RdUSART+1,singlchar);
	}
}

void USART3_IRQHandler(void)
{
	unsigned char singlchar;
	uint16_t	usingle;
	//--Proveryaem TXE
	if(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == SET)
	{
		if((read_fastdirstruct_usart(WrUSART+2,&singlchar))!=0)
		{
			usingle=((uint16_t)singlchar)&0x00ff;
			USART_SendData(USART3,usingle);

		}
		else
		{
			if(USART3->CR1 & IT_TXEIE_USART)
			{
			    UsartClearStatusCR1(USART3,IT_TXEIE_USART);			//Nado zakrit port
			}
		}
	}

	if(USART_GetFlagStatus(USART3,USART_FLAG_ORE) == SET)
	{
		(void)USART3->SR;
		(void)USART3->DR;
	}

	if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE) == SET)
	{
		usingle=USART_ReceiveData(USART3);
		singlchar=(uint8_t)(usingle&0xff);
		write_fastdirstruct_usart(RdUSART+2,singlchar);
	}
}
void TIM4_IRQHandler(void)
{
	//uint8_t	usinglechar;
	/*������� ��� ��������������� ����������*/
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	counter_ms++;
}


//
// Configure UART
//
USART_errort  ConfigureUSART(const S_port_config *ps_port_unit_config, USART_TypeDef *USART_init,
		                     S_Task_parameters **usartHandler){
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_TypeDef  *GPIO_USART_port_TX = NULL;
	GPIO_TypeDef  *GPIO_USART_port_RX = NULL;
	uint16_t GPIO_USART_pin_TX = 0;
	uint16_t GPIO_USART_pin_RX = 0;
	u8 counter;

	// ��� ������������� USART � ������ ���������
	for(counter=0;counter<NUMBER_USART;counter++){
		if(USART_init==present_usart[counter]){break;}
	}
	if(counter>=NUMBER_USART){return USART_ERROR_CONFIG;} // ���� �������������� USART ���� � ������ - ����� � �������
// ��������� ���� ������/�������� ���������� USART
	init_USART(USART_init, counter);

	//������� ������������ �������������� �-�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);


	// �������� � ����������� ������ �����

	//USART1: Tx (PA9)  Rx (PA10)
	//USART2: Tx (PA2)  Rx (PA3)
	//USART3: Tx (PB10) Rx (PB11)

	if (USART_init==USART1){
		GPIO_USART_port_TX=USART1_PORT_TX;
		GPIO_USART_port_RX=USART1_PORT_RX;
		GPIO_USART_pin_TX=USART1_PIN_TX;
		GPIO_USART_pin_RX=USART1_PIN_RX;
		// ������� ������������ USART1
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
		// ������� ������������ ����� �
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		// ������� ����������
		NVIC_EnableIRQ(USART1_IRQn);
	}
	else if(USART_init==USART2){
		GPIO_USART_port_TX=USART2_PORT_TX;
		GPIO_USART_port_RX=USART2_PORT_RX;
		GPIO_USART_pin_TX=USART2_PIN_TX;
		GPIO_USART_pin_RX=USART2_PIN_RX;
		// ������� ������������ USART2
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		// ������� ������������ ����� �
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		// ������� ����������
		NVIC_EnableIRQ(USART2_IRQn);
	}
	else if(USART_init==USART3){
		GPIO_USART_port_TX=USART3_PORT_TX;
		GPIO_USART_port_RX=USART3_PORT_RX;
		GPIO_USART_pin_TX=USART3_PIN_TX;
		GPIO_USART_pin_RX=USART3_PIN_RX;
		// ������� ������������ USART3
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		// ������� ������������ ����� �
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		// ������� ����������
		NVIC_EnableIRQ(USART3_IRQn);
	}
/*	else if(USART_init==USART4){
			GPIO_USART_port_TX=USART4_PORT_TX;
			GPIO_USART_port_RX=USART4_PORT_RX;
			GPIO_USART_pin_TX=USART4_PIN_TX;
			GPIO_USART_pin_RX=USART4_PIN_RX;
			// ������� ������������ USART4
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART4,ENABLE);
			// ������� ������������ ����� �
			RCC_APB2PeriphClockCmd(RCC_APB4Periph_GPIOB,ENABLE);
			// ������� ����������
			NVIC_EnableIRQ(USART4_IRQn);
		}
	else if(USART_init==USART5){
			GPIO_USART_port_TX=USART5_PORT_TX;
			GPIO_USART_port_RX=USART5_PORT_RX;
			GPIO_USART_pin_TX=USART5_PIN_TX;
			GPIO_USART_pin_RX=USART5_PIN_RX;
			// ������� ������������ USART5
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART5,ENABLE);
			// ������� ������������ ����� �
			RCC_APB2PeriphClockCmd(RCC_APB1Periph_GPIOB,ENABLE);
			// ������� ����������
			NVIC_EnableIRQ(USART5_IRQn);
		}
	else if(USART_init==USART6){
			GPIO_USART_port_TX=USART6_PORT_TX;
			GPIO_USART_port_RX=USART6_PORT_RX;
			GPIO_USART_pin_TX=USART6_PIN_TX;
			GPIO_USART_pin_RX=USART6_PIN_RX;
			// ������� ������������ USART6
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART6,ENABLE);
			// ������� ������������ ����� �
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
			// ������� ����������
			NVIC_EnableIRQ(USART6_IRQn);
		}
*/
	/* Configure USART Tx  as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_USART_pin_TX;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIO_USART_port_TX, &GPIO_InitStructure);

	/* Configure USART Rx  as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_USART_pin_RX;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_USART_port_RX, &GPIO_InitStructure);

	//------------------------------------------------------------------------


	//----------------------------- ��������� USART----------------------------
	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;

	USART_InitStructure.USART_BaudRate = ps_port_unit_config->baudrate;
	// ������ �����
	switch(ps_port_unit_config->amountBit){
	case 8:USART_InitStructure.USART_WordLength = USART_WordLength_8b;break;
	case 9:USART_InitStructure.USART_WordLength = USART_WordLength_9b;break;
	default:break;
	}
	// �-�� ���� �����
	switch(ps_port_unit_config->stopbits){
	case 1:USART_InitStructure.USART_StopBits = USART_StopBits_1;break;
	case 2:USART_InitStructure.USART_StopBits = USART_StopBits_2;break;
	default:break;
	}
	// ��� ��������
	switch(ps_port_unit_config->parity){
	case 0:USART_InitStructure.USART_Parity = USART_Parity_No;break;
	case 1:USART_InitStructure.USART_Parity = USART_Parity_Odd;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;break;
	case 2:USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;break;
	default:break;
	}

	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode =USART_Mode_Rx|USART_Mode_Tx;

	USART_Init(USART_init, &USART_InitStructure);
	USART_ClockInit(USART_init,&USART_ClockInitStruct);

	USART_ITConfig(USART_init, USART_IT_TXE, ENABLE);

	USART_ITConfig(USART_init, USART_IT_RXNE, ENABLE);

	/* Enable USART */
	USART_Cmd(USART_init, ENABLE);

    if (usartHandler != NULL) {
    	*usartHandler = &task_parameters[counter];
    }

	return USART_OK;
}

void Clrinbuf(S_Task_parameters *ptaskparameters,int MaxTime)
{
	unsigned int iRead=1;
	 uint8_t achar;
	while (iRead!=0)
	{
		iRead=ReadUSART(ptaskparameters->RdUSART,&achar,1,MaxTime);
	}
}

void Clrinbuf_without_time(S_Task_parameters *ptaskparameters)
{
	uint8_t achar;
	while ((Read_From(ptaskparameters->RdUSART,&achar,1))!=0)
	{

	}
}


