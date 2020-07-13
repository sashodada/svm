#ifndef SVM_ENUMS_H_
#define SVM_ENUMS_H_

enum OP_CODE
{
	// stack 
	OP_PUSH,
	OP_POP,

	// move
	OP_MOV,

	// jumps
	OP_JMP,
	OP_JNZ,
	
	// unary operators
	OP_NOT,
	OP_NEG,
	OP_INC,
	OP_DEC,

	// binary operators
	OP_ADD,
	OP_SUB,
	OP_MLT,
	OP_DIV,
	OP_REM,

	// comparison
	OP_EQ,
	OP_NEQ,
	OP_GEQ,
	OP_GT,
	OP_LEQ,
	OP_LT,

	// logical
	OP_AND,
	OP_OR,

	// halt
	OP_HALT
};

enum ARGUMENT_PLACEMENT
{
	PLC_REGISTER,
	PLC_REGISTER_OFFSET,
	PLC_IMMEDIATE
};

enum REGISTER
{
	REG_RIP,
	REG_RBP,
	REG_RSP,
	REG_RAX,
	REG_RBX,
	REG_DAT,
	REG_BSS
};

#endif // SVM_ENUMS_H_
