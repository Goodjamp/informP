#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "..\GPSprocessing.h"

uint8_t data[] = "$GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70";

int main(int argc, char *argv[]){
    GPRMC_Def myGPRMC;

    uint8_t data[]={5, 10};
    uint8_t *pdata = data;
    uint8_t data1 = *pdata++;
    printf("*pdata = %u\n",*pdata );
    printf("data1 = *pdata++; data1 = %u\n",data1 );
    printf("data[0] = %u\n",data[0] );
    printf("data[1] = %u\n",data[1] );
    addGPSPars(GPRMC, &myGPRMC);
    parsGPS(&myGPRMC, (uint8_t*)data, (uint8_t)strlen(data));
    printf("myGPRMC.headSize =  %u\n", myGPRMC.procesingPars.headSize);
	return 0;
}
typedef struct{
    float
}parsStar;
