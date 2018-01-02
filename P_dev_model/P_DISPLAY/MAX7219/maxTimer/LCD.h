#include "stdbool.h"

#define NUM_NAX_PER_SCREEN  2

typedef uint16_t maxDataDef[NUM_NAX_PER_SCREEN];

typedef enum{
    TX_ADDRESS_ONE,
    TX_ADDRESS_ALL
}TX_ADDRESS;

typedef enum{
    TX_DATA,
    GEN_LD,
    TX_COMPLITE
}txState;

void txData(maxDataDef *data, uint8_t dataSize, uint16_t orderNumberDispl, TX_ADDRESS txAddress);

void hwInterfaceTx(uint16_t orderNumberDispl, TX_ADDRESS txAddress);

txState getNextData(uint16_t *nexSymbol);
bool getState(void);
