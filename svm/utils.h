#ifndef SVM_OP_CODES_H_
#define SVM_OP_CODES_H_

#include <unordered_map>
#include <string>
#include "../common/enums.h"
using namespace std;

typedef pair<OP_CODE, string> opCodeEntry;
typedef pair<ARGUMENT_PLACEMENT, string> placementEntry;
typedef pair<REGISTER, string> registerEntry;

void setRegisterValue(unsigned long &reg, int value) { reg = 0; *(int*)&reg = value; }
void setRegisterValue(unsigned long &reg, double value) { *(double*)&reg = value; }

unordered_map<OP_CODE, string> opCodeNames{
	opCodeEntry{OP_PUSH, "push"},
	opCodeEntry{OP_POP, "pop"},
	opCodeEntry{OP_MOV, "mov"},
	opCodeEntry{OP_JMP, "jump"},
	opCodeEntry{OP_JZ, "jump-zero"},
	opCodeEntry{OP_JNZ, "jump-not-zero"},
	opCodeEntry{OP_NOT, "not"},
	opCodeEntry{OP_NEG, "negate"},
	opCodeEntry{OP_INC, "increment"},
	opCodeEntry{OP_DEC, "decrement"},
	opCodeEntry{OP_ADD, "add"},
	opCodeEntry{OP_SUB, "subtract"},
	opCodeEntry{OP_MLT, "multiply"},
	opCodeEntry{OP_DIV, "divide"},
	opCodeEntry{OP_REM, "remainder"},
	opCodeEntry{OP_EQ, "equal"},
	opCodeEntry{OP_NEQ, "not-equal"},
	opCodeEntry{OP_GEQ, "greater-or-eqal"},
	opCodeEntry{OP_GT, "greater"},
	opCodeEntry{OP_LEQ, "less-or-equal"},
	opCodeEntry{OP_LT, "less-than"},
	opCodeEntry{OP_AND, "and"},
	opCodeEntry{OP_OR, "or"},
	opCodeEntry{OP_HALT, "halt"},
};

unordered_map<ARGUMENT_PLACEMENT, string> placementNames{
	placementEntry{PLC_REGISTER, "register"},
	placementEntry{PLC_REGISTER_OFFSET, "register-w-offset"},
	placementEntry{PLC_IMMEDIATE, "immediate"},
};

unordered_map<REGISTER, string> registerNames{
	registerEntry{REG_RIP, "instruction pointer"},
	registerEntry{REG_RBP, "base pointer"},
	registerEntry{REG_RSP, "stack pointer"},
	registerEntry{REG_RAX, "RAX"},
	registerEntry{REG_RBX, "RBX"},
	registerEntry{REG_DAT, "Data pointer"},
	registerEntry{REG_BSS, "BSS pointer"},
};

ARGUMENT_PLACEMENT getPlacement(unsigned char meta)
{
	return static_cast<ARGUMENT_PLACEMENT>(meta >> 6);
}

REGISTER getRegister(unsigned char meta)
{
	return static_cast<REGISTER>((meta >> 3) & 0b111);
}

ValueType getValueType(unsigned char meta)
{
	return static_cast<ValueType>(meta & 0b11);
}

#endif