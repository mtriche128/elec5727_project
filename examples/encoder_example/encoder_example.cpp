#include <iostream>
#include "codec/codec.h"
using namespace std;

int main(void)
{
	SymbolEncoder encoder(2,0); // initialize a 1 bit-per-symbol decoder
	queue<bool> bitqueue;
	queue<int> symqueue;
	
	bitqueue.push(0);
	bitqueue.push(0);
	bitqueue.push(0);
	bitqueue.push(1);
	bitqueue.push(1);
	bitqueue.push(0);
	bitqueue.push(1);
	bitqueue.push(1);
	bitqueue.push(1);
	bitqueue.push(1);
	bitqueue.push(1);
	bitqueue.push(0);
	bitqueue.push(0);
	bitqueue.push(1);
	bitqueue.push(0);
	bitqueue.push(0);
	
	encoder.push(bitqueue,symqueue);
	
	bitqueue = queue<bool>();
	bitqueue.push(0);	

	encoder.push(bitqueue,symqueue);
	
	while(!symqueue.empty())
	{
		cout << symqueue.front() << " ";
		symqueue.pop();
	}
		
	cout << endl;

	return 0;
}

