#include "stdint.h"
#include "stdbool.h"
#include "LCD.h"

#define NUM_NAX_PER_SCREEN  2

struct{
    maxDataDef  *data;
    uint8_t     dataSize;
    uint8_t     maxCnt;
    uint8_t     dataCnt;
    uint8_t     orderNumberDispl;
    TX_ADDRESS  txAddress;
}dataForTx;

void txData(maxDataDef *data, uint8_t dataSize, uint16_t orderNumberDispl, TX_ADDRESS txAddress){
     dataForTx.dataCnt  = 0;
     dataForTx.maxCnt   = 0;
     dataForTx.data     = data;
     dataForTx.dataSize = dataSize;
     dataForTx.orderNumberDispl = orderNumberDispl;
     dataForTx.txAddress = txAddress;
     hwInterfaceTx(dataForTx.orderNumberDispl, dataForTx.txAddress);
}

txState getNextData(uint16_t *nexSymbol){
    if(dataForTx.dataCnt >= dataForTx.dataSize)
    {
        return TX_COMPLITE;
    }
    if(dataForTx.maxCnt >= NUM_NAX_PER_SCREEN)
    {
        dataForTx.dataCnt++;
        dataForTx.maxCnt = 0;
        return GEN_LD;
    }

    *nexSymbol = dataForTx.data[dataForTx.dataCnt][dataForTx.maxCnt++];
    printf("|");

    return TX_DATA;
}

bool getState(void){
    if ( dataForTx.dataCnt >= dataForTx.dataSize)
        return 0;

    return 1;

}

