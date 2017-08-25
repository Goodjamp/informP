/**
 ******************************************************************************
 * @file    sumbol7Segments.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    15-August-2017
 * @brief   This file contains description symbols for 7 segments indicator
 */
#ifndef SUMBOL7SEGMENTS_H_
#define SUMBOL7SEGMENTS_H_

typedef struct{
	uint8_t symbol;
	uint8_t segments;
}symbol7SegmentsTypedef;

const symbol7SegmentsTypedef symbols7Segments[] = {\
		{
		/*******        0       *******/
				.symbol = '0',
			    .segments = 0b01111110,

		},
		{
		/*******        1       *******/
				.symbol = '1',
						    //.abcdefg
			    .segments = 0b00110000,

		},
		{
		/*******        2       *******/
				.symbol = '2',
				            //.abcdefg
	            .segments = 0b01101101,

		},
		{
		/*******        3       *******/
				.symbol = '3',
				            //.abcdefg
	            .segments = 0b01111001,

		},
		{
		/*******        4       *******/
				.symbol = '4',
				            //.abcdefg
	            .segments = 0b00110011,

		},
		{
		/*******        5       *******/
				.symbol = '5',
				            //.abcdefg
	            .segments = 0b01011011,

		},
		{
		/*******        6       *******/
				.symbol = '6',
				            //.abcdefg
	            .segments = 0b01011111,

		},
		{
		/*******        7       *******/
				.symbol = '7',
				            //.abcdefg
	            .segments = 0b01110000,

		},
		{
		/*******        8       *******/
				.symbol = '8',
				            //.abcdefg
	            .segments = 0b01111111,

		},
		{
		/*******        9       *******/
				.symbol = '9',
				  //.abcdefg
	            .segments = 0b01111011,

		},
		{
		/*******        A       *******/
				.symbol = 'A',
				            //.abcdefg
	            .segments = 0b01110111,

		},
		{
		/*******        B       *******/
				.symbol = 'B',
	                        //.abcdefg
                .segments = 0b01111111,

		},
		{
		/*******        C       *******/
				.symbol = 'C',
				            //.abcdefg
	            .segments = 0b01001110,

		},
		{
		/*******        D       *******/
				.symbol = 'D',
				            //.abcdefg
	            .segments = 0b01111110,

		},
		{
		/*******        I       *******/
				.symbol = 'I',
				            //.abcdefg
	            .segments = 0b00110000,

		},
		{
		/*******        F       *******/
				.symbol = 'F',
				            //.abcdefg
	            .segments = 0b01010111,

		},
		{
		/*******        -       *******/
				.symbol = '-',
				            //.abcdefg
	            .segments = 0b00000001,

		},
		{
		/*******        .       *******/
				.symbol = '.',
				            //.abcdefg
	            .segments = 0b10000000,

		},

		};

#endif
