#ifndef SVM_VISITORS_H_
#define SVM_VISITORS_H_

#include <iostream>
#include <sstream>
#include "visitor.h"
#include "../../common/enums.h"
#include "symbol_tables.h"
#include "utils.h"
using namespace std;

typedef vector<InstructionArgument *> op_args;

class VariableVisitor : public Visitor
{
public:
	string variableName = "";
	ValueType vtype = UNKNOWN;
	virtual void visit(VariableDeclarationNode *node)
	{
		variableName = node->getName();
		this->vtype = node->getValueType();
	}

	virtual void visit(PostfixUnaryExpressionNode *node)
	{
		auto var = node->getVariable();
		var->accept(this);
	}

	virtual void visit(PrefixUnaryExpressionNode *node)
	{
		auto var = node->getVariable();
		var->accept(this);
	}

	virtual void visit(VariableReferenceNode *node)
	{
		variableName = node->getName();
	}

	virtual void visit(AssignmentNode *node)
	{
		auto var = node->getVariable();
		var->accept(this);
	}
};

class ValueVisitor : public Visitor
{
public:
	bool isInitialized = true;

	virtual void visit(IntLiteralNode *node)
	{
		isInitialized = (node->getValue() != 0);
	}

	virtual void visit(DoubleLiteralNode *node)
	{
		isInitialized = (node->getValue() != 0);
	}
};

class CompilationVisitor : public Visitor
{
private:
	ostream &buffer;
	int &ip;

public:
	unordered_map<string, ValueType> variableTypes;
	vector<ASTNode *> _bssVars;
	vector<ASTNode *> _dataVars;
	CompilationVisitor(ostream &_buffer, int &_ip) : buffer(_buffer), ip(_ip) {}
	virtual ~CompilationVisitor() {}

	virtual void visit(ProgramNode *node)
	{
		auto nodes = node->getStatements();
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i] == nullptr)
				continue;
			nodes[i]->accept(this);
		}

		auto varVisitor = new VariableVisitor();
		for (auto v : _bssVars)
		{
			v->accept(varVisitor);
			bssVars[varVisitor->variableName] = v;
		}
		for (auto v : _dataVars)
		{
			v->accept(varVisitor);
			dataVars[varVisitor->variableName] = v;
		}
	}

	virtual void visit(VariableDeclarationNode *node)
	{
		auto varVisitor = new VariableVisitor();
		node->accept(varVisitor);
		if (variableTypes.find(varVisitor->variableName) != variableTypes.end())
		{
			throw runtime_error("variable cannot be redeclared");
		}
		variableTypes[varVisitor->variableName] = varVisitor->vtype;

		string varName = varVisitor->variableName;

		for (size_t i = 0; i < _bssVars.size(); ++i)
		{
			_bssVars[i]->accept(varVisitor);
			if (varVisitor->variableName == varName)
			{
				_bssVars.erase(_bssVars.begin() + i);
				break;
			}
		}

		_bssVars.push_back(node);
	}

	virtual void visit(AssignmentNode *node)
	{
		auto varVisitor = new VariableVisitor();
		node->getVariable()->accept(varVisitor);
		if (varVisitor->vtype == UNKNOWN)
		{
			if (variableTypes.find(varVisitor->variableName) == variableTypes.end())
			{
				throw runtime_error("variable not declared");
			}
			varVisitor->vtype = variableTypes[varVisitor->variableName];
		}
		else
		{
			variableTypes[varVisitor->variableName] = varVisitor->vtype;
		}

		string varName = varVisitor->variableName;

		for (size_t i = 0; i < _bssVars.size(); ++i)
		{
			_bssVars[i]->accept(varVisitor);
			if (varVisitor->variableName == varName)
			{
				_bssVars.erase(_bssVars.begin() + i);
				break;
			}
		}

		for (size_t i = 0; i < _dataVars.size(); ++i)
		{
			_dataVars[i]->accept(varVisitor);
			if (varVisitor->variableName == varName)
			{
				_dataVars.erase(_dataVars.begin() + i);
				break;
			}
		}

		auto valVisitor = new ValueVisitor();
		node->getValue()->accept(valVisitor);
		if (valVisitor->isInitialized)
		{
			_dataVars.push_back(node);
		}
		else
		{
			_bssVars.push_back(node);
		}
	}
};

void compileBlockStatementsHelper(ostream &buffer, int scope, int &ip, BlockStatementNode *node);

void copyDetail(InstructionArgument *left, InstructionArgument *right)
{
	left->data = right->data;
	left->placement = right->placement;
	left->reg = right->reg;
	left->type = right->type;
}

class ExpressionCompilerVisitor : public Visitor
{
private:
	int scope;
	int &ip;
	ostream &buffer;

public:
	InstructionArgument *detail;
	ExpressionCompilerVisitor(ostream &_buffer, int _scope, int &_ip) : buffer(_buffer), scope(_scope), ip(_ip)
	{
		detail = new InstructionArgument();
	}

	virtual ~ExpressionCompilerVisitor() { delete detail; }

	virtual void visit(FunctionInvocationNode *node)
	{
		auto rbx = getRegisterInstructionArgument(REG_RBX, INT);
		auto rbp = getRegisterInstructionArgument(REG_RBP, INT);
		auto rsp = getRegisterInstructionArgument(REG_RSP, INT);

		int argSize = getFunctionArgumentSize(node);
		cout << "argSize " << argSize << endl;
		auto argOffset = getImmediateArgument(argSize);

		ip += writeInstruction(OP_PUSH, op_args{rbx}, buffer);
		ip += writeInstruction(OP_SUB, op_args{rsp, argOffset}, buffer);

		auto args = node->getArguments();
		for (int i = 0; i < args.size(); ++i)
		{
			cout << "here" << endl;
			int currIp = ip;
			auto arg = args[i];
			auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
			args[i]->accept(comp);
			int argOffsetSize = getArgumentOffset(node, i);
			ValueType argType = getArgumentType(node, i);
			auto argOffset = getRegisterOffsetArgument(REG_RSP, argType, argOffsetSize);
			ip += writeInstruction(OP_MOV, op_args{argOffset, comp->detail}, buffer);
		}

		/**
		 * instructions:
		 * MOV: 			1
		 * register: 		1
		 * immediate:		5
		 * SUBTOTAL			7+
		 * 
		 * JMP:				1
		 * immediate:		5
		 * SUBTOTAL			6+
		 * ===================
		 * TOTAL		   13
		 */
		int newIp = ip + 13;
		auto anchor = getImmediateArgument(newIp);
		ip += writeInstruction(OP_MOV, op_args{rbx, anchor}, buffer);

		int functionAddress = functionBegining[node->getName()];
		auto jump = getImmediateArgument(functionAddress);

		ip += writeInstruction(OP_JMP, op_args{jump}, buffer);
		ip += writeInstruction(OP_ADD, op_args{rsp, argOffset}, buffer);
		ip += writeInstruction(OP_POP, op_args{rbx}, buffer);

		detail->reg = REG_RAX;
		detail->placement = PLC_REGISTER;
		detail->type = getValueType(functionSymbolTable[node->getName()]->getType());
	}

	virtual void visit(BlockStatementNode *node)
	{
		compileBlockStatementsHelper(buffer, scope, ip, node);
	}

	virtual void visit(BinaryExpressionNode *node)
	{
		auto left = node->getLeft();
		auto right = node->getRight();

		auto cmpl = new ExpressionCompilerVisitor(buffer, scope, ip);
		auto cmpr = new ExpressionCompilerVisitor(buffer, scope, ip);

		right->accept(cmpr);
		auto rightOperand = cmpr->detail;
		if (cmpr->detail->placement == PLC_REGISTER && cmpr->detail->reg == REG_RAX)
		{
			ip += writeInstruction(OP_PUSH, op_args{cmpr->detail}, buffer);
			rightOperand = getRegisterOffsetArgument(REG_RSP, cmpr->detail->type, 0);
		}

		left->accept(cmpl);
		auto rax = getRegisterInstructionArgument(REG_RAX, cmpl->detail->type);
		if (cmpl->detail->placement != PLC_REGISTER || cmpl->detail->reg != REG_RAX)
		{
			ip += writeInstruction(OP_MOV, op_args{rax, cmpl->detail}, buffer);
		}
		ip += writeInstruction(getOperatorOpCode(node->getOperation()), op_args{rax, rightOperand}, buffer);

		if (rightOperand != cmpr->detail)
		{
			auto rsp = getRegisterInstructionArgument(REG_RSP, rightOperand->type);
			auto off = getImmediateArgument(getValueTypeOffset(cmpr->detail->type));
			ip += writeInstruction(OP_ADD, op_args{rsp, off}, buffer);
			delete rsp;
			delete off;
		}

		ValueType type;
		switch (getOperatorOpCode(node->getOperation()))
		{
		case OP_AND:
		case OP_OR:
		case OP_EQ:
		case OP_NEQ:
		case OP_GEQ:
		case OP_GT:
		case OP_LT:
		case OP_LEQ:
			type = INT;
			break;
		default:
			type = (cmpl->detail->type == INT || cmpr->detail->type == INT ? INT : DOUBLE);
		};

		detail->type = type;
		detail->placement = PLC_REGISTER;
		detail->reg = REG_RAX;
	}

	virtual void visit(AssignmentNode *node)
	{
		auto var = node->getVariable(), val = node->getValue();
		auto calcvar = new ExpressionCompilerVisitor(buffer, scope, ip);
		auto calcval = new ExpressionCompilerVisitor(buffer, scope, ip);
		var->accept(calcvar);
		val->accept(calcval);

		auto opCode = (node->getOperator() == "=" ? OP_MOV : getOperatorOpCode(node->getOperator()));
		ip += writeInstruction(opCode, op_args{calcvar->detail, calcval->detail}, buffer);

		copyDetail(detail, calcvar->detail);
	}

	virtual void visit(PostfixUnaryExpressionNode *node)
	{
		auto inner = node->getVariable();
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		inner->accept(comp);

		auto opCode = getOperatorOpCode(node->getOperation());
		auto rax = getRegisterInstructionArgument(REG_RAX, comp->detail->type);
		ip += writeInstruction(opCode, op_args{comp->detail}, buffer);
		ip += writeInstruction(OP_MOV, op_args{rax, comp->detail}, buffer);

		copyDetail(detail, comp->detail);
	}

	virtual void visit(PrefixUnaryExpressionNode *node)
	{
		auto inner = node->getVariable();
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		inner->accept(comp);

		auto opCode = getOperatorOpCode(node->getOpreation());
		auto rax = getRegisterInstructionArgument(REG_RAX, comp->detail->type);
		ip += writeInstruction(OP_MOV, op_args{rax, comp->detail}, buffer);
		ip += writeInstruction(opCode, op_args{comp->detail}, buffer);

		copyDetail(detail, comp->detail);
	}

	virtual void visit(NegationNode *node)
	{
		auto inner = node->getInner();
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		inner->accept(comp);

		auto rax = getRegisterInstructionArgument(REG_RAX, comp->detail->type);
		if (comp->detail->placement != PLC_REGISTER || comp->detail->reg != REG_RAX)
		{
			ip += writeInstruction(OP_MOV, op_args{rax, comp->detail}, buffer);
		}
		ip += writeInstruction(OP_NEG, op_args{rax}, buffer);
		detail->type = INT;
		detail->placement = PLC_REGISTER;
		detail->reg = REG_RAX;
		delete comp;
		delete rax;
	}

	virtual void visit(BooleanNegationNode *node)
	{
		auto inner = node->getInner();
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		inner->accept(comp);

		auto rax = getRegisterInstructionArgument(REG_RAX, comp->detail->type);
		if (comp->detail->placement != PLC_REGISTER || comp->detail->reg != REG_RAX)
		{
			ip += writeInstruction(OP_MOV, op_args{rax, comp->detail}, buffer);
		}
		ip += writeInstruction(OP_NOT, op_args{rax}, buffer);
		detail->type = INT;
		detail->placement = PLC_REGISTER;
		detail->reg = REG_RAX;
		delete comp;
		delete rax;
	}

	virtual void visit(IntLiteralNode *node)
	{
		detail = getImmediateArgument(node->getValue());
	}

	virtual void visit(DoubleLiteralNode *node)
	{
		detail = getImmediateArgument(node->getValue());
	}

	virtual void visit(VariableReferenceNode *node)
	{
		auto offset = getLocalVariableOffset(node->getName(), scope);
		if (!offset)
			offset = getArgumentOffsetByName(scope, node->getName());
		if (!offset)
			offset = getGlobalVariableOffset(node->getName());
		if (!offset)
			throw runtime_error("Cannot find variable " + node->getName());

		detail = offset;
	}

	virtual void visit(VariableDeclarationNode *node)
	{
		auto offset = getLocalVariableOffset(node->getName(), scope);
		if (!offset)
			offset = getArgumentOffsetByName(scope, node->getName());
		if (!offset)
			offset = getGlobalVariableOffset(node->getName());
		if (!offset)
			throw runtime_error("Cannot find variable " + node->getName());

		detail = offset;
	}
};

class StatementCompiler : public Visitor
{
private:
	int scope;
	int &ip;
	ostream &buffer;

	void dumpGlobals()
	{
		auto bss = getRegisterInstructionArgument(REG_BSS, INT);
		auto dataSize = getDataSize();

		auto bssOffset = getImmediateArgument(dataSize);
		ip += writeInstruction(OP_ADD, op_args{bss, bssOffset}, buffer);

		for (auto vars = &bssVars; vars != &dataVars; vars = &dataVars)
		{
			for (auto v : *vars)
			{
				auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
				v.second->accept(comp);
				delete comp;
			}
		}

		delete bssOffset;
		delete bss;
	}

public:
	StatementCompiler(ostream &_buffer, int _scope, int &_ip) : scope(_scope), ip(_ip), buffer(_buffer) {}
	virtual ~StatementCompiler() {}

	virtual void visit(ForLoopNode *node)
	{
		auto newScope = nodeToScopeMap[node];
		int offsetSize = 0;
		for (auto x : variableSymbolTable[newScope])
		{
			offsetSize += getValueTypeOffset(x->getValueType());
		}

		auto rsp = getRegisterInstructionArgument(REG_RSP, INT);
		auto offset = getImmediateArgument(offsetSize);
		ip += writeInstruction(OP_SUB, op_args{rsp, offset}, buffer);

		auto comp = new ExpressionCompilerVisitor(buffer, newScope, ip);
		node->getInitialization()->accept(comp);
		delete comp;

		auto loopBegin = getImmediateArgument(ip);

		comp = new ExpressionCompilerVisitor(buffer, newScope, ip);
		node->getBody()->accept(comp);
		delete comp;

		comp = new ExpressionCompilerVisitor(buffer, newScope, ip);
		node->getIteration()->accept(comp);
		delete comp;

		comp = new ExpressionCompilerVisitor(buffer, newScope, ip);
		node->getCondition()->accept(comp);

		ip += writeInstruction(OP_JNZ, op_args{comp->detail, loopBegin}, buffer);
		ip += writeInstruction(OP_ADD, op_args{rsp, offset}, buffer);
		delete comp;
		delete loopBegin;
		delete offset;
		delete rsp;
	}

	virtual void visit(WhileLoopNode *node)
	{
		auto newScope = nodeToScopeMap[node];
		int offsetSize = 0;
		for (auto x : variableSymbolTable[newScope])
		{
			offsetSize += getValueTypeOffset(x->getValueType());
		}

		auto rsp = getRegisterInstructionArgument(REG_RSP, INT);
		auto offset = getImmediateArgument(offsetSize);
		ip += writeInstruction(OP_SUB, op_args{rsp, offset}, buffer);

		auto loopBegin = getImmediateArgument(ip);

		auto comp = new ExpressionCompilerVisitor(buffer, newScope, ip);
		node->getBody()->accept(comp);
		delete comp;

		comp = new ExpressionCompilerVisitor(buffer, newScope, ip);
		node->getCondition()->accept(comp);
		ip += writeInstruction(OP_JNZ, op_args{comp->detail, loopBegin}, buffer);
		ip += writeInstruction(OP_ADD, op_args{rsp, offset}, buffer);
		delete comp;
		delete loopBegin;
		delete offset;
		delete rsp;
	}

	virtual void visit(IfNode *node)
	{
		auto newScope = nodeToScopeMap[node];
		int offsetSize = 0;
		for (auto x : variableSymbolTable[newScope])
		{
			offsetSize += getValueTypeOffset(x->getValueType());
		}

		auto rsp = getRegisterInstructionArgument(REG_RSP, INT);
		auto offset = getImmediateArgument(offsetSize);
		ip += writeInstruction(OP_SUB, op_args{rsp, offset}, buffer);

		auto comp = new ExpressionCompilerVisitor(buffer, newScope, ip);
		node->getCondition()->accept(comp);

		// leave place for JNZ instruction (comp->detail->type offset + immediateInt(5b) + instruction(1b) )
		int conditionResultInstructionSize =
			(comp->detail->placement == PLC_REGISTER ? 0 : (comp->detail->placement == PLC_REGISTER_OFFSET ? 4 : getValueTypeOffset(comp->detail->type)));

		ip += conditionResultInstructionSize + (node->getElseBody() ? 16 : 7);
		ostringstream newBuf;

		auto comp2 = new StatementCompiler(newBuf, newScope, ip);
		node->getBody()->accept(comp2);

		auto jumpDest = getImmediateArgument(ip);
		writeInstruction(OP_JZ, op_args{comp->detail, jumpDest}, buffer);

		buffer.write(newBuf.str().data(), newBuf.str().size());

		delete comp2;
		if (node->getElseBody())
		{
			// leave place for jump instruction
			ip += 6;
			ostringstream elseBuf;
			comp2 = new StatementCompiler(elseBuf, newScope, ip);
			node->getElseBody()->accept(comp2);
			auto jumpAfterIf = getImmediateArgument(ip);
			writeInstruction(OP_JMP, op_args{jumpAfterIf}, buffer);
			buffer.write(elseBuf.str().data(), elseBuf.str().size());
			delete jumpAfterIf;
			delete comp2;
		}

		ip += writeInstruction(OP_ADD, op_args{rsp, offset}, buffer);
		delete offset;
		delete rsp;
		delete jumpDest;
		delete comp;
	}

	virtual void visit(ReturnNode *node)
	{
		if (node->getReturnValue())
		{
			auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
			node->getReturnValue()->accept(comp);
			if (comp->detail->reg != REG_RAX)
			{
				auto rax = getRegisterInstructionArgument(REG_RAX, comp->detail->type);
				ip += writeInstruction(OP_MOV, op_args{rax, comp->detail}, buffer);
				delete rax;
			}
			delete comp;
		}

		auto rbx = getRegisterInstructionArgument(REG_RBX, INT);
		ip += writeInstruction(OP_JMP, op_args{rbx}, buffer);
		delete rbx;
	}

	virtual void visit(BlockStatementNode *node)
	{
		auto newScope = nodeToScopeMap[node];
		int offsetSize = 0;
		for (auto x : variableSymbolTable[newScope])
		{
			offsetSize += getValueTypeOffset(x->getValueType());
		}

		auto rsp = getRegisterInstructionArgument(REG_RSP, INT);
		auto offset = getImmediateArgument(offsetSize);
		ip += writeInstruction(OP_SUB, op_args{rsp, offset}, buffer);

		auto statements = node->getStatements();
		for (int i = 0; i < statements.size(); ++i)
		{
			auto comp = new StatementCompiler(buffer, newScope, ip);
			statements[i]->accept(comp);
		}

		ip += writeInstruction(OP_ADD, op_args{rsp, offset}, buffer);
	}

	virtual void visit(PrefixUnaryExpressionNode *node)
	{
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		node->accept(comp);
		delete comp;
	}

	virtual void visit(PostfixUnaryExpressionNode *node)
	{
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		node->accept(comp);
		delete comp;
	}

	virtual void visit(AssignmentNode *node)
	{
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		node->accept(comp);
		delete comp;
	}

	virtual void visit(FunctionInvocationNode *node)
	{
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		node->accept(comp);
	}

	virtual void visit(FunctionDeclarationNode *node)
	{
		functionBegining[node->getName()] = ip;

		if (node->getName() == "main")
		{
			this->dumpGlobals();
		}

		auto rbp = getRegisterInstructionArgument(REG_RBP, INT);
		auto rsp = getRegisterInstructionArgument(REG_RSP, INT);
		auto rbx = getRegisterInstructionArgument(REG_RBX, INT);
		ip += writeInstruction(OP_PUSH, op_args{rbx}, buffer);
		ip += writeInstruction(OP_PUSH, op_args{rbp}, buffer);
		ip += writeInstruction(OP_MOV, op_args{rbp, rsp}, buffer);
		int stackSize = 0;
		auto localVars = variableSymbolTable[nodeToScopeMap[node]];
		for (int i = 0; i < localVars.size(); ++i)
		{
			stackSize += getValueTypeOffset(localVars[i]->getValueType());
		}
		cout << stackSize << endl;
		auto localVariablesOffset = getImmediateArgument(stackSize);
		ip += writeInstruction(OP_SUB, op_args{rsp, localVariablesOffset}, buffer);

		// leave room for MOV %rbx (ip) - 7b
		ip += 7;
		ostringstream newBuffer;
		compileBlockStatementsHelper(newBuffer, nodeToScopeMap[node], ip, node->getBody());

		auto funcEnd = getImmediateArgument(ip);
		writeInstruction(OP_MOV, op_args{rbx, funcEnd}, buffer);
		buffer.write(newBuffer.str().data(), newBuffer.str().size());

		ip += writeInstruction(OP_MOV, op_args{rsp, rbp}, buffer);
		ip += writeInstruction(OP_POP, op_args{rbp}, buffer);
		ip += writeInstruction(OP_POP, op_args{rbx}, buffer);
		if (node->getName() == "main")
		{
			ip += writeInstruction(OP_HALT, op_args{}, buffer);
			return;
		}
		ip += writeInstruction(OP_JMP, op_args{rbx}, buffer);
	}
};

void compileBlockStatementsHelper(ostream &buffer, int scope, int &ip, BlockStatementNode *node)
{
	auto statements = node->getStatements();
	for (auto s : statements)
	{
		auto comp = new StatementCompiler(buffer, scope, ip);
		s->accept((Visitor *)comp);
	}
}

#endif // SVM_VISITORS_H_#ifndef SVM_VISITORS_H_
