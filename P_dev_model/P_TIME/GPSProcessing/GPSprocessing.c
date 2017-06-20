#include "GPSprocessing.h"

#include "stdio.h"

// Project my headers
#include "processing_USART.h"
#include "USART_fifo_operation.h"
#include "GPSprocessing.h"

u8 s_gprmc[]={GPRMC_HEADR};
u32 rtc_isr_state;
uint8_t PMTK314[] = "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29`";
extern *ptaskparameters =

void initUSARTGPS(void){
	S_port_config portConfig={
			.baudrate = gpsUSARTSpeed,
			.stopbits = gpsUSARTStopBits,
			.parity = gpsUSARTParity,
			.amountbyte = gpsUSARTWordLengt,
			.controlpotok = 0
	};
	ConfigureUSART(&portConfig, gpsUSARTDef);
}

void getMessageGPS(void){
	while(1){
		ReadUSART(ptaskparameters->RdUSART, (u8*) ps_modbus_req_input, 8,20) != 8);
	}
}
