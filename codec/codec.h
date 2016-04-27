/**************************************************************************//**
 *
 * @file  codec.h
 * @brief Header file for the symbol codec.
 *
 *****************************************************************************/
 
#include <vector>
#include <stdint.h>
using namespace std;

#ifndef _CODEC_H
#define _CODEC_H

/**
 * @brief The symbol decoder.
 * 
 * This class can decode a symbol sequence into a bit-stream.
 */

class SymbolDecoder
{
public:
	SymbolDecoder(int bps);
	~SymbolDecoder(void);

	bool push(const int input, vector<bool> &output);
	
private:
	
	void value2bitstream(const int value, const int bits, vector<bool> &stream);
 	
	int m_bps;   // bits-per-symbol 
	int **m_state; // state-machine
	int m_prev_sym;
};

/**
 * @brief The symbol encoder.
 * 
 * This class can encode a symbol sequence into a bit-stream.
 */

class SymbolEncoder
{
public:
	SymbolEncoder(int bps);
	~SymbolEncoder(void);

	int push(vector<bool> &input);
	
private:
	
	int bitstream2value(const int bits, vector<bool> &stream);
 	
	int m_bps;   // bits-per-symbol 
	int **m_state; // state-machine
	int m_prev_sym;
};

#endif
