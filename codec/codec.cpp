/**************************************************************************//**
 * Matthew Triche
 * ELEC5727
 * Final Project
 * 
 * @file  codec.cpp
 * @brief This source file implements the symbol codec.
 *
 *****************************************************************************/

#include <cassert>
#include <iostream>
#include <queue>
#include <math.h>
#include <stdint.h>
#include "codec.h"
using namespace std;

/* ------------------------------------------------------------------------- *
 * Implement CodecBase                                                       *
 * ------------------------------------------------------------------------- */

/**
 * @brief The constructor.
 * 
 * The constructor shall generate the state-machine matrix.
 *
 * @param[in] bps bits-per-symbol
 */

CodecBase::CodecBase(int bps)
{	
	m_state_dim = (int)pow(2,bps) + 1; // calculate matrix dimension
	m_bps = bps; // save bps value
	m_prev_sym = -1; // indicate no previous symbol has been given
	
	// create state-machine matrix
	m_state = new int*[m_state_dim];
	for(int i = 0; i < m_state_dim; i++)
	{
		int data = 0; // initialize data
		m_state[i] = new int[m_state_dim]; // create matrix row
		
		// initialize elements
		for(int j = 0; j < m_state_dim; j++)
		{
			if(i == j)
			{
				// symbols can't transition to themselves
				m_state[i][j] = -1;
			}
			
			else
			{
				// transition symbol(i) -> symbol(j) => binary(data)
				m_state[i][j] = data++;
			}
		}	
	}
}

/**
 * @brief The destructor.
 */

CodecBase::~CodecBase(void)
{
	// deallocate state-machine matrix
	for(int i = 0; i < m_state_dim; i++)
	{
		delete [] m_state[i];
	}
	delete [] m_state;
}
	

/* ------------------------------------------------------------------------- *
 * Implement Decoder                                                         *
 * ------------------------------------------------------------------------- */

SymbolDecoder::SymbolDecoder(int bps) : CodecBase(bps)
{
	// do nothing for now
}

/**
 * @brief Push a new symbol into the symbol decoder state-machine.
 *
 * @param[in]  input  The next symbol in the sequence.
 * @param[out] output The target bit-queue
 * 
 * @return True if a new bit was generated. False otherwise.
 */

bool SymbolDecoder::push(const int input, queue<bool> &output)
{
	// sanity check symbol value
	assert( (input >= 0) && (input < m_state_dim) );
	
	if(m_prev_sym == -1)
	{
		// the first symbol has been pushed into the state-machine
		m_prev_sym = input;
		return false;
	}
	
	else if(m_prev_sym != input)
	{
		value2bitqueue(m_state[m_prev_sym][input],m_bps,output);
		m_prev_sym = input;
		return true;
	}
	
	else
	{
		// a new symbol hasn't been given, yet
		return false;
	}
}

/**
 * @brief Reset the decoder state-machine.
 *
 * This feature is intended to help support framing. 
 */

void SymbolDecoder::reset(void)
{
	// indicate there is no previously sent symbol
	m_prev_sym = -1;
}

/**
 * @brief Convert a data value to a bit-queue.
 *
 * @param[in]  value  The data value.
 * @param[in]  bits   The number of bits in the queue.
 * @param[out] bq     The target bit-queue.
 */

void SymbolDecoder::value2bitqueue(const int value, const int bits, queue<bool> &bq)
{
	int shift_reg = value;
	for(int i = 0; i < bits; i++)
	{
		// push the data value's MSb into the bit-queue
		if((int)(1 << (bits-1)) & shift_reg)
		{
			bq.push(true);
		}
		else
		{
			bq.push(false);
		}
		
		// shift left to consider the next bit
		shift_reg = shift_reg << 1;
	}
}

/* ------------------------------------------------------------------------- *
 * Implement Encoder                                                         *
 * ------------------------------------------------------------------------- */

/**
 * @brief The constructor.
 */

SymbolEncoder::SymbolEncoder(int bps, int init) : CodecBase(bps)
{
	m_init = init; // set initial symbol
}

/**
 * @brief Push a bit-queue into the encoder state-machine.
 *
 * @param[in]  input  A new bit-queue.
 * @param[out] output The symbol-queue.
 * 
 * @return The number of symbols generated.
 */

int SymbolEncoder::push(queue<bool> &input, queue<int> &output)
{
	int gcount = 0; // stores the number of symbols generated 
	
	// add input to persistent bit-queue
	while(!input.empty())
	{
		m_queue.push(input.front());
		input.pop();
	}
	
	// consume the entire bit-queue possible
	while(m_queue.size() >= m_bps)
	{
		int s;
		int value = bitqueue2value(m_bps,m_queue);
		
		if(m_prev_sym == -1)
		{
			// output initial symbol
			output.push(m_init);
			m_prev_sym = m_init;
			gcount++;
		}
		
		// find what the next symbol should be
		for(s = 0; s < m_state_dim; s++)
		{
			int v = m_state[m_prev_sym][s];
			if(v == value)
			{
				// found a match!
				m_prev_sym = s;
				output.push(s);
				gcount++;
				break;
			}
		}
		
		// sanity check the execution sequence, no match
		assert(s != m_state_dim);
	}
	
	return gcount;
}

/**
 * @brief Convert a bit-queue to a data value.
 *
 * @param[in] bits The number of bits to pull from the queue.
 * @param[in] bq   The target bit-queue.
 * 
 * @return The resulting data value.
 */

int SymbolEncoder::bitqueue2value(const int bits, queue<bool> &bq)
{
	int value = 0;
	
	for(int i = 0; i < bits; i++)
	{
		// the queue shouldn't be empty
		assert(!bq.empty());
		
		 value = value << 1; // shift the data value left 1 bit
		
		// place the next bit in the queue into the data value's LSb
		if(bq.front())
		{
			value |= (int)0x01;
		}
		
		// remove the front bit from the bit-queue
		bq.pop();
	}
	
	return value;
}		

 