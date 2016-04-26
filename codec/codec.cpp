/**************************************************************************//**
 *
 * @file  codec.cpp
 * @brief This source file implements the symbol codec.
 *
 *****************************************************************************/

#include <iostream>
#include <vector>
#include <math.h>
#include <stdint.h>
#include "codec.h"
using namespace std;
 
/**
 * @brief The constructor.
 * 
 * The constructor shall generate the state-machine matrix.
 * 
 * @param[in] bps bits-per-symbol
 */


SymbolDecoder::SymbolDecoder(int bps)
{
	int N = (int)pow(2,bps) + 1;
	
	m_bps = bps; // save bps value
	m_prev_sym = 0; // initialize previous symbol holder
	
	// create state-machine matrix
	m_state = new int*[N];
	for(int i = 0; i < N; i++)
	{
		int data = 0; // initialize data
		m_state[i] = new int[N]; // create matrix row
		
		// initialize elements
		for(int j = 0; j < N; j++)
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

SymbolDecoder::~SymbolDecoder(void)
{
	// deallocate state-machine matrix
	for(int i = 0; i < m_bps; i++)
	{
		delete [] m_state[i];
	}
	delete [] m_state;
}
	
	
/**
 * @brief Push a new symbol into the symbol decoder state-machine.
 *
 * @param[in]  input  The next symbol in the sequence.
 * @param[out] output The target bit-stream
 */

void SymbolDecoder::push(const int input, vector<bool> &output)
{
	value2bitstream(m_state[m_prev_sym][input],m_bps,output);
	m_prev_sym = input;
}

/**
 * @brief Convert a data value to a bit-stream.
 *
 * @param[in]  value  The data value.
 * @param[in]  bits   The number of bits in the stream.
 * @param[out] stream The target bit-stream.
 */

void SymbolDecoder::value2bitstream(const int value, const int bits, vector<bool> &stream)
{
	int shift_reg = value;
	for(int i = 0; i < bits; i++)
	{
		// push the data value's LSB into the bit-stream
		if((int)0x0001 & shift_reg)
		{
			stream.push_back(true);
		}
		else
		{
			stream.push_back(false);
		}
		
		// shift right to consider the next bit
		shift_reg = shift_reg >> 1;
	}
}
 