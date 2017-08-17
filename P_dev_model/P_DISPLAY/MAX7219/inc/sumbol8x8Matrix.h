/**
 ******************************************************************************
 * @file    sumbol8x8Matrix.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    12-March-2017
 * @brief   This file contains description sumbol for 8*8 led matrix
 */
#ifndef SUMBOL8X8MATRIX_H_
#define SUMBOL8X8MATRIX_H_

#define descript8X8Sumbol(Y, X1, X2, X3, X4, X5, X6, X7, X8) \
		{												      \
				.symbol = Y,								 \
			    .points =									 \
			{												 \
			 (uint16_t)(( 1<<8 ) | ( (uint8_t)X1 )),		 \
			 (uint16_t)(( 2<<8 ) | ( (uint8_t)X2 )),		 \
			 (uint16_t)(( 3<<8 ) | ( (uint8_t)X3 )),		 \
			 (uint16_t)(( 4<<8 ) | ( (uint8_t)X4 )),		 \
			 (uint16_t)(( 5<<8 ) | ( (uint8_t)X5 )),		 \
			 (uint16_t)(( 6<<8 ) | ( (uint8_t)X6 )),		 \
			 (uint16_t)(( 7<<8 ) | ( (uint8_t)X7 )),		 \
			 (uint16_t)(( 8<<8 ) | ( (uint8_t)X8 ))		     \
			}							                 	 \
		}


typedef struct{
	uint8_t symbol;
	uint16_t points[8];
}symbol8x8MatrixTypedef;


const symbol8x8MatrixTypedef symbols8x8Matrix[] = {
		descript8X8Sumbol('1',
				0b00000000, //1
				0b00000100, //2
				0b00000100, //3
				0b00000100, //4
				0b00000100, //5
				0b00000100, //6
				0b00000100, //7
				0b00000000  //8
		),
		descript8X8Sumbol('2',
				0b00000000, //1
				0b01111100, //2
				0b00000100, //3
				0b00000100, //4
				0b00000100, //5
				0b00000100, //6
				0b00000100, //7
				0b00000000  //8
		),
		descript8X8Sumbol('3',
				0b00000000, //1
				0b00000100, //2
				0b00000100, //3
				0b00000100, //4
				0b00000100, //5
				0b00000100, //6
				0b00000100, //7
				0b00000000  //8
		),
		descript8X8Sumbol('4',
				0b00000000, //1
				0b00000100, //2
				0b00000100, //3
				0b00000100, //4
				0b00000100, //5
				0b00000100, //6
				0b00000100, //7
				0b00000000  //8
		),
		descript8X8Sumbol('5',
				0b00000000, //1
				0b00000100, //2
				0b00000100, //3
				0b00000100, //4
				0b00000100, //5
				0b00000100, //6
				0b00000100, //7
				0b00000000  //8
		),
		descript8X8Sumbol('6',
				0b00000000, //1
				0b00000100, //2
				0b00000100, //3
				0b00000100, //4
				0b00000100, //5
				0b00000100, //6
				0b00000100, //7
				0b00000000  //8
		),
		descript8X8Sumbol('7',
				0b00000000, //1
				0b00000100, //2
				0b00000100, //3
				0b00000100, //4
				0b00000100, //5
				0b00000100, //6
				0b00000100, //7
				0b00000000  //8
		),

};

#endif
