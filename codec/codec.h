/**************************************************************************//**
 *
 * @file  codec.h
 * @brief Header file for the symbol codec.
 *
 *****************************************************************************/

#include <iostream>
#include <queue>
#include <stdint.h>
using namespace std;

#ifndef _CODEC_H
#define _CODEC_H

/**
 * @brief The symbol codec base class.
 * 
 * This codec base class constructor shall generate the state-machine matrix.
 */

class CodecBase
{
public:
	CodecBase(int bps);
	~CodecBase();
	
protected:
	int m_bps;       // bits-per-symbol 
	int **m_state;   // state-machine
	int m_prev_sym;  // previous symbol given
	int m_state_dim; // dimension of the state-machine matrix
};

/**
 * @brief The symbol decoder.
 * 
 * This class can decode a symbol sequence into a bit-queue.
 */

class SymbolDecoder : public CodecBase
{
public:
	SymbolDecoder(int bps);
	~SymbolDecoder(void) { };

	bool push(const int input, queue<bool> &output);
	
private:
	
	void value2bitqueue(const int value, const int bits, queue<bool> &bq);
};

/**
 * @brief The symbol encoder.
 * 
 * This class can encode a bit-queue into a symbol-queue.
 */

class SymbolEncoder : public CodecBase
{
public:
	SymbolEncoder(int bps, int init);
	~SymbolEncoder(void) { };

	int push(queue<bool> &input, queue<int> &output);
	
private:
	
	int bitqueue2value(const int bits, queue<bool> &bq);
	
	int m_init; // initial symbol generated
	queue<bool> m_queue; // persistent bit-queue
};

#endif
