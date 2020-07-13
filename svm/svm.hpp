#ifndef STACK_VM_H_
#define STACK_VM_H_

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class StackVM
{
private:
	// stack memory
	uint8_t *stack = nullptr;
	istream &input;

	// registers
	unsigned long registers[7];
	unsigned long &rsp = registers[0];
	unsigned long &rbp = registers[1];

	unsigned long &rax = registers[2];
	unsigned long &rbx = registers[3];
	unsigned long &rcx = registers[4];

	unsigned long &dat = registers[5];
	unsigned long &bss = registers[6];
	unsigned long &rip = registers[7];

public:
	StackVM(istream &_input, size_t size = 0x100000) : input(_input)
	{
		stack = new uint8_t[size];
		rsp = size;
	}

	void load(const string &programFileName)
	{
		ifstream file(programFileName);
		int ctr = 0;
		char data[128];
		// while (cin.getline())
	}

	void fetch_decode()
	{

	}

	void execute() {}

	void start()
	{
		while(true)
		{
			fetch_decode();
			execute();
		}

		cout << rbx << endl;
	}
	
	~StackVM()
	{
		delete[] stack;
	}
};

#endif // STACK_VM_H_

