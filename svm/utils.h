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
	opCodeEntry{OP_JZ, "jump-z"},
	opCodeEntry{OP_JNZ, "jump-nz"},
	opCodeEntry{OP_NOT, "not"},
	opCodeEntry{OP_NEG, "negate"},
	opCodeEntry{OP_INC, "incr"},
	opCodeEntry{OP_DEC, "decr"},
	opCodeEntry{OP_ADD, "add"},
	opCodeEntry{OP_SUB, "subtr"},
	opCodeEntry{OP_MLT, "mult"},
	opCodeEntry{OP_DIV, "divide"},
	opCodeEntry{OP_REM, "rem"},
	opCodeEntry{OP_EQ, "equal"},
	opCodeEntry{OP_NEQ, "nequal"},
	opCodeEntry{OP_GEQ, "gequal"},
	opCodeEntry{OP_GT, "greater"},
	opCodeEntry{OP_LEQ, "lequel"},
	opCodeEntry{OP_LT, "less"},
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
	registerEntry{REG_RIP, "RIP"},
	registerEntry{REG_RBP, "RBP"},
	registerEntry{REG_RSP, "RSP"},
	registerEntry{REG_RAX, "RAX"},
	registerEntry{REG_RBX, "RBX"},
	registerEntry{REG_DAT, "DAT"},
	registerEntry{REG_BSS, "BSS"},
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