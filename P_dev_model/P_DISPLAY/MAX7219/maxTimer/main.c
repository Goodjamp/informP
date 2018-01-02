#include "stdint.h"
#include "../inc/lcdHWInterfaceTIMER.h"
#include "LCD.h"


uint8_t cnt = 0;
#define SIZE_TX_DATA  2
maxDataDef txLCDData[SIZE_TX_DATA] = {
    {0b1111111100000000, 0b0000000011111111},
    {0b1010101010101010, 0b0101010101010101}
};

int main(int argc, char *argv[])
{
uint8_t flag = 0;
    txData(txLCDData, SIZE_TX_DATA, 0, TX_ADDRESS_ONE);

    for(cnt=0; cnt < 1000; cnt++)
    {

        transmitCallback();
        if( !getState() )
        {
           if( flag)
           {
            return 0;
           }
           flag = 1;
        }
    }

   return 0;
}
