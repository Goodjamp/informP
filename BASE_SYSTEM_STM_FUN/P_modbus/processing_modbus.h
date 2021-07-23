 /*
 * modbus.h
 *
 *  Created on: Apr 10, 2015
 *      Author: Gerasimchuk Alexandr
 */
#ifndef MODBUS_H_
#define MODBUS_H_

#include "stm32f10x_tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "processing_USART.h"
#include "USART_fifo_operation.h"
#include "funct.h"
#include "string.h"
#include "timers.h"
#include "init_system.h"
#include "processing_modbus_extern.h"
#include "processing_reset_control.h"

#define MODBUS_SIZE_BUFF_IN 20
#define MODBUS_SIZE_BUFF_OUT 150

#define TIME_BETWEEN_ERROR_REQ_MASTER     10

#define	SIZE_EXCEPTION                     5

#define	P_SHIFT(X,Y)   (X+Y)

typedef enum{
	REQ_MASTER_OK=0,
	REQ_MASTER_ERROR=10,
	REQ_MASTER_FUN_ADDRES_ERROR=11,
	REQ_MASTER_CRC_ERROR=12
}REZ_REQ_CHEACK_MASTER;


#pragma pack(push, 1)

typedef struct {
	S_Task_parameters *ps_task_parameters;
	u8                 num_modbus; //  MODBUS_MASTER, MODBUS_SLAVE
}S_modbus_tsk_par;

typedef struct{
	S_ulist_modbus         *ps_ulist_modbus;
	REZ_REQ_CHEACK_MASTER  rez_zapros;
	u8                     error_req_counter;
	u8                     non_req_count;
} S_ulist_req_modbus;


//  Nodbus Master
typedef struct{
	S_ulist_req_modbus s_ulist_req_modbus[MAX_NUM_MODBUS_REQ];
}S_list_madbus_req;



typedef struct {
	u8 byte_count;
	u8 payload;
}S_req_f16;

typedef union{
	u16 CRC_HiLo;
	S_req_f16 s_req_f16;
} UN_modbus_req_var;

typedef struct {
	u8 address_dev;
	u8 function;
	u8 address_mem_H;
	u8 address_mem_L;
    u8 number_reg_H;
	u8 number_reg_L;
	UN_modbus_req_var req_field_var;
}S_modbus_req_input;

typedef struct {
	u8 address_dev;
	u8 function;
	u8 address_mem_L;
	u8 address_mem_H;
	u8 num_reg_L;
	u8 num_reg_H;
	u8 CRC_H;
	u8 CRC_L;
}S_modbus_req_read;

typedef struct {
	u8 address_dev;
	u8 function;
	u8 address_mem_L;
	u8 address_mem_H;
	u8 forse_data_L;
	u8 forse_data_H;
	u8 CRC_H;
	u8 CRC_L;
}S_modbus_req_write;

typedef union{
	u8 type_exception;
	u8 num_byte_res;
} UN_modbus_excep_num;

typedef struct {
	u8 address_dev;
	u8 function;
	UN_modbus_excep_num un_excep_num;
	u8 resp_payload;
}S_modbus_make_res;

typedef u16 (*PF_procesing_req_slave)(void *, S_modbus_make_res *);

typedef REZ_REQ_CHEACK_SLAVE (*PF_modbus_callback_check)(uint16_t addressReg, uint16_t numberOfReg);

typedef struct{
	PF_modbus_callback_check pf_callback_check;
	u8 flag_callback_check;
} S_modbus_callback_check;

typedef u8 (*PF_add_processing_req_slave)(void*,u8,u16);

typedef struct{
	PF_add_processing_req_slave f_add_req_procesing;
	u8 flag_req_procesing;
} S_add_procesing_req;


#pragma pack(pop)

u8 modbus_fill_S_connectmodbus(u8 *p_read_data);
// ---MODBUS MASTER------------------------
INIT_MBB_Rezult modbus_fill_S_modbus_req(u8* p_req_config_data);
u8 Put_MODBUS(u8 *buf_in, S_connectmodbus *s_connectmodbus_port,S_ulist_req_modbus *ps_ulist_req_modbus, u16 status);
u16 ModMasterResTrs(S_Task_parameters *ptaskparameters,\
		u8 *buf_in, S_connectmodbus *pconnectmodbus, \
		S_ulist_req_modbus *ps_ulist_req);
void t_Modbus_MASTER(void *pvParameters);
void modbus_master_req(S_ulist_req_modbus *ps_ulist_req_modbus,S_connectmodbus *ps_connectmodbus,S_Task_parameters* ps_task_parameters);
// ---MODBUS SLAVE------------------------
REZ_REQ_CHEACK_SLAVE cheack_req_modbus(S_Task_parameters *p_iparameters, S_modbus_req_input *ps_modbus_req_cheack,u8 address_kp);
u16 func_0102(void *req, S_modbus_make_res *buf_respons);
u16 func_0304(void *req, S_modbus_make_res *buf_respons);
u16 func_05(void *req, S_modbus_make_res *buf_respons);
u16 func_06(void *p_req, S_modbus_make_res *p_buf_respons);
u16 func_16(void *req, S_modbus_make_res *buf_respons);
void t_Modbus_SLAVE(void *pvParameters);
u8 modbus_exception(S_modbus_req_input *ps_modbus_req, S_modbus_make_res *buf_respons,u8 exeption);
void modbus_callback_add_check(PF_modbus_callback_check p_modbus_callback_check_TY, u8 number_req);
void modbus_callback_address_check(PF_modbus_callback_check pf_modbus_callback_address_check, u8 number_req);
void modbus_callback_add_processing(PF_add_processing_req_slave p_modbus_callback_req, u8 number_req);


#endif /* MODBUS_H_ */
