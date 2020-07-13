#ifndef SVM_VISITORS_H_
#define SVM_VISITORS_H_

#include <iostream>
#include <sstream>
#include "visitor.h"
#include "../../common/enums.h"
#include "symbol_tables.h"
#include "utils.h"
using namespace std;

typedef vector<InstructionArgument*> op_args;

void initNodeScope(ostream &buffer, int &ip, int scope)
{
	auto rsp = getRegisterInstructionArgument(REG_RSP, INT);
	// get variable size for current scope
	// subtract from RSP current scope size
	// set next scope additional task size
}

void initFunctionStackFrame(ostream &buffer, int &ip, int scope)
{
	auto rbx = getRegisterInstructionArgument(REG_RBX, INT);
	auto rbp = getRegisterInstructionArgument(REG_RBP, INT);
	auto rsp = getRegisterInstructionArgument(REG_RSP, INT);

	ip += writeInstruction(OP_PUSH, op_args{ rbx }, buffer);

	// subtract from SP argument size
	// set next scope additional stack size
	// calculate variables and move to placement
	// mov %rbx (ip + 13)
	// jump to function
	// add to SP argument size
	// subtract from additional stack size
	// pop rbx
}
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
	vector<ASTNode *> bssVars;
	vector<ASTNode *> dataVars;
	CompilationVisitor(ostream &_buffer, int &_ip) : buffer(_buffer), ip(_ip) {}
	virtual ~CompilationVisitor() {}

	virtual void visit(ProgramNode *node)
	{
		auto nodes = node->getStatements();
		cout << nodes.size() << endl;
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i] == nullptr)
				continue;
			nodes[i]->accept(this);
		}
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

		for (vector<ASTNode *> v = bssVars; v != dataVars; v = dataVars)
		{
			for (size_t i = 0; i < v.size(); ++i)
			{
				v[i]->accept(varVisitor);
				if (varVisitor->variableName == varName)
				{
					v.erase(v.begin() + i);
					break;
				}
			}
		}

		auto valVisitor = new ValueVisitor();
		node->getValue()->accept(valVisitor);
		if (valVisitor->isInitialized)
		{
			dataVars.push_back(node);
		}
		else
		{
			bssVars.push_back(node);
		}

		cout << "assignment\n";
	}
};

class ExpressionCompilerVisitor : public Visitor
{
private:
	int scope;
	int &ip;
	InstructionArgument *detail;

public:
	ostream &buffer;
	ExpressionCompilerVisitor(ostream &_buffer, int _scope, int &_ip) : buffer(_buffer), scope(_scope), ip(_ip) {}
	virtual ~ExpressionCompilerVisitor() {}

	virtual void visit(FunctionInvocationNode *node)
	{
		auto rbx = getRegisterInstructionArgument(REG_RBX, INT);
		auto rbp = getRegisterInstructionArgument(REG_RBP, INT);
		auto rsp = getRegisterInstructionArgument(REG_RSP, INT);

		// TODO:
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
			ip += writeInstruction(OP_PUSH, op_args{ cmpr->detail }, buffer);
			rightOperand = getRegisterInstructionArgument(REG_RSP, cmpr->detail->type);
		}

		left->accept(cmpl);
		auto rax = getRegisterInstructionArgument(REG_RAX, cmpl->detail->type);
		if (cmpl->detail->placement != PLC_REGISTER || cmpl->detail->reg != REG_RAX)
		{
			ip += writeInstruction(OP_MOV, op_args{ rax, cmpl->detail }, buffer);
		}
		ip += writeInstruction(getOperatorOpCode(node->getOperation()), op_args{ rax, rightOperand }, buffer);

		if (rightOperand != cmpr->detail)
		{
			auto rsp = getRegisterInstructionArgument(REG_RSP, rightOperand->type);
			auto off = getImmediateArgument(getValueTypeOffset(cmpr->detail->type));
			ip += writeInstruction(OP_ADD, op_args{ rsp, off }, buffer);
			delete rsp;
			delete off;
		}
		delete cmpr->detail;
		delete cmpl->detail;

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
			case OP_LEQ: type = INT; break;
			default:	 type = (cmpl->detail->type == INT || cmpr->detail->type == INT ? INT : DOUBLE);
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

		auto opCode = (node->getOperator() == "=" ? OP_MOV : getOperatorOpCode("" + node->getOperator()[0]));
		ip += writeInstruction(opCode, op_args{ calcvar->detail, calcval->detail }, buffer);
		detail = calcvar->detail;
		delete calcval->detail;
	}

	virtual void visit(PostfixUnaryExpressionNode *node)
	{
		auto inner = node->getVariable();
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		inner->accept(comp);
		
		auto opCode = getOperatorOpCode(node->getOperation());
		auto rax	= getRegisterInstructionArgument(REG_RAX, comp->detail->type); 
		ip += writeInstruction(opCode, op_args{ comp->detail }, buffer);
		ip += writeInstruction(OP_MOV, op_args{ rax, comp->detail }, buffer);

		detail = comp->detail;
	}
	
	virtual void visit(PrefixUnaryExpressionNode *node)
	{
		auto inner = node->getVariable();
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		inner->accept(comp);
		
		auto opCode = getOperatorOpCode(node->getOpreation());
		auto rax	= getRegisterInstructionArgument(REG_RAX, comp->detail->type); 
		ip += writeInstruction(OP_MOV, op_args{ rax, comp->detail }, buffer);
		ip += writeInstruction(opCode, op_args{ comp->detail }, buffer);

		detail = comp->detail;
	}

	virtual void visit(NegationNode *node)
	{
		auto inner = node->getInner();
		auto comp = new ExpressionCompilerVisitor(buffer, scope, ip);
		inner->accept(comp);

		auto rax = getRegisterInstructionArgument(REG_RAX, comp->detail->type);
		if (comp->detail->placement != PLC_REGISTER || comp->detail->reg != REG_RAX)
		{
			ip += writeInstruction(OP_MOV, op_args{ rax, comp->detail }, buffer);
		}
		ip += writeInstruction(OP_NEG, op_args{ rax }, buffer);
		detail = new InstructionArgument();
		detail->type = INT;
		detail->placement = PLC_REGISTER;
		detail->reg = REG_RAX;
		delete comp->detail;
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
			ip += writeInstruction(OP_MOV, op_args{ rax, comp->detail }, buffer);
		}
		ip += writeInstruction(OP_NOT, op_args{ rax }, buffer);
		detail = new InstructionArgument();
		detail->type = INT;
		detail->placement = PLC_REGISTER;
		detail->reg = REG_RAX;
		delete comp->detail;
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
		int offset = getVariableOffset(node->getName(), scope);
		detail = getRegisterOffsetArgument(REG_RBP, getVariableType(node->getName(), scope), offset);
	}

	virtual void visit(VariableDeclarationNode *node)
	{
		int offset = getVariableOffset(node->getName(), scope);
		detail = getRegisterOffsetArgument(REG_RBP, getVariableType(node->getName(), scope), offset);
	}
};

class StatementCompiler : public Visitor
{
private:
	int scope;
	int &ip;
	ostream &buffer;

public:
	StatementCompiler(ostream &_buffer, int _scope, int &_ip) : scope(_scope), ip(_ip), buffer(_buffer) {}
	virtual ~StatementCompiler() {}

	virtual void visit(ForLoopNode *node)
	{
	}

	virtual void visit(WhileLoopNode *node)
	{
	}

	virtual void visit(IfNode *node)
	{
	}

	virtual void visit(ReturnNode *node)
	{
	}

	virtual void visit(BlockStatementNode *node)
	{
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
};

class FunctionCompiler
{
private:
	ostream &buffer;
	int &ip;
	int scope;
	vector<ASTNode *> nodes;

public:
	FunctionCompiler(ostream &_buffer, int &_ip, int _scope, const vector<ASTNode *> &_nodes) : buffer(_buffer), ip(_ip), scope(_scope), nodes(_nodes) {}

	void compile()
	{
		for (auto statement : nodes)
		{
			auto sc = new StatementCompiler(buffer, scope, ip);
			statement->accept(sc);
			delete sc;
		}
	}
};

#endif // SVM_VISITORS_H_#ifndef SVM_VISITORS_H_
