#include <iostream>
#include "codec/codec.h"
using namespace std;

int main(void)
{
	SymbolDecoder decoder(2); // initialize a 1 bit-per-symbol decoder
	queue<bool> bitqueue;
	
	decoder.push(0,bitqueue);
	decoder.push(1,bitqueue);
	decoder.push(2,bitqueue);
	decoder.push(3,bitqueue);
	decoder.push(4,bitqueue);
	decoder.push(3,bitqueue);
	decoder.push(2,bitqueue);
	decoder.push(1,bitqueue);
	decoder.push(0,bitqueue);
	
	while(!bitqueue.empty())
	{
		if(bitqueue.front())
		{
			cout << "1 ";
		}
		
		else
		{
			cout << "0 ";
		}
		
		bitqueue.pop();
	}
		
	cout << endl;

	return 0;
}

