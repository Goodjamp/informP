/*
 * global_dev.h
 *
 *  Created on: March 26, 2015
 *      Author: Gerasimchuk
 */

#ifndef GLOBAL_DEV_H_
#define  GLOBAL_DEV_H_

// ������ ��� ����������� ������ ������ ���������� �� ����������������
#define MY_FAIL(X)        _MY_FAIL(X)
#define _MY_FAIL(NAME)   <processing_##NAME.h>

// ������ ��� ��������������� ����� ��������� ������������
#define USER_CONFIG_FIELD(Y,X)         _USER_CONFIG_FIELD(Y,X)
#define _USER_CONFIG_FIELD(TYPE,NAME)     TYPE##_##NAME##_user_config

// ������ ��� ��������������� ����� ��������� ������� ����������� ���������
#define USER_ADDRESS_FIELD(Y,X)         _USER_ADDRESS_FIELD(Y,X)
#define _USER_ADDRESS_FIELD(TYPE,NAME)     TYPE##_##NAME##_address

// ������ ��� ��������������� ����� ��������� ����������� ���������
#define USER_OPER_DATA_FIELD(Y,X)         _USER_OPER_DATA_FIELD(Y,X)
#define _USER_OPER_DATA_FIELD(TYPE,NAME)     TYPE##_##NAME##_oper_data

// ������ ��� ��������������� ����� �������-������
#define TASK_PROCESSING(X)         _TASK_PROCESSING(X)
#define _TASK_PROCESSING(NAME)     t_processing_##NAME

// ������ ��� ��������������� ����� �������-�������� �������� ����������� ��������� ������������� ����������
#define CALC_ADDRESS_OPER_REG_PROCESSING(X,Y)         _CALC_ADDRESS_OPER_REG_PROCESSING(X,Y)
#define _CALC_ADDRESS_OPER_REG_PROCESSING(NAME,Y)     NAME##_calc_address_oper_reg(&s_address_oper_data.USER_ADDRESS_FIELD(s,NAME),Y)

// ������ ��� ��������������� ������ � �������
#define TASK_IDENT(X)    _TASK_IDENT(X)
#define _TASK_IDENT(X)    #X

//������ ��� ��������������� ������ ���������� ������
#define TASK_PRIORITY(X)   X##_T_PRIORITY

//������ ��� ��������������� �����-��������� �� USART
#define USART(X)   _USART(X)
#define _USART(NUM)   USART##NUM


#endif
