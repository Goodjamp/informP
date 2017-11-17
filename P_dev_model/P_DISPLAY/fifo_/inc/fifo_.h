/*
 * @Author   Gerasimchuk Alexandr
 * @version  1.0
 * @Date     29.12.2016
 *
 * @Description Simple sours for FIFO operation
 */

#ifndef FIFO_H_
#define FIFO_H_

#include "stdint.h"

#define hFIFO fifoHandler*

typedef enum{
	FIFO_STATUS_OK,
	FIFO_STATUS_WRITE,
	FIFO_STATUS_READ,
	FIFO_STATUS_OVERUN,
	FIFO_STATUS_ERROR_INIT
}FIFO_STATUS;


typedef struct{
	uint16_t    posRead;
	uint16_t    posWrite;
	uint16_t    fifoSize;
	uint16_t    counter;
	uint8_t     itemSize;
	uint8_t     *fifoArray;
	FIFO_STATUS fifoStatus;
}fifoHandler;


FIFO_STATUS fifoInit(hFIFO inFifo, uint16_t fifoSize, uint8_t itemtSize, uint8_t *buffAddress);
uint16_t fifoReadData(hFIFO inFifo, uint8_t* readArray, uint16_t numData);
uint16_t fifoWriteData(hFIFO inFifo, uint8_t const *  writeArra, uint16_t numData);
uint16_t fifoGetNumData(hFIFO inFifo);

#endif
