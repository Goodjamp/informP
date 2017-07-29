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

typedef struct{
    uint32_t sign:1;
    uint32_t mantisa:8;
    uint32_t data:23;
}floatBitField;

int main(int argc, char *argv[]){
    float a = 14.5;
    float b = 145;
    float c = 0.01;
    printf("a = %f\n", a);
    printf("b = %f\n", b);
    b =  b * c;
    printf("b = %f\n", b);

    GPRMC_Def myGPRMC;
    addGPSPars(GPRMC, &myGPRMC);
    parsGPS(&myGPRMC, (uint8_t*)data1, (uint8_t)strlen(data1));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS(&myGPRMC, (uint8_t*)data3, (uint8_t)strlen(data3));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS(&myGPRMC, (uint8_t*)data4, (uint8_t)strlen(data4));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS(&myGPRMC, (uint8_t*)data5, (uint8_t)strlen(data5));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS(&myGPRMC, (uint8_t*)data6, (uint8_t)strlen(data6));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
    parsGPS(&myGPRMC, (uint8_t*)data7, (uint8_t)strlen(data7));
    printf("gps Rx status =  %u\n", myGPRMC.procesingPars.statusGPS);
	return 0;
}


