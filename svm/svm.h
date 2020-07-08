#ifndef STACK_VM_H_
#define STACK_VM_H_

class StackVM
{
private:
	// stack memory
	uint8_t *stack = nullptr;

	// registers
	unsigned long sp;
	unsigned long bp;

	unsigned long ax;
	unsigned long bx;
	unsigned long cx;
	unsigned long dx;


public:
	StackVM(size_t size = 0x100000)
	{
		stack = new uint8_t[size];
		sp = size;
	}
	
	~StackVM()
	{
		delete[] stack;
	}
};

#endif // STACK_VM_H_

