/*
 * processing_mem_map.c
 *
 *  Created on: December 29, 2014
 *      Author: Gerasimchuk
 *      Version: 1
 */
#include "processing_mem_map_dev.h"
#include "processing_mem_map.h"
#include "processing_mem_map_extern.h"

//��������� � �������� �������� (�����������������, ���������, ����������������) ����� ������
S_mem_map s_mem_map;
// ��������� ������� ����������� ���������
S_address_oper_data s_address_oper_data;
// ������� ������ ��� ����� ������
u8 mem_map[SIZE_MEM_MAP]={[0 ... (SIZE_MEM_MAP-1)]=0};
// �������� �� ������ ����� ������, ����������� � proces_object
S_proces_object_modbus *p_proces_object;
// ������ ������� � ����� ������, ����������
xSemaphoreHandle h_mutex_mem_map_axes;




//-----�������  processing_mem_map_init ---------
// ������� ��������� ������������ � ������������� ����� ������
void processing_mem_map_init(void){
	// �������� �� ������ ����� ������, ����������� � proces_object
	p_proces_object=(S_proces_object_modbus*)mem_map;
	// ������ ������ ��� ������� � MEM MAP
	h_mutex_mem_map_axes=xSemaphoreCreateMutex();
	// ����������� ����� ������
	processing_mem_map_init_mem_map();

}


//-----------init_mem_map--------------
// ������������� ����� ������: �������� ������� ��������� ��, ��, ����������� ���������, ������� ���������
//
u8 processing_mem_map_init_mem_map(void) {
	u16 temp_address;
	// ����� ��������� ����� ������
	s_mem_map.size_mem_map = SIZE_MEM_MAP;
	s_mem_map.ps_proces_object_mem_map = (u8*) &mem_map[0];
	s_mem_map.p_start_DATA = s_mem_map.ps_proces_object_mem_map;  // ��������� �� ������ ������� ����� - ����� � ����� ������
	s_mem_map.p_start_oper_data = s_mem_map.p_start_DATA + NUM_REG_DATA*SIZE_PROCES_OBJECT; // ��������� �� ������ ����������� ��������� � ����� ������
	s_mem_map.p_start_config_data=s_mem_map.p_start_oper_data+SIZE_OPER_DATA; //  ��������� �� ������ ���������������� ��������� � ����� ������

	//������ ������� ������� �������� ���������, ������� ������ � ������ ������� (����������� ���������)
	s_address_oper_data.s_address_devise_reset_status.address_device_rcr=USER_ADDRESS_OPER_DATA;// ��������� ����� ������� ����������� ��������� - ������� ������� ����������
	s_address_oper_data.s_address_devise_reset_status.address_device_statys=s_address_oper_data.s_address_devise_reset_status.address_device_rcr+1;
	s_address_oper_data.s_modbus_address.status_PORT=s_address_oper_data.s_address_devise_reset_status.address_device_statys+1;      //��������� ����� ��������� ������� ������ (������ modbus)
	temp_address=s_address_oper_data.s_modbus_address.status_PORT+NUM_PORT_MODBUS; //
	// ��������� ��������� �������� � ��������� ��������� ����� �����
#ifdef DEV_0
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_0)(&s_address_oper_data.USER_ADDRESS_FIELD(s,DEV_0),temp_address);

#endif
#ifdef DEV_1
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	//temp_address=CALC_ADDRESS_OPER                 _REG_PROCESSING(DEV_1)(&s_address_oper_data.USER_ADDRESS_FIELD(s,DEV_1),temp_address);
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_1,temp_address);
#endif
#ifdef DEV_2
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	//temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_2)(&s_address_oper_data.USER_ADDRESS_FIELD(s,DEV_2),temp_address);
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_2,temp_address);
#endif
#ifdef DEV_3
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	//temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_3)(&s_address_oper_data.USER_ADDRESS_FIELD(s,DEV_3),temp_address);
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_3,temp_address);
#endif
#ifdef DEV_4
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_4,temp_address);

#endif
#ifdef DEV_5
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_5,temp_address);

#endif
#ifdef DEV_6
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_6,temp_address);
#endif
#ifdef DEV_7
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_7,temp_address);

#endif
#ifdef DEV_8
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_8,temp_address);

#endif
#ifdef DEV_9
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_9,temp_address);

#endif
#ifdef DEV_10
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_10,temp_address);

#endif
#ifdef DEV_11
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_11,temp_address);

#endif
#ifdef DEV_12
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_12,temp_address);

#endif
#ifdef DEV_13
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_13,temp_address);

#endif
#ifdef DEV_14
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_14,temp_address);

#endif
#ifdef DEV_15
	// ������� �-� �������� ������� ����������� ��������� ���������� (������ ������������)
	temp_address=CALC_ADDRESS_OPER_REG_PROCESSING(DEV_15,temp_address);

#endif

	return 0;
}

// ---------�������  processing_mem_map_write_s_proces_object_modbus---------
// ������� processing_mem_map_write_s_proces_object_modbus ���������� ������ � ����� ������
// �� ��������� ������ ������������
ERROR_mem processing_mem_map_write_s_proces_object_modbus( const u16 *p_data,const u16 num_data, const u16 address) {
	u16 k1;
	u8 *p_addres_to;
	u16* p_data_field;
	TYPE_REG_MEM_MAP type_reg_start, type_reg_stop;

	type_reg_stop=processing_mem_map_detect(address+num_data-1, &p_addres_to);
	type_reg_start=processing_mem_map_detect(address, &p_addres_to);
	// (���� ������ �� ����������� � ������������� ������) ��� (���� ������� ������� �� ������ ����� ��������) - ������� � �����������,
	if((type_reg_start==ERROR_DATA)||(type_reg_stop!=type_reg_start)){
		return MEM_ERROR;
	}
	// �������� ������ � ����� ������
	xSemaphoreTake(h_mutex_mem_map_axes, portMAX_DELAY); // ���� ������, ������� ����������� ���������� �����

	for(k1=0;k1<num_data;k1++){
		if((type_reg_start==OPER_DATA)||((type_reg_start==PMZ_DATA))){
			p_data_field=(u16*)&(((S_proces_object_modbus*)p_addres_to)->date[0]);
			*(p_data_field)=*(p_data);
			p_addres_to+=sizeof(S_proces_object_modbus);
		}
		else if(type_reg_start==CONFIG_DATA){ //
			*((u16*)p_addres_to)=*(p_data);
			p_addres_to+=2;
		}
		p_data++;
	}
	xSemaphoreGive(h_mutex_mem_map_axes);        // ����� ������
	return MEM_OK;
}


// ---------�������  fill_s_proces_object_modbust---------
// ������� fill_s_proces_object_modbus ������ ���� ��������� _proces_object_
// ������� ���������
// p_data - ��������� �� ����� ����������
// num_data - �-�� ��������� ����������
// address - ����� ���������� �������� ���������� �� ������� ������������
ERROR_mem processing_mem_map_read_s_proces_object_modbus( u16 *p_data, u16 num_data, u16 address) {
	u16 k1;
	u8 *p_addres_from;
	TYPE_REG_MEM_MAP type_reg_start, type_reg_stop;
	//
	type_reg_stop=processing_mem_map_detect(address+num_data-1, &p_addres_from);
	type_reg_start=processing_mem_map_detect(address, &p_addres_from);
	// (���� ������ �� ����������� � ������������� ������) ��� (���� ������� ������� �� ������ ����� ��������) - ������� � �����������,
	if((type_reg_start==ERROR_DATA)||(type_reg_stop!=type_reg_start)){
		return MEM_ERROR;
	}
	// - ������� -
	xSemaphoreTake(h_mutex_mem_map_axes, portMAX_DELAY); // ���� ������, ������� ����������� ���������� �����
	for(k1=0;k1<num_data;k1++){
		if((type_reg_start==OPER_DATA)||((type_reg_start==PMZ_DATA))){
			p_data[k1]=*((u16*)&(((S_proces_object_modbus*)p_addres_from)->date[0]));
			p_addres_from+=sizeof(S_proces_object_modbus);
		}
		else if(type_reg_start==CONFIG_DATA){ // ������� ������ ������� ���� ������� �� ����
			p_data[k1]=*((u16*)p_addres_from);
			p_addres_from+=sizeof(u16);
		}
	}
	xSemaphoreGive(h_mutex_mem_map_axes);   // ����� ������

	return MEM_OK;
}



// ---------�������  processing_mem_map_detect ---------
// ������� processing_mem_map_detect - ������� ����������� ���� �������������� ������ ������������� �� ����� ������, � ������� ������ �����
// �������� � ������ ��
// data_user_addres - ����� ������� ������������, ������� ���������� ��������������
// p_addres_mem_map - ��������� �� ������, � �������� ����������� ������������� ����� ��� �����������
TYPE_REG_MEM_MAP processing_mem_map_detect(u16 data_user_addres, u8** p_addres_mem_map){

	// ������m ��������� �����/������
	if	((data_user_addres >= USER_ADDRESS_DATA)
			&& (data_user_addres < (USER_ADDRESS_DATA + NUM_REG_DATA)))
	{
		*p_addres_mem_map=(u8*)(s_mem_map.p_start_DATA+data_user_addres*SIZE_PROCES_OBJECT);
		return(PMZ_DATA);
	}
	// ������ ����������� ���������
	else if	((data_user_addres >= USER_ADDRESS_OPER_DATA)
			&& (data_user_addres < USER_ADDRESS_CONFIG_DATA))
	{

		*p_addres_mem_map=(u8*)(s_mem_map.p_start_oper_data+(data_user_addres-USER_ADDRESS_OPER_DATA)*SIZE_PROCES_OBJECT);
		return(OPER_DATA);
	}

	// ������� ��������� ����
	else if ((data_user_addres>= USER_ADDRESS_CONFIG_DATA)
			&& (data_user_addres < (USER_ADDRESS_CONFIG_DATA+NUM_REG_CONFIG)))
	{
		*p_addres_mem_map=(u8*)(s_mem_map.p_start_config_data+(data_user_addres-USER_ADDRESS_CONFIG_DATA)*SIZE_REG_CONFIG);
		return(CONFIG_DATA);
	}

	return(ERROR_DATA);
}

