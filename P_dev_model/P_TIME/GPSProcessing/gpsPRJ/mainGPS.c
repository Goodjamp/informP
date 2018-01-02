#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "..\GPSprocessing.h"

uint8_t data1[] = "asdsfasfas$GPR";
uint8_t data2[] = "MC,2254";
uint8_t data3[] = "46,A";
uint8_t data4[] = ",4916.45,N";
uint8_t data5[] = ",12311.12,W,000.";
uint8_t data6[] = "5,054.7,1911";
uint8_t data7[] = "94,020.3,E*68";


#define maxSetConfig(X,Y)    (( (uint8_t)X<<8 )|( (uint8_t)Y ))
union{
    uint8_t array[2];
    uint16_t a01;
}shortToarray;



int main(int argc, char *argv[]){

    GPRMC_Def myGPRMC;
    addGPSPars(GPRMC, &myGPRMC);
    parsGPS((void*)&myGPRMC, (uint8_t*)data1, (uint8_t)strlen((char*)data1));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS((void*)&myGPRMC, (uint8_t*)data3, (uint8_t)strlen((char*)data3));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS((void*)&myGPRMC, (uint8_t*)data4, (uint8_t)strlen((char*)data4));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS((void*)&myGPRMC, (uint8_t*)data5, (uint8_t)strlen((char*)data5));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS((void*)&myGPRMC, (uint8_t*)data6, (uint8_t)strlen((char*)data6));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS((void*)&myGPRMC, (uint8_t*)data7, (uint8_t)strlen((char*)data7));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
	return 0;
}


