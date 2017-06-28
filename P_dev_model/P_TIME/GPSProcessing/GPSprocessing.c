#include "GPSprocessing.h"

#include "stdint.h"
#include "stdio.h"
#include "ctype.h"
#include "string.h"

// Project my headers
//#include "processing_USART.h"
//#include "USART_fifo_operation.h"
#include "GPSprocessing.h"

static uint8_t charHexToLSOct(uint8_t dataIn, uint8_t *numDigits);
static void parsGPRMC(processingGPS *procGPS);
static void parsGPGGA(processingGPS *procGPS);


static const struct {
        uint8_t *header;
        void (*listParsFun)(processingGPS *);
}listGPSMes[] = {
        [GPRMC]= {
                .header = (uint8_t*)"$GPRMC,",
                .listParsFun = parsGPRMC
              },
        [GPGGA]= {
               .header = (uint8_t*)"$GPGGA,",
               .listParsFun = parsGPRMC
             }
};





/*
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
*/

uint8_t calcCheckSum(uint8_t *data, uint8_t size){
    uint8_t k, rezCeckSum = 0;
    for(;k<size; k++)
        rezCeckSum ^= data[k];
    return rezCeckSum;
}

void clearGPSPars(processingGPS *procGPS){
    procGPS->cntChekSum = 0;
    procGPS->rxMesSize = 0;
    procGPS->state = Rx_HEAD;
    memset(procGPS->parsGPSBuff, 0, SIZEOF_PARS_BUFF);
    procGPS->calcCheckSum ^= calcCheckSum(listGPSMes[procGPS->typeMes].header, procGPS->headSize);
}

// Set new message for pars
GPS_STATUS addGPSPars(GPS_TYPE_MES mesType, void *procGPSIn){
    processingGPS *procGPS = procGPSIn;
    if(  !ISGPSMES(mesType)  ) return GPS_ERROR_TYPE_MES;
    if( mesType == procGPS->typeMes ) return GPS_OK;
    procGPS->typeMes = mesType;
    procGPS->headSize = (uint8_t)strlen((char*)listGPSMes[mesType].header);
    procGPS->rxMesSize = procGPS->headSize;
    clearGPSPars(procGPS);
    return GPS_OK;
}





void parsGPS(void *procGPS_in, uint8_t *symbols, uint8_t length){
    uint16_t cnt, k;
    processingGPS *procGPS = (processingGPS*)procGPS_in;
    for(cnt = 0; cnt < length; cnt++){
        switch(procGPS->state){
        case Rx_HEAD:
            for(k = 1; k < procGPS->headSize; k++) //shift left on one byte
            {
                procGPS->parsGPSBuff[k-1] = procGPS->parsGPSBuff[k];
            }
            procGPS->parsGPSBuff[procGPS->headSize-1] = symbols[cnt];
            if( strcmp((char*)listGPSMes[procGPS->typeMes].header, (char*)procGPS->parsGPSBuff) == 0 )
            {
                procGPS->state = Rx_PAYLOAD;
                printf("Rx Ok: %s\n", procGPS->parsGPSBuff);
            }
            break;
        case Rx_PAYLOAD:
            if(symbols[cnt] == '*') // last payload symbol, THEN we should receive 2 byte checksum
            {
                printf("Rx_PAYLOAD Ok\n");
                procGPS->state = Rx_CHECKSUM;
                break;
            }
            procGPS->parsGPSBuff[procGPS->rxMesSize] = symbols[cnt];
            procGPS->calcCheckSum ^= symbols[cnt];
            procGPS->rxMesSize++;
            break;
        case Rx_CHECKSUM:
            procGPS->cntChekSum++;
            if(charHexToLSOct(symbols[cnt], &procGPS->rxCheckSum))
            {
                procGPS->statusGPS = GPS_ERROR_RX_CHEKSUM;
                procGPS->state = Rx_HEAD;
                break;
            }
            if(procGPS->cntChekSum < SIZEOF_CHECKSUM )
            {
                break;
            }
            // Receive message unsuccessful.
            if(procGPS->rxCheckSum != procGPS->calcCheckSum)
            {
                procGPS->state = Rx_HEAD;
                procGPS->statusGPS = GPS_ERROR_CHEKSUM;
                break;
            }
            // Receive message unsuccessful.
            listGPSMes[procGPS->typeMes].listParsFun(procGPS);
            break;
        default: break;
        }
    }
}

uint8_t charHexToLSOct(uint8_t dataIn, uint8_t *numDigits) {

    if (isalpha(dataIn)) {
        dataIn = tolower(dataIn);
        if ((dataIn >= 'a') || (dataIn <= 'f'))
        {
            (*numDigits) <<= 4;
            (*numDigits) |= dataIn - 'a' + 10;
         }
        else
        {
            return 1;
        }
    }
    else if (isdigit(dataIn))
    {
        (*numDigits) <<= 4;
        (*numDigits) |= dataIn - '0';
    }
    else
    {
        return 1;  // All other symbols - error
    }
    return 0;
}

// Pars GPRMS message
inline void parsGPRMC(processingGPS *procGPS){
    GPRMC_Def *pGPRMC = (GPRMC_Def*)procGPS;
    uint8_t cnt;
    uint8_t fieldCnt = 0; //
    // Check message is valid
    for(cnt = 0; cnt <  pGPRMC->procesingPars.rxMesSize; cnt++)
    {
        if(pGPRMC->procesingPars.parsGPSBuff[cnt] == DEVIDE_SYMBOL)
        {
            fieldCnt++;
        }
        if( fieldCnt != RMC_F_VALIDITY) continue;
        cnt++;
        // Find validation field
        if( (pGPRMC->procesingPars.parsGPSBuff[cnt]  != sateliteRx) ||
            (pGPRMC->procesingPars.parsGPSBuff[cnt]  != sateliteNotRx) )
        {
            pGPRMC->procesingPars.statusGPS = GPS_ERROR_SYMBOL;
            return;
        }
        if(pGPRMC->procesingPars.parsGPSBuff[cnt] == sateliteNotRx )
        {
            pGPRMC->procesingPars.statusGPS = GPS_NOT_VALID;
            return;
        }
        else
        {
            break;
        }
    }
    if( fieldCnt != RMC_F_VALIDITY)
    {
        pGPRMC->procesingPars.statusGPS = GPS_NOT_VALID;
    }
    for(cnt = 0; cnt <  pGPRMC->procesingPars.rxMesSize; cnt++){

        switch (cnt){
            case 0:
                if(!isdigit(pGPRMC->procesingPars.parsGPSBuff[cnt]))
                {
                    pGPRMC->procesingPars.statusGPS = GPS_ERROR_SYMBOL;
                    return;
                }
                pGPRMC->honour =
                fieldCnt++;
                break;
            case 1: break;
            case 2: break;
            case 3: break;
        }
    }
}

void parsGPGGA(processingGPS *procGPS){

}
