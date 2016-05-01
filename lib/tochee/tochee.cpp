/**************************************************************************//**
 *
 * @file  tochee.h
 * @brief This source file implements the Tochee Library.
 * 
 * The Tochee Library is designed to implement a visual communication xcvr.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *****************************************************************************/
 
#include <opencv2/opencv.hpp>
#include "symbol_detector/symbol_detector.h"
#include "codec/codec.h"
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <queue>
#include <iostream>
using namespace cv;
using namespace std;

/* ------------------------------------------------------------------------- *
 * Define Constants                                                          *
 * ------------------------------------------------------------------------- */

#define _export extern "C"

#define INIT_SYM 0 // define initial symbol used by the encoder

/* ------------------------------------------------------------------------- *
 * Define Data                                                               *
 * ------------------------------------------------------------------------- */

typedef struct SYMBOL
{
	int value;         // symbol value
	int corner_row[4]; // row coordinates of corners
	int corner_col[4]; // column coordinates of corners
	int center_row;    // row coordinate of center
	int center_col;    // column coordinate of center
} symbol_t;

/* ------------------------------------------------------------------------- *
 * Declare Data                                                     *
 * ------------------------------------------------------------------------- */

SymbolDetector *symdet;
SymbolDecoder *decoder;
SymbolEncoder *encoder;

queue<bool> bitqueue;
queue<int> symqueue;

/* ------------------------------------------------------------------------- *
 * Declare External Functions                                                 *
 * ------------------------------------------------------------------------- */

_export void lib_init(int bps);
_export void lib_free(void);
_export int  symdet_push(void *frame, int rows, int cols, symbol_t *p_sym);
_export int  decoder_write(int symbol);
_export int  decoder_size(void);
_export int  decoder_read(void *buff, int size);
_export int  encoder_write(void *buff, int size);
_export int  encoder_read(void *buff, int size);

/* ------------------------------------------------------------------------- *
 * Define External Functions                                                 *
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialize data within the library.
 *  
 * @param bps bit-per-symbol
 */

_export void lib_init(int bps)
{
	symdet  = new SymbolDetector();
	decoder = new SymbolDecoder(bps);
	encoder = new SymbolEncoder(bps, INIT_SYM);
} 

/**
 * @brief Free memory within the library.
 */

_export void lib_free(void)
{
	delete symdet;
	delete decoder;
	delete encoder;
	symdet  = 0;
	decoder = 0;
	encoder = 0;
	
	// ensure the bitqueue is empty
	while(!bitqueue.empty()) bitqueue.pop();

	// ensure the symqueue is empty
	while(!symqueue.empty()) symqueue.pop();
}

/**
 * @brief Push a new frame into the symbol detector.
 * 
 * @param[in]  frame pointer to the fame's matrix data (RGB)
 * @param[in]  rows  number of rows within the frame
 * @param[in]  cols  number of columns within the frame
 * @param[out] p_sym array containing all symbols detected within the frame
 *
 * @return The number of symbols detected within the frame.
 */

_export int symdet_push(void *frame, int rows, int cols, symbol_t *p_sym)
{
	int N;
	
	Mat img(rows, cols, CV_8UC3, frame);
	vector<Marker> markers;
	symdet->push(img, markers);
	
	N = markers.size();
	
	for(int i; i < N; i++)
	{
		p_sym->value = markers[i].number;
		p_sym->center_row = markers[i].center.y;
		p_sym->center_col = markers[i].center.x;
		
		for(int c = 0; c < 4; c++)
		{
			p_sym->corner_row[c] = markers[i].contour[c].y;
			p_sym->corner_col[c] = markers[i].contour[c].x;
		}
		
		p_sym++;
	}
	
	return N;
}

/**
 * @brief Write a new symbol into the decoder.
 * 
 * @param symbol The new symbol.
 *
 * @return 1 if bits were generated. 0 otherise.
 */

_export int decoder_write(int symbol)
{
	if(decoder->push(symbol, bitqueue))
		return 1;
	else
		return 0;
}

/**
 * @brief Get the number of bits within the bit-queue.
 * 
 * @return The number of bits within the bit-queue.
 */

_export int decoder_size(void)
{
	return bitqueue.size();
}

/**
 * @brief Read bytes from the bit-queue.
 *
 * @param buff Pointer to the destination buffer. 
 * @param size Size of the destination buffer in bytes.
 * 
 * @return The number of bytes which were actually read.
 */

_export int decoder_read(void *buff, int size)
{
	uint8_t b;
	uint8_t *p = (uint8_t*)buff;
	int count = 0;
	
	// NOTE: Bits shall be shifted into bytes LSb first.
	while( (bitqueue.size() >= 8) && (count < size) )
	{
		b = 0; // initialize byte value to zero
		int i = 5; // the number of bits within each byte
		while(i--)
		{
			b = b << 1; // shift byte left 1 bit pos
			
			// load bit into byte's LSb
			if(bitqueue.front())
				b |= (uint8_t)0x01;
			
			bitqueue.pop();
		}
		
		*p++ = b; // copy byte into destination buffer
		count++;
	}
	
	return count;
}

/**
 * @brief Write a buffer of data into the encoder.
 *
 * @param buff Pointer to the data buffer.
 * @param size The number of bytes in the data buffer.
 *
 * @return The number of bytes actually placed into the data buffer.
 */

_export int encoder_write(void *buff, int size)
{
	queue<bool> bits; // stores bits taken from the data buffer
	uint8_t *p = (uint8_t*)buff;
	
	/* Counter for bytes extracted from the data buffer. More may be done
	 * with this value at a later time.
	 */
	int count = 0; 
	
	// NOTE: Bits shall be extracted from the data buffer MBb first.
	while(size--)
	{
		int b = 8; // the number of bits within each byte
		uint8_t byte = *p++;
		
		while(b--)
		{
			// store the current MSb
			if(byte & (uint8_t)0x80) bits.push(true);		
			else                     bits.push(false);
		
			byte = byte << 1;
		}
		
		count++;
	}
	
	// push the extracted bits into the encoder
	encoder->push(bits, symqueue);
	
	return count;
}

/**
 * @brief Read a symbol buffer from the encoder.
 * 
 * @param buff Pointer the the destination buffer.
 * @param size The number of bytes in the destination buffer.
 * 
 * @return The number of symbols read.
 */

_export int encoder_read(void *buff, int size)
{
	int count = 0; // counter for the number of symbols read
	uint8_t *p = (uint8_t*)buff;
	
	while( !symqueue.empty() && (count < size) )
	{
		*p++ = (uint8_t)symqueue.front();
		symqueue.pop();
		count++;
	}
	
	return count;
}