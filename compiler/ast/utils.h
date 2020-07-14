#ifndef SVM_UTILS_H_
#define SVM_UTILS_H_

#include "../../common/enums.h"

struct InstructionArgument
{
	ARGUMENT_PLACEMENT placement;
	REGISTER reg;
	ValueType type;
	unsigned long data;
};

InstructionArgument *getRegisterInstructionArgument(REGISTER reg, ValueType type)
{
	auto data = new InstructionArgument();
	data->placement = PLC_REGISTER;
	data->reg = reg;
	data->type = type;
	return data;
}

InstructionArgument *getRegisterOffsetArgument(REGISTER reg, ValueType type, int offset)
{
	auto data = new InstructionArgument();
	data->placement = PLC_REGISTER_OFFSET;
	data->reg = reg;
	data->type = type;
	unsigned long value = 0;
	((int*)&value)[1] = offset;
	data->data = value;
	return data;
}

InstructionArgument *getImmediateArgument(int value)
{
	auto data = new InstructionArgument();
	data->placement = PLC_IMMEDIATE;
	data->type = INT;
	unsigned long x = 0;
	((int*)&x)[1] = value;
	data->data = x;
	return data;
}

InstructionArgument *getImmediateArgument(double value)
{
	auto data = new InstructionArgument();
	data->placement = PLC_IMMEDIATE;
	data->type = INT;
	unsigned long x = 0;
	x = *(long*)(&value);
	data->data = x;
	return data;
}

int writeInstruction(OP_CODE op, const vector<InstructionArgument*> &args, ostream &buffer)
{
	int instructionLength = 1;
	cout << op << endl;
	
	buffer << static_cast<unsigned char>(op);
	for (auto arg : args)
	{
		unsigned char argument_meta = arg->placement << 6;
		if (arg->placement == PLC_REGISTER || arg->placement == PLC_REGISTER_OFFSET)
		{
			argument_meta |= (0x7 & arg->reg) << 3;
		}
		argument_meta |= (0x3 & arg->type);

		buffer << argument_meta;

		++instructionLength;
		if (arg->placement == PLC_REGISTER)
		{
			continue;
		}
		unsigned short arg_beg, arg_end = 8;
		if (arg->placement == PLC_REGISTER_OFFSET)
		{
			arg_beg = 4;
		}
		if (arg->placement == PLC_IMMEDIATE)
		{
			arg_beg = (arg->type == INT ? 4 : 0);
		}

		for (unsigned short i = arg_beg; i < arg_end; ++i)
		{
			buffer << ((unsigned char*)&(arg->data))[i];
		}

		instructionLength += arg_end - arg_beg;
	}

	cout << "Length: " << instructionLength << endl;
	return instructionLength;
}

#endif // SVM_UTILS_H_
