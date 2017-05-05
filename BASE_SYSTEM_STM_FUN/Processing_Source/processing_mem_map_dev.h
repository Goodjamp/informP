/*
 * processing_mem_map_dev.h
 *
 *  Created on: April 19, 2015
 *      Author: Gerasimchuk
 *      Versin: 1
 */

// ���� ���� ������ ��� ���� ����� �������� ������������ �������, � ������
// ���������������� "SIZE_MEM_MAP" ������������� � ����� "processing_mem_map.c", �� ���
// ��� ������� ����� ���������������� � ����� "processing_config_dev.h" � "processing_mem_map.h"
// �� � ����� "processing_config_dev.h" ��������� ���� "processing_mem_map.h", �������
// ���� ���������������� "SIZE_MEM_MAP" ������� � ����� "processing_mem_map.h", �� ����� ���������� ����
// "processing_config_dev.h", � ������� ��������� ���� "processing_mem_map.h" - �������� ������������
// ������ � ������ (����������� �� �������� ��� ���� ��������� ������)

#ifndef PROCESSING_MEM_MAP_DEV_H_
#define PROCESSING_MEM_MAP_DEV_H_

#include "stm32f10x.h"
#include "processing_mem_map.h"
#include "processing_config_dev.h"

// ������ ����� ������ (�������� ������,����������� �������� � �������� ������������)
#define SIZE_MEM_MAP             NUM_REG_DATA*SIZE_PROCES_OBJECT+sizeof(S_oper_data)+NUM_REG_CONFIG*SIZE_REG_CONFIG

#endif
