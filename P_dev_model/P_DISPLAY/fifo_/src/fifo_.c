/*
 * @Author   Gerasimchuk Alexandr
 * @version  1.0
 * @Date     29.12.2016
 *
 * @Description Simple sours for FIFO operation
 *              If fifoSize is pow 2, set
 *              #define POW_OF_TWO
 *              for decrease speed write/read fifo operation
 */


#include "stdio.h"
#include "fifo_.h"



FIFO_STATUS fifoInit(hFIFO inFifo, uint16_t fifoSize, uint8_t itemtSize, uint8_t *buffAddress) {
	inFifo->fifoSize = fifoSize;
	inFifo->itemSize = itemtSize;
	inFifo->fifoArray = buffAddress;
	inFifo->counter = 0;
	inFifo->posRead = 0;
	inFifo->posWrite = 0;
#ifdef POW_OF_TWO
	uint16_t pow2 = ~((uint16_t)0);
	inFifo->fifoStatus = FIFO_STATUS_ERROR_INIT;
	for(;pow2 > 0; pow2 >>= 1)
	{
		if((pow2+1) == (fifoSize))
		{
			inFifo->fifoStatus = FIFO_STATUS_OK;
			break;
		}
	}
#else
	inFifo->fifoStatus = FIFO_STATUS_OK;
#endif
	return inFifo->fifoStatus;
}


/* @ fifoReadData read data from FIFO
 *
 *
 */
uint16_t fifoReadData(hFIFO inFifo, uint8_t* readArray, uint16_t numData) {
	uint16_t cnt, nextItem;
	uint32_t fifoByteSize = inFifo->fifoSize * inFifo->itemSize;
	if(inFifo->fifoStatus != FIFO_STATUS_OK)
	{
		return 0;
	}
	if (inFifo->counter == 0) // if fifo is empty
	{
		return 0;
	}
	for (cnt = 0; cnt < numData; cnt++) {
#ifdef POW_OF_TWO
#else
		if (inFifo->posRead >= fifoByteSize) // fifo up buffer limit
		{
			inFifo->posRead = 0;
		}
#endif
		if (inFifo->counter == 0) // fifo empty
		{
			return cnt;
		}

		nextItem = inFifo->posRead + inFifo->itemSize;
		for (; inFifo->posRead < nextItem; inFifo->posRead++, readArray++) // copy byte of one fifo item
		{
			(*readArray) = inFifo->fifoArray[inFifo->posRead];
		}
		inFifo->counter --;
	}

	return cnt;
}


/* @ fifoWriteData write data to FIFO
 *
 *
 */
uint16_t fifoWriteData(hFIFO inFifo, uint8_t const * writeArray, uint16_t numData) {
	uint16_t cnt, nextItem;
	uint32_t fifoByteSize = inFifo->fifoSize * inFifo->itemSize;
	if(inFifo->fifoStatus != FIFO_STATUS_OK) // error initilisation
	{
		return 0;
	}
	if (inFifo->counter >= fifoByteSize ) // if fifo is full
			{
		return 0;
	}
	for (cnt = 0; cnt < numData; cnt++) {
#ifdef POW_OF_TWO
#else
		if (inFifo->posWrite >= fifoByteSize ) // fifo up buffer limit
		{
			inFifo->posWrite = 0;
		}
#endif
		if (inFifo->counter >= inFifo->fifoSize) // fifo overrun (if number items in fifo equal fifo size)
		{
			return cnt;
		}

		nextItem = inFifo->posWrite + inFifo->itemSize;
		for (; inFifo->posWrite < nextItem; inFifo->posWrite++, writeArray++) // copy byte of one fifo item
		{
			inFifo->fifoArray[inFifo->posWrite ] = (*writeArray);
		}
		inFifo->counter++;
	}

	return cnt;
}


/* @ fifoGetNumData
 *
 *
 */
uint16_t fifoGetNumData(hFIFO inFifo) {
	return inFifo->counter;
}

