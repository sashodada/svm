#include <iostream>
#include "svm.hpp"
using namespace std;

int main()
{
	ifstream file("../output.code", ios::binary);
	StackVM myvm(file, 0x100000);
	int i = 0;
	while (!myvm.halted)
	{
		myvm.fetchInstruction();
		myvm.execute();

	}
	return 0;
}
