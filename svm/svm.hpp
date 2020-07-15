#ifndef STACK_VM_H_
#define STACK_VM_H_

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "../common/enums.h"
#include "utils.h"
using namespace std;

int codeSize = 246;

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

public:
	void dumpRegisters()
	{
		for (int i = 0; i < 7; ++i)
		{
			auto regptr = getRegisterPtr(static_cast<REGISTER>(i));
			cout << "register " << registerNames[static_cast<REGISTER>(i)] << " " << *(int *)regptr << endl;
		}
	}

private:
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
		input.read((char *)&codeSize, sizeof(int));
		input.read((char *)stack, codeSize);
		*(int *)&bss = codeSize;
		*(int *)&dat = codeSize;
		rip = 0;
		rax = 0;
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

	void jz()
	{
		auto ptr = getArgumentPlace(stack[rip]);
		auto type = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		bool isZero = false;
		if (type == INT)
			isZero = *(int *)ptr == 0;
		else if (type == DOUBLE)
			isZero = *(double *)ptr == 0;

		if (isZero)
			jump();
		else
			rip += 1 + calculateOffset(stack[rip]);
	}

	void jnz()
	{
		auto ptr = getArgumentPlace(stack[rip]);
		auto type = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		bool isZero = false;
		if (type == INT)
			isZero = *(int *)ptr == 0;
		else if (type == DOUBLE)
			isZero = *(double *)ptr == 0;

		if (!isZero)
			jump();
		else
			rip += 1 + calculateOffset(stack[rip]);
	}

	void _push(double val)
	{
		*(int *)&rsp -= 8;
		*(double *)(stack + *(int *)&rsp) = val;
	}

	void _push(int val)
	{
		*(int *)&rsp -= 4;
		*(int *)(stack + *(int *)&rsp) = val;
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

	void sub()
	{
		auto ptrl = getArgumentPlace(stack[rip]);
		auto ltype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		auto ptrr = getArgumentPlace(stack[rip]);
		auto rtype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (ltype == DOUBLE && rtype == DOUBLE)
			(*(double *)ptrl) -= *(double *)ptrr;
		if (ltype == DOUBLE && rtype == INT)
		{
			int val = *(double *)ptrl;
			(*(double *)ptrl) = 0;
			(*(int *)ptrl) = val - *(int *)ptrr;
		}
		if (ltype == INT && rtype == DOUBLE)
			(*(int *)ptrl) -= *(double *)ptrr;
		if (ltype == INT && rtype == INT)
			(*(int *)ptrl) -= *(int *)ptrr;
	}

	void multiply()
	{
		auto ptrl = getArgumentPlace(stack[rip]);
		auto ltype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		auto ptrr = getArgumentPlace(stack[rip]);
		auto rtype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (ltype == DOUBLE && rtype == DOUBLE)
			(*(double *)ptrl) *= *(double *)ptrr;
		if (ltype == DOUBLE && rtype == INT)
		{
			int val = *(double *)ptrl;
			(*(double *)ptrl) = 0;
			(*(int *)ptrl) = val * *(int *)ptrr;
		}
		if (ltype == INT && rtype == DOUBLE)
			(*(int *)ptrl) *= *(double *)ptrr;
		if (ltype == INT && rtype == INT)
			(*(int *)ptrl) *= *(int *)ptrr;
	}

	void divide()
	{
		auto ptrl = getArgumentPlace(stack[rip]);
		auto ltype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		auto ptrr = getArgumentPlace(stack[rip]);
		auto rtype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (ltype == DOUBLE && rtype == DOUBLE)
			(*(double *)ptrl) /= *(double *)ptrr;
		if (ltype == DOUBLE && rtype == INT)
		{
			int val = *(double *)ptrl;
			(*(double *)ptrl) = 0;
			(*(int *)ptrl) = val / *(int *)ptrr;
		}
		if (ltype == INT && rtype == DOUBLE)
			(*(int *)ptrl) /= *(double *)ptrr;
		if (ltype == INT && rtype == INT)
			(*(int *)ptrl) /= *(int *)ptrr;
	}

	void remainder()
	{
		auto ptrl = getArgumentPlace(stack[rip]);
		auto ltype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		auto ptrr = getArgumentPlace(stack[rip]);
		auto rtype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (ltype == DOUBLE && rtype == DOUBLE)
		{
			int valr = (int)*(double *)ptrl % (int)*(double *)ptrr;
			*(double *)ptrl = (double)valr;
		}
		if (ltype == DOUBLE && rtype == INT)
		{
			int val = *(double *)ptrl;
			(*(double *)ptrl) = 0;
			(*(int *)ptrl) = val % *(int *)ptrr;
		}
		if (ltype == INT && rtype == DOUBLE)
			(*(int *)ptrl) %= (int)*(double *)ptrr;
		if (ltype == INT && rtype == INT)
			(*(int *)ptrl) %= *(int *)ptrr;
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
		auto ptr = getArgumentPlace(stack[rip]);
		auto type = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (type == DOUBLE)
		{
			auto val = *(double *)ptr;
			*(double *)ptr = 0.0;
			*(int *)ptr = (int)!val;
		}
		else if (type == INT)
			*(int *)ptr = !*(int *)ptr;
	}

	void negate()
	{
		auto ptr = getArgumentPlace(stack[rip]);
		auto type = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (type == DOUBLE)
			*(double *)ptr *= -1.0;
		else if (type == INT)
			*(int *)ptr *= -1;
	}

	void op_and()
	{
		auto ptrl = getArgumentPlace(stack[rip]);
		auto ltype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		auto ptrr = getArgumentPlace(stack[rip]);
		auto rtype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (ltype == DOUBLE && rtype == DOUBLE)
		{
			int valr = *(double *)ptrl && *(double *)ptrr;
			*(double *)ptrl = 0;
			*(int *)ptrl = valr;
		}
		if (ltype == DOUBLE && rtype == INT)
		{
			int val = *(double *)ptrl;
			(*(double *)ptrl) = 0;
			*(int *)ptrl = val && *(int *)ptrr;
		}
		if (ltype == INT && rtype == DOUBLE)
			*(int *)ptrl = *(int *)ptrl && *(double *)ptrr;
		if (ltype == INT && rtype == INT)
			*(int *)ptrl = (*(int *)ptrl) && *(int *)ptrr;
	}

	void op_or()
	{
		auto ptrl = getArgumentPlace(stack[rip]);
		auto ltype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		auto ptrr = getArgumentPlace(stack[rip]);
		auto rtype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (ltype == DOUBLE && rtype == DOUBLE)
		{
			int valr = *(double *)ptrl && *(double *)ptrr;
			*(double *)ptrl = 0;
			*(int *)ptrl = valr;
		}
		if (ltype == DOUBLE && rtype == INT)
		{
			int val = *(double *)ptrl;
			(*(double *)ptrl) = 0;
			*(int *)ptrl = val || *(int *)ptrr;
		}
		if (ltype == INT && rtype == DOUBLE)
			*(int *)ptrl = *(int *)ptrl || *(double *)ptrr;
		if (ltype == INT && rtype == INT)
			*(int *)ptrl = (*(int *)ptrl) || *(int *)ptrr;
	}

	void increment()
	{
		auto ptr = getArgumentPlace(stack[rip]);
		auto type = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (type == DOUBLE)
			*(double *)ptr += 1;
		else if (type == INT)
			*(int *)ptr += 1;
	}

	void decrement()
	{
		auto ptr = getArgumentPlace(stack[rip]);
		auto type = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		if (type == DOUBLE)
			*(double *)ptr -= 1;
		else if (type == INT)
			*(int *)ptr -= 1;
	}

	void cmp(OP_CODE op)
	{
		auto ptrl = getArgumentPlace(stack[rip]);
		auto ltype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		auto ptrr = getArgumentPlace(stack[rip]);
		auto rtype = getValueType(stack[rip]);
		rip += 1 + calculateOffset(stack[rip]);

		double l = 0, r = 0;

		if (ltype == DOUBLE)
			l = *(double *)ptrl;
		else if (ltype == INT)
			l = *(int *)ptrl;

		if (rtype == DOUBLE)
			r = *(double *)ptrr;
		else if (rtype == INT)
			r = *(int *)ptrr;

		int comp_res = (l < r ? 1 : (l == r ? 0 : -1));

		*(double *)ptrl = 0;
		if (op == OP_LT)
			*(int *)ptrl = (comp_res == 1);
		if (op == OP_LEQ)
			*(int *)ptrl = (comp_res > -1);
		if (op == OP_GT)
			*(int *)ptrl = (comp_res == -1);
		if (op == OP_GEQ)
			*(int *)ptrl = (comp_res < 1);
		if (op == OP_EQ)
			*(int *)ptrl = (comp_res == 0);
		if (op == OP_NEQ)
			*(int *)ptrl = (comp_res != 1);
	}

public:
	StackVM(istream &_input, size_t size = 0x100000) : input(_input)
	{
		stack = new unsigned char[size];
		*(int *)&rbp = *(int *)&rsp = size;
		loadCode();
	}

	bool halted = false;
	void fetchInstruction()
	{
		nextCom = (OP_CODE)stack[rip++];
		cout << nextCom << "\t" << opCodeNames[nextCom] << "\t\t";
	}

	void execute()
	{
		int oldIp = rip;
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
			jz();
			break;
		case OP_JNZ:
			jnz();
			break;

		case OP_NOT:
			op_not();
			break;

		case OP_NEG:
			negate();
			break;

		case OP_INC:
			increment();
			break;

		case OP_DEC:
			decrement();
			break;

		case OP_ADD:
			add();
			break;

		case OP_SUB:
			sub();
			break;

		case OP_MLT:
			multiply();
			break;

		case OP_DIV:
			divide();
			break;

		case OP_REM:
			remainder();
			break;

		case OP_EQ:
		case OP_NEQ:
		case OP_GEQ:
		case OP_GT:
		case OP_LEQ:
		case OP_LT:
			cmp(nextCom);
			break;

		case OP_AND:
			op_and();
			break;

		case OP_OR:
			op_or();
			break;

		case OP_HALT:
			halt();
			break;
		}

		cout << " " << (long)rip - (long)oldIp + 1 << ";\n";
		if (nextCom == OP_HALT)
		{
			dumpRegisters();
			cout << "\n";
		}
	}

	~StackVM()
	{
		delete[] stack;
	}
};

#endif // STACK_VM_H_
