#ifndef STACK_VM_H_
#define STACK_VM_H_

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "../common/enums.h"
#include "utils.h"
using namespace std;

int codeSize = 71;


class StackVM
{
private:
	// stack memory
	unsigned char *stack = nullptr;
	istream &input;

	// registers
	unsigned long registers[7];
	unsigned long &rsp = registers[0];
	unsigned long &rbp = registers[1];

	unsigned long &rax = registers[2];
	unsigned long &rbx = registers[3];

	unsigned long &dat = registers[4];
	unsigned long &bss = registers[5];
	unsigned long &rip = registers[6];

	OP_CODE nextCom;

	void dumpRegisters()
	{
		for (int i = 0; i < 7; ++i)
		{
			auto regptr = getRegisterPtr(static_cast<REGISTER>(i));
			cout << "register " << registerNames[static_cast<REGISTER>(i)] << " " << *(int *)regptr << endl;
		}
	}

	unsigned char *getRegisterPtr(REGISTER reg)
	{
		switch (reg)
		{
		case REG_RSP:
			return (unsigned char *)&rsp;
		case REG_RBP:
			return (unsigned char *)&rbp;
		case REG_RAX:
			return (unsigned char *)&rax;
		case REG_RBX:
			return (unsigned char *)&rbx;
		case REG_RIP:
			return (unsigned char *)&rip;
		case REG_DAT:
			return (unsigned char *)&dat;
		case REG_BSS:
			return (unsigned char *)&bss;
		default:
			throw runtime_error("register fetch error");
		};
	}

	unsigned char *getArgumentPlace(unsigned char meta)
	{
		unsigned char *ptr;
		switch (getPlacement(meta))
		{
		case PLC_IMMEDIATE:
			cout << "(immediate)"
				 << " ";
			return stack + rip + 1;
		case PLC_REGISTER:
			cout << "[%" + registerNames[getRegister(meta)] << "] ";
			return getRegisterPtr(getRegister(meta));
		case PLC_REGISTER_OFFSET:
			cout << "[%" + registerNames[getRegister(meta)] + " + n] ";
			ptr = getRegisterPtr(getRegister(meta));
			return stack + *(int *)ptr + *(int *)(stack + rip + 1);
		default:
			throw runtime_error("argument placement error");
		}
	}

	int calculateOffset(unsigned char meta)
	{
		if (getPlacement(meta) == PLC_REGISTER)
			return 0;
		else if (getPlacement(meta) == PLC_REGISTER_OFFSET)
			return 4;
		return getValueType(meta) * 4;
	}

	void loadCode()
	{
		input.read((char *)stack, codeSize);
		*(int*)&bss = codeSize;
		*(int*)&dat = codeSize;
		*(int*)&rip = 0;
		*(int*)&rax = 0;

	}

	void jump()
	{
		if (getValueType(stack[rip]) != INT)
		{
			throw runtime_error("malformed jump instruction");
		}
		auto ptr = getArgumentPlace(stack[rip]);
		auto type = getValueType(stack[rip]);
		int jumpAddr = *(int *)ptr;
		rip = jumpAddr;
	}

	void _push(double val)
	{
		*(int *)&rsp -= 8;
		for (int i = 0; i < 8; ++i)
		{
			stack[*(int *)&rsp + i] = *(((unsigned char *)&val) + i);
		}
	}

	void _push(int val)
	{
		*(int *)&rsp -= 4;
		for (int i = 0; i < 4; ++i)
		{
			stack[*(int *)&rsp + i] = *(((unsigned char *)&val) + i);
		}
	}

	void push()
	{
		auto ptr = getArgumentPlace(stack[rip]);
		if (getValueType(stack[rip]) == INT)
			_push(*(int *)ptr);
		else
			_push(*(double *)ptr);
		rip += 1 + calculateOffset(stack[rip]);
	}

	void _pop(double *ptr)
	{
		*ptr = *(double *)(stack + *(int *)&rsp);
		*(int *)&rsp += 8;
	}

	void _pop(int *ptr)
	{
		*ptr = *(int *)(stack + *(int *)&rsp);
		*(int *)&rsp += 4;
	}

	void pop()
	{
		auto ptr = getArgumentPlace(stack[rip]);
		if (getValueType(stack[rip]) == INT)
			_pop((int *)ptr);
		else
			_pop((double *)ptr);
		rip += 1 + calculateOffset(stack[rip]);
	}

	void add()
	{
		auto ptrl = getArgumentPlace(stack[rip]);
		auto ltype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		auto ptrr = getArgumentPlace(stack[rip]);
		auto rtype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (ltype == DOUBLE && rtype == DOUBLE)
			(*(double *)ptrl) += *(double *)ptrr;
		if (ltype == DOUBLE && rtype == INT)
		{
			int val = *(double *)ptrl;
			(*(double *)ptrl) = 0;
			(*(int *)ptrl) = val + *(int *)ptrr;
		}
		if (ltype == INT && rtype == DOUBLE)
			(*(int *)ptrl) += *(double *)ptrr;
		if (ltype == INT && rtype == INT)
			(*(int *)ptrl) += *(int *)ptrr;
	}

	void move()
	{
		auto ptrl = getArgumentPlace(stack[rip]);
		auto ltype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		auto ptrr = getArgumentPlace(stack[rip]);
		auto rtype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (ltype == DOUBLE && rtype == DOUBLE)
			(*(double *)ptrl) = *(double *)ptrr;
		else if (ltype == DOUBLE && rtype == INT)
		{
			(*(double *)ptrl) = 0;
			(*(int *)ptrl) = *(int *)ptrr;
		}
		else if (ltype == INT && rtype == DOUBLE)
			(*(int *)ptrl) = *(double *)ptrr;
		else if (ltype == INT && rtype == INT)
			(*(int *)ptrl) = *(int *)ptrr;
	}

	void halt() { halted = true; }

	void op_not()
	{
	}

	void neg()
	{
	}

public:
	StackVM(istream &_input, size_t size = 0x100000) : input(_input)
	{
		stack = new unsigned char[size];
		cout << size << endl;
		*(int *)&rbp = *(int *)&rsp = size;
		loadCode();
	}

	bool halted = false;
	void fetchInstruction()
	{
		nextCom = (OP_CODE)stack[rip++];
		cout << nextCom << endl;
		cout << opCodeNames[nextCom] << " ";
	}

	void execute()
	{
		switch (nextCom)
		{
		case OP_PUSH:
			push();
			break;
		case OP_POP:
			pop();
			break;
		case OP_MOV:
			move();
			break;
		case OP_JMP:
			jump();
			break;
		case OP_JZ:
			break;
		case OP_JNZ:
			break;
		case OP_NOT:
			op_not();
			break;
		case OP_NEG:
			neg();
			break;
		case OP_INC:
			break;
		case OP_DEC:
			break;
		case OP_ADD:
			add();
			break;
		case OP_SUB:
			break;
		case OP_MLT:
			break;
		case OP_DIV:
			break;
		case OP_REM:
			break;
		case OP_EQ:
			break;
		case OP_NEQ:
			break;
		case OP_GEQ:
			break;
		case OP_GT:
			break;
		case OP_LEQ:
			break;
		case OP_LT:
			break;
		case OP_AND:
			break;
		case OP_OR:
			break;
		case OP_HALT:
			halt();
			break;
		}

		dumpRegisters();
		cout << "\n\n";
	}

	~StackVM()
	{
		delete[] stack;
	}
};

#endif // STACK_VM_H_
