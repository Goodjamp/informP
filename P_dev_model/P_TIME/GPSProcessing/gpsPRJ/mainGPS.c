#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "GPSprocessing.h"

uint8_t data[] = "$GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70";

int main(int argc, char *argv[]){
    GPRMC_Def myGPRMC;
    addGPSPars(GPRMC, &myGPRMC);
    parsGPS(&myGPRMC, (uint8_t*)data, (uint8_t)strlen(data));
    printf("myGPRMC.headSize =  %u\n", myGPRMC.procesingPars.headSize);
    printf("Hellow world\n");
	return 0;
}
