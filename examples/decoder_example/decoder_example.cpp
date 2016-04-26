#include <iostream>
#include "codec/codec.h"
using namespace std;

int main(void)
{
	SymbolDecoder decoder(1); // initialize 1 bps decoder
	vector<bool> bitstream;
	
	// initialize symbol is '0'
	decoder.push(1,bitstream);
	decoder.push(2,bitstream);
	decoder.push(1,bitstream);
	decoder.push(0,bitstream);
	
	for(int i = 0; i < bitstream.size(); i++)
	{
		if(bitstream[i])
		{
			cout << "1 ";
		}
		
		else
		{
			cout << "0 ";
		}
	}
		
	cout << endl;

	return 0;
}

