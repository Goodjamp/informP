#include "GPSprocessing.h"

#include "stdint.h"
#include "stdio.h"
#include "ctype.h"
#include "string.h"

#include "GPSprocessing.h"

static uint8_t charHexToLSOctet(uint8_t dataIn, uint8_t *numDigits);
static GPS_STATUS parsGPRMC(void *procGPS);
inline GPS_STATUS parsGPGGA(void *procGPS);
static uint8_t convertStrToNum(uint8_t *data, uint8_t size, TYPE_NUM type,  void *rez);
static uint8_t takeFIeld(uint8_t **data, uint8_t *size, uint8_t numField, processingGPS *message);


static const struct {
        uint8_t *header;
        GPS_STATUS (*listParsFun)(void *);
}listGPSMes[] = {
        [GPRMC]= {
                .header = (uint8_t*)"$GPRMC,",
                .listParsFun = parsGPRMC
              },
        [GPGGA]= {
               .header = (uint8_t*)"$GPGGA,",
               .listParsFun = parsGPGGA
             }
};

uint8_t calcCheckSum(uint8_t *data, uint8_t size){
    uint8_t k = 1;
    uint8_t rezCeckSum = 0;
    for(;k<size; k++)
        rezCeckSum ^= data[k];
    return rezCeckSum;
}

void clearGPSPars(processingGPS *procGPS){
    procGPS->cntChekSum = 0;
    procGPS->rxMesSize = 0;
    procGPS->state = Rx_HEAD;
    memset(procGPS->parsGPSBuff, 0, SIZEOF_PARS_BUFF);
    procGPS->calcCheckSum = calcCheckSum(listGPSMes[procGPS->typeMes].header, procGPS->headSize);
}

// Set new message for pars
GPS_STATUS addGPSPars(GPS_TYPE_MES mesType, void *procGPSIn){
    processingGPS *procGPS = procGPSIn;
    //if(  !ISGPSMES(mesType)  ) return GPS_STATUS_ERROR_TYPE_MES;
    procGPS->typeMes = mesType;
    procGPS->headSize = (uint8_t)strlen((char*)listGPSMes[mesType].header);
    procGPS->rxMesSize = procGPS->headSize;
    procGPS->statusGPS = GPS_STATUS_RX_PROCESS;
    clearGPSPars(procGPS);
    return GPS_STATUS_RX_PROCESS;
}

//----function charHexToLSOctet--------------------
// function charHexToLSOctet - convert char to Least Significant octet of number
static uint8_t charHexToLSOctet(uint8_t dataIn, uint8_t *numDigits) {
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



GPS_STATUS parsGPS(void *procGPS_in, uint8_t *symbols, uint8_t length){
    uint16_t cnt, k;
    processingGPS *procGPS = (processingGPS*)procGPS_in;
    procGPS->statusGPS = GPS_STATUS_RX_PROCESS;
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
                GPS_PRINTF("Rx Ok: %s\n", procGPS->parsGPSBuff);
            }
            break;
        case Rx_PAYLOAD:
            if(symbols[cnt] == '*') // last payload symbol, THEN we should receive 2 byte checksum
            {
                GPS_PRINTF("Rx_PAYLOAD Ok\n");
                procGPS->state = Rx_CHECKSUM;
                break;
            }
            procGPS->parsGPSBuff[procGPS->rxMesSize] = symbols[cnt];
            procGPS->calcCheckSum ^= symbols[cnt];
            procGPS->rxMesSize++;
            break;
        case Rx_CHECKSUM:
            procGPS->cntChekSum++;
            if(charHexToLSOctet(symbols[cnt], &procGPS->rxCheckSum))
            {
            	// in current character not HEX symbol - error
            	clearGPSPars(procGPS);
                break;
            }
            if(procGPS->cntChekSum < SIZEOF_CHECKSUM )
            {
                break;
            }
            // Receive message unsuccessful.
            if(procGPS->rxCheckSum != procGPS->calcCheckSum)
            {
            	clearGPSPars(procGPS);
                procGPS->statusGPS = GPS_STATUS_ERROR_CRC;
                GPS_PRINTF("error GPS_STATUS_ERROR_CRC \n");
                break;
            }
            // Receive message successful. Pars
            GPS_PRINTF("GPS start pars\n");
            if( listGPSMes[procGPS->typeMes].listParsFun(procGPS_in))
            {
                GPS_PRINTF("error GPS_STATUS_ERROR_PARS \n");
                procGPS->statusGPS = GPS_STATUS_ERROR_GENERAL;
            }
            else
            {
            	procGPS->statusGPS = GPS_STATUS_COMPLETE;
            }
            clearGPSPars(procGPS);
            break;
        default: break;
        }
    }
    return GPS_STATUS_ERROR_GENERAL;
}


GPS_STATUS rxGPSStatus(void *procGPS_in){
    return ((processingGPS*)procGPS_in)->statusGPS;
}

// Pars GPRMS message
inline GPS_STATUS parsGPRMC(void *procGPS){
    GPRMC_Def *pGPRMC = (GPRMC_Def*)procGPS;
    uint8_t *data;
    uint8_t dataSize;

    // Check message is valid
    takeFIeld(&data,&dataSize,RMC_F_VALIDITY,&pGPRMC->procesingPars);
    if( *data != sateliteRx )
    {
        return GPS_STATUS_ERROR_TYPE_MES;
    }
    //------------Take a Time fields--------------------------
    if( takeFIeld(&data, &dataSize, RMC_F_TIME, &pGPRMC->procesingPars)) {
        return GPS_STATUS_ERROR_TAKE_FIELD;
    };
    // check size of seconds_miliseconds
    if((dataSize - (RMC_F_TIME_MINUTES_SIZE + RMC_F_TIME_SECONDS_SIZE)) < RMC_F_TIME_SECONDS_SIZE)
    {
        return GPS_STATUS_ERROR_GENERAL;
    }
    //---HOURS----
    if( convertStrToNum(&data[RMC_F_TIME_HOURS], RMC_F_TIME_HOURS_SIZE, TYPE_NUM_INT, (void*)&pGPRMC->honour))
    {
        return GPS_STATUS_ERROR_CONV_TO_NUM;
    };
    GPS_PRINTF("HOURS = %u\n", pGPRMC->honour);
    //---MINUTES----
    if( convertStrToNum(&data[RMC_F_TIME_MINUTES], RMC_F_TIME_MINUTES_SIZE, TYPE_NUM_INT, (void*)&pGPRMC->minutes))
    {
        return GPS_STATUS_ERROR_CONV_TO_NUM;
    };
    GPS_PRINTF("MINUTES = %u\n", pGPRMC->minutes);
    //---SECONDS----
    if( convertStrToNum(&data[RMC_F_TIME_SECONDS], dataSize - (RMC_F_TIME_MINUTES_SIZE + RMC_F_TIME_SECONDS_SIZE), TYPE_NUM_FLOAT, (void*)&pGPRMC->seconds))
    {
        return GPS_STATUS_ERROR_CONV_TO_NUM;
    };
    GPS_PRINTF("SECONDS = %f\n", pGPRMC->seconds);
    //------------Take a Date fields--------------------------
    if( takeFIeld(&data, &dataSize, RMC_F_DATE, &pGPRMC->procesingPars))
    {
        return GPS_STATUS_ERROR_CONV_TO_NUM;
    };
    //---DAY----
    if( convertStrToNum(&data[RMC_F_DATE_DAY], RMC_F_DATE_DAY_SIZE, TYPE_NUM_INT, (void*)&pGPRMC->date))
    {
        return GPS_STATUS_ERROR_CONV_TO_NUM;
    };
    GPS_PRINTF("HOURS = %u\n", pGPRMC->date);
    //---MONTH----
    if( convertStrToNum(&data[RMC_F_DATE_MONTH], RMC_F_DATE_MONTH_SIZE, TYPE_NUM_INT, (void*)&pGPRMC->mounth))
    {
        return GPS_STATUS_ERROR_CONV_TO_NUM;
    };
    GPS_PRINTF("MONTH = %u\n", pGPRMC->mounth);
    //---YER----
    if( convertStrToNum(&data[RMC_F_DATE_YER], RMC_F_DATE_YER_SIZE, TYPE_NUM_INT, (void*)&pGPRMC->year))
    {
        return GPS_STATUS_ERROR_CONV_TO_NUM;
    };
    GPS_PRINTF("YER = %u\n", pGPRMC->year);


    //------------Take a LATITUDE field--------------------------
    if( takeFIeld(&data, &dataSize, RMC_F_LATITUDE, &pGPRMC->procesingPars)) {
        return GPS_STATUS_ERROR_TAKE_FIELD;
    };
    //---LATITUDE----
    if( convertStrToNum(data, dataSize, TYPE_NUM_FLOAT, (void*)&pGPRMC->latitude))
    {
        return GPS_STATUS_ERROR_CONV_TO_NUM;
    };
    GPS_PRINTF("LATITUDE = %f\n", pGPRMC->latitude);

    //------------Take a NORTH_SOUTH field--------------------------
    if( takeFIeld(&data, &dataSize, RMC_F_NORTH_SOUTH, &pGPRMC->procesingPars)) {
        return GPS_STATUS_ERROR_TAKE_FIELD;
    };
    //---NORTH_SOUTH----
    memcpy(&pGPRMC->latitude_side, data, dataSize);
    GPS_PRINTF("NORTH_SOUTH = %c\n", pGPRMC->latitude_side);


    //------------Take a LONGITITUDE field--------------------------
    if( takeFIeld(&data, &dataSize, RMC_F_LONGITITUDE, &pGPRMC->procesingPars)) {
        return GPS_STATUS_ERROR_TAKE_FIELD;
    };
    //---LONGITITUDE----
    if( convertStrToNum(data, dataSize, TYPE_NUM_FLOAT, (void*)&pGPRMC->longitude))
    {
        return GPS_STATUS_ERROR_CONV_TO_NUM;
    };
    GPS_PRINTF("LONGITITUDE = %f\n", pGPRMC->longitude);

    //------------Take a EAST_WEST field--------------------------
    if( takeFIeld(&data, &dataSize, RMC_F_EAST_WEST, &pGPRMC->procesingPars)) {
        return GPS_STATUS_ERROR_TAKE_FIELD;
    };
    //---EAST_WEST----
    memcpy(&pGPRMC->longitude_side, data, dataSize);
    GPS_PRINTF("EAST_WEST = %c\n", pGPRMC->longitude_side);


    takeFIeld(&data, &dataSize, RMC_F_NORTH_SOUTH, &pGPRMC->procesingPars);
    takeFIeld(&data, &dataSize, RMC_F_LONGITITUDE, &pGPRMC->procesingPars);
    takeFIeld(&data, &dataSize, RMC_F_EAST_WEST, &pGPRMC->procesingPars);
    takeFIeld(&data, &dataSize, RMC_F_SPEED, &pGPRMC->procesingPars);
    takeFIeld(&data, &dataSize, RMC_F_TRUE_COURSE, &pGPRMC->procesingPars);
    takeFIeld(&data, &dataSize, RMC_F_VARIATION, &pGPRMC->procesingPars);
    takeFIeld(&data, &dataSize, RMC_F_VALIDITY, &pGPRMC->procesingPars);
    takeFIeld(&data, &dataSize, RMC_F_V_EAST_WEST, &pGPRMC->procesingPars);
    return 0;
}


static uint8_t convertStrToNum(uint8_t *data, uint8_t size, TYPE_NUM type,  void *rez){
    uint8_t cnt;
    uint32_t tempRez = 0;
    uint8_t detectDevider = 0;
    float   multiple = 1;
    for(cnt = 0; cnt < size; cnt++){
        if(data[cnt] == '.')
        {

            detectDevider = 1;
            continue;
        }
        if(detectDevider)
        {
            multiple *=  0.1;
        }
        if(!isdigit(data[cnt]))
        {
            return 1;
        }
        tempRez = tempRez * 10 + data[cnt] - '0';
    }

    switch(type){
    case TYPE_NUM_FLOAT: {
        *((float*)rez) = (float)tempRez * multiple;
        break;
    }
    case TYPE_NUM_INT:
        *((uint8_t*)rez) = tempRez;
        break;
    case TYPE_NUM_DATA:
        break;
    case TYPE_NUM_ARRAY:
        break;
    default:
        return 0;
    }
    return 0;
}

static uint8_t takeFIeld(uint8_t **data, uint8_t *size, uint8_t numField, processingGPS *message){
    uint8_t cnt;
    uint8_t fieldCnt = 0, fieldSize = 0;
    *data = NULL;//message->parsGPSBuff;
    for(cnt = 0; cnt < message->rxMesSize; cnt++ ){

        if(message->parsGPSBuff[cnt] == FIELD_DEVIDER)
        {
            if(fieldCnt == numField)
            {
                *size = fieldSize;
                *data = fieldSize == 0 ? NULL : &message->parsGPSBuff[cnt - fieldSize];
                return 0;
            }
            fieldCnt++;
            fieldSize = 0;
            continue;
        }
        fieldSize++;
    }
    return 1;
}


inline GPS_STATUS parsGPGGA(void *procGPS){
	return GPS_STATUS_COMPLETE;
}



