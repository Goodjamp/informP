/*
 * flash_operation.c
 *
 *  Created on: December 25, 2014
 *      Author: Gerasimchuk
 *      Versin: 1
 */

#include "stm32f10x_flash.h"
#include "stm32f10x.h"

#ifndef FLASH_OPERATION
#define FLASH_OPERATION


// Calculate absolute page addres
#define PAGE_ABS_ADDRESS(NUM_PAGE) (FLASH_BASE + 0x0400 * NUM_PAGE)


// ----------------Прототипы функций файла работы с flash-памятью------------------
void FLASH_OPERATION_read_flash_8b(u8 *p_buff, u32 length, u32 address);
void FLASH_OPERATION_read_flash_16b(u16 *data, u32 length, u32 address);
FLASH_Status FLASH_OPERATION_write_flash_page_16b(u16 *data, u32 length, u16 num_page);
FLASH_Status FLASH_OPERATION_write_flash_16b(u16 *data, u32 length, u32 address);
void FLASH_OPERATION_erase_page(u16 num_page);

#endif
