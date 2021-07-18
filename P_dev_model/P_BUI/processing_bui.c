#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "funct.h"
#include "processing_mem_map.h"
#include "processing_mem_map_extern.h"

#include "processing_bui_extern.h"
#include "processing_USART.h"

#define USART_BUI                 USART1

#define BUI_START_SYMBOL            0
#define BUI_BUFFER_SIZE             12
#define BUI_COMMAND_TYPE            0x41
#define BUI_COMMAND_TX_STATUS       1

#define BUI_SEGMENTS_SUMBOLS_CNT    4

#define member_size(type, member) sizeof(((type *)0)->member)

typedef enum {
    BUI_FOUND_START,
    BUI_READ_HEAD,
    BUI_READ_COMMAND,
    BUI_READ_STRING,
} BuiState;

#pragma pack(push,1)
typedef struct {
    struct {
        uint8_t address;
        uint8_t packetType;
    } head;
    union {
        struct {
            uint8_t command;
            uint8_t matrixSymbol;
            uint8_t segmentsSymbols[BUI_SEGMENTS_SUMBOLS_CNT];
            uint8_t crc;
        } stringPack;
        struct {
            uint8_t command;
            uint8_t crc;
        } commandPack;
    };
} BuiRequestPacket;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint8_t startSumbol;
    uint8_t address;
    uint8_t totalState: 1;
    uint8_t wasReset: 1;
    uint8_t noFrq: 1;
    uint8_t noRam: 1;
    uint8_t reserved: 4;
    uint8_t crc;
} BuiReplayState;
#pragma pack(pop)

struct {
    BuiState state;
    uint32_t currentReadCnt;
    uint32_t targetReadCnt;
    uint32_t globPos;
} readState = {
     .state = BUI_FOUND_START,
     .currentReadCnt = 0,
     .targetReadCnt = 0,
     .globPos = 0,
};

uint8_t buiRxBuff[BUI_BUFFER_SIZE];
uint8_t buiTxBuff[BUI_BUFFER_SIZE];
extern S_address_oper_data s_address_oper_data;

uint16_t bui_calc_address_oper_reg(S_bui_address *ps_bui_address, uint16_t adres_start) {
    ps_bui_address->buiStatus     = adres_start;
    ps_bui_address->buiMatrixSymbol   = ps_bui_address->buiStatus + structFieldRegSize(S_bui_oper_data,buiStatus);
    ps_bui_address->buiSegmentSymbol0 = ps_bui_address->buiMatrixSymbol + structFieldRegSize(S_bui_oper_data,buiMatrixSymbol);
    ps_bui_address->buiSegmentSymbol1 = ps_bui_address->buiSegmentSymbol0 + structFieldRegSize(S_bui_oper_data,buiSegmentSymbol0);
    ps_bui_address->buiSegmentSymbol2 = ps_bui_address->buiSegmentSymbol1 + structFieldRegSize(S_bui_oper_data,buiSegmentSymbol1);
    ps_bui_address->buiSegmentSymbol3 = ps_bui_address->buiSegmentSymbol2 + structFieldRegSize(S_bui_oper_data,buiSegmentSymbol2);
    adres_start                       = ps_bui_address->buiSegmentSymbol3 + structFieldRegSize(S_bui_oper_data,buiSegmentSymbol3);
    return adres_start;
}

uint8_t buiCalcCrc(uint8_t buff[], uint32_t size){
    uint8_t crc = 0;

    for(uint32_t k = 0; k < size; k++) {
        crc += *buff++;
    }
    crc = ~crc;
    crc++;
    return crc;
}


static void resetReading(void) {
    readState.state = BUI_FOUND_START;
    readState.globPos = 0;
    readState.currentReadCnt = 0;
    readState.targetReadCnt = member_size(BuiRequestPacket, head);
}

static bool readWithTest(struct point_wrdirobject *usartH) {
    uint32_t readCnt = ReadUSART(usartH, &buiRxBuff[readState.globPos], 1, 20);
    if (readCnt == 0) {
        return false;
    }
    if (buiRxBuff[readState.currentReadCnt] == BUI_START_SYMBOL) {
        resetReading();
        return false;
    }
    if (readState.currentReadCnt > readState.targetReadCnt) {
        resetReading();
        return false;
    }
    readState.currentReadCnt++;
    readState.globPos++;
    if (readState.currentReadCnt < readState.targetReadCnt) {
        return false;
    }
    return true;
}

void t_processing_bui(void *pvParameters)
{
    S_Task_parameters *usartH = NULL;
    S_bui_user_config *config = (S_bui_user_config *)pvParameters;
    S_port_config ps_port_unit_config = {
       .baudrate = config->baudrate,
       .parity = config->parity,
       .stopbits = 2,
       .amountBit = 8,
       .controlpotok = 0
    };
    uint16_t buiStr[5] = {'?', 'E', 'E', 'E', 'E'};
    BuiRequestPacket *buiRequest = (BuiRequestPacket *)buiRxBuff;
    BuiReplayState repalyState = {
        .startSumbol = BUI_START_SYMBOL,
        .address = config->address,
        .totalState = 1,
        .wasReset = 1,
        .noFrq = 0,
        .noRam = 0,
        .reserved = 0,
    };

    processing_mem_map_write_s_proces_object_modbus(buiStr, 5,
                                                    s_address_oper_data.s_bui_address.buiMatrixSymbol);
    ConfigureUSART(&ps_port_unit_config, USART_BUI, &usartH);

    while(1)
    {
        vTaskDelay(2);
        switch (readState.state) {
        case BUI_FOUND_START:
            if (ReadUSART(usartH->RdUSART, buiRxBuff, 1, 20) != 1) {
                break;
            }
            if (buiRxBuff[0] != BUI_START_SYMBOL) {
                break;
            }
            readState.state = BUI_READ_HEAD;
            readState.globPos = 0;
            readState.currentReadCnt = 0;
            readState.targetReadCnt = member_size(BuiRequestPacket, head);
            break;
        case BUI_READ_HEAD:
            if (readWithTest(usartH->RdUSART) == false) {
                break;
            }
            if (buiRequest->head.address != config->address) {
                resetReading();
                break;
            }
            if (buiRequest->head.packetType  > 0
                && buiRequest->head.packetType  < BUI_COMMAND_TYPE) {
                readState.state = BUI_READ_STRING;
                readState.currentReadCnt = 0;
                readState.targetReadCnt = member_size(BuiRequestPacket, stringPack);
            } else if (buiRequest->head.packetType == BUI_COMMAND_TYPE) {
                readState.state = BUI_READ_COMMAND;
                readState.currentReadCnt = 0;
                readState.targetReadCnt = member_size(BuiRequestPacket, commandPack);
            } else {
                resetReading();
            }
            break;

        case BUI_READ_STRING: {
            if (readWithTest(usartH->RdUSART) == false) {
                break;
            }
            if (buiCalcCrc(buiRxBuff,
                           member_size(BuiRequestPacket, head)
                           + member_size(BuiRequestPacket, stringPack) - 1)
                != buiRequest->stringPack.crc) {
                resetReading();
                break;
            }
            /*Update screen*/
            switch (buiRequest->stringPack.matrixSymbol) {
            case 'T':
            case 'H':
            case 'D':
            case 'P':
            case 'F':
            case 'C':
            case 'S':
            case 'Q':
            case 'O':
            case 'Z':
            	buiStr[0] = buiRequest->stringPack.matrixSymbol;
            	break;

            default:
            	buiStr[0] = '?';
            	break;
            }

            for (uint32_t k = 0; k < BUI_SEGMENTS_SUMBOLS_CNT; k++) {
                if (buiRequest->stringPack.segmentsSymbols[k] >= '0'
                    && buiRequest->stringPack.segmentsSymbols[k] <= '9') {
                    buiStr[k + 1] =  buiRequest->stringPack.segmentsSymbols[k];
                } else if  (buiRequest->stringPack.segmentsSymbols[k] >= 0xA0
                            && buiRequest->stringPack.segmentsSymbols[k] <= 0xA9) {
                    buiStr[k + 1] =  ('.' << 8) | (buiRequest->stringPack.segmentsSymbols[k] - (0xA0 - 0x30));
                } else {
                    buiStr[k + 1] = 'E';
                }
            }

            processing_mem_map_write_s_proces_object_modbus(buiStr, 5,
                                                            s_address_oper_data.s_bui_address.buiMatrixSymbol);
            resetReading();
            break;
        }

        case BUI_READ_COMMAND:
            if (readWithTest(usartH->RdUSART) == false) {
                break;
            }
            if (buiCalcCrc(buiRxBuff,
                           member_size(BuiRequestPacket, head)
                           + member_size(BuiRequestPacket, commandPack) - 1)
                != buiRequest->commandPack.crc) {
                resetReading();
                break;
            }
            if (buiRequest->commandPack.command == BUI_COMMAND_TX_STATUS) {
                repalyState.crc = buiCalcCrc((uint8_t *)&repalyState, sizeof(repalyState) - 1);
                memcpy(buiTxBuff, &repalyState, sizeof(repalyState));
                write_Usart_Buf(usartH->USARTx, buiTxBuff,
                                sizeof(repalyState), usartH->WrUSART);
                repalyState.wasReset = 0;
            }
            resetReading();
            break;
        }
    }
}
