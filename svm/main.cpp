#include <iostream>
#include "svm.hpp"
using namespace std;

int main()
{
	ifstream file("test.out");
	StackVM(file, 0x100000);
	return 0;
}
