#ifndef SYMBOL_TABLES_H_
#define SYMBOL_TABLES_H_

#include <iostream>
#include "node_factory.h"
#include "../../common/enums.h"
#include "./utils.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

unordered_map<int, int> scopeParents;

unordered_map<string, ASTNode *> bssVars;
unordered_map<string, ASTNode *> dataVars;
unordered_map<int, vector<VariableDeclarationNode *>> variableSymbolTable;

unordered_map<string, int> functionBegining;

unordered_map<string, FunctionDeclarationNode *> functionSymbolTable;
unordered_map<ASTNode *, int> nodeToScopeMap;
unordered_map<int, vector<ArgumentData *>> scopeArgumentsMap;

vector<FunctionDeclarationNode *> functionsInOrder;

OP_CODE getOperatorOpCode(const string &op, bool prefix = false)
{
	if (op == "+")
		return OP_ADD;
	if (op == "-")
		return OP_SUB;
	if (op == "*")
		return OP_MLT;
	if (op == "/")
		return OP_DIV;
	if (op == "%")
		return OP_REM;

	if (op == "!")
		return OP_NOT;
	if (op == "-")
		return OP_NEG;

	if (op == "++")
		return OP_INC;
	if (op == "--")
		return OP_DEC;

	if (op == "==")
		return OP_EQ;
	if (op == "!=")
		return OP_NEQ;
	if (op == ">=")
		return OP_GEQ;
	if (op == ">")
		return OP_GT;
	if (op == "<=")
		return OP_LEQ;
	if (op == "<")
		return OP_LT;

	if (op == "&&")
		return OP_AND;
	if (op == "||")
		return OP_OR;

	if (op == "+=")
		return OP_ADD;
	if (op == "-=")
		return OP_SUB;
	if (op == "*=")
		return OP_MLT;
	if (op == "/=")
		return OP_DIV;
	if (op == "/=")
		return OP_REM;

	return OP_HALT;
}

ValueType getValueType(string type)
{
	if (type == "int")
		return INT;
	if (type == "double")
		return DOUBLE;
	return UNKNOWN;
}

int getValueTypeOffset(ValueType type)
{
	switch (type)
	{
	case INT:
		return 4;
	case DOUBLE:
		return 8;
	default:
		return 0;
	}
}

ValueType getVariableType(const string &var, int scope)
{
	for (auto x = scope; scopeParents.find(x) != scopeParents.end(); x = scopeParents[x])
	{
		auto scope_vars = variableSymbolTable[x];
		for (size_t i = 0; i < scope_vars.size(); ++i)
		{
			if (scope_vars[i]->getName() == var)
			{
				return scope_vars[i]->getValueType();
			}
		}
	}

	return UNKNOWN;
}

InstructionArgument *getLocalVariableOffset(const string &var, int scope)
{
	int offset = 0;
	bool shouldSubtract = false;
	ValueType type;

	for (int s = scope; s != 0; s = scopeParents[s])
	{
		auto symbolTable = variableSymbolTable[s];
		for (int i = symbolTable.size() - 1; i >= 0; --i)
		{
			if (var == symbolTable[i]->getName())
			{
				shouldSubtract = true;
				type = symbolTable[i]->getValueType();
			}
			offset -= shouldSubtract * getValueTypeOffset(symbolTable[i]->getValueType());
		}
	}

	if (offset == 0)
	{
		return nullptr;
	}
	return getRegisterOffsetArgument(REG_RBP, type, offset - 8);
}

InstructionArgument *getGlobalVariableOffset(const string &var)
{
	int bssOffset = 0, dataOffset = 0;
	REGISTER reg;
	auto scope = variableSymbolTable[0];

	for (size_t i = 0; i < scope.size(); ++i)
	{
		auto node = scope[i];
		if (bssVars.find(node->getName()) != bssVars.end())
			reg = REG_BSS;
		else
			reg = REG_DAT;

		if (node->getName() == var)
			{
				cout << var << " " << reg << " " << (reg == REG_BSS ? bssOffset : dataOffset) << endl;
				return getRegisterOffsetArgument(reg, node->getValueType(), reg == REG_BSS ? bssOffset : dataOffset);
			}

		if (reg == REG_BSS)
			bssOffset += getValueTypeOffset(node->getValueType());
		else
			dataOffset += getValueTypeOffset(node->getValueType());
	}

	return nullptr;
}

int getDataSize()
{
	int dataOffset = 0;
	auto scope = variableSymbolTable[0];

	for (size_t i = 0; i < scope.size(); ++i)
	{
		auto node = scope[i];
		if (bssVars.find(node->getName()) == bssVars.end())
			dataOffset += getValueTypeOffset(node->getValueType());
	}

	return dataOffset;
}

void addScope(int scope, int scopeParent = -1)
{
	// variableSymbolTable[scope];
	scopeParents[scope] = scopeParent;
}

void setNodeScope(ASTNode *node, int scope)
{
	nodeToScopeMap[node] = scope;
}

int getNodeScope(ASTNode *node)
{
	return nodeToScopeMap[node];
}

void addScopeVariable(int scope, const string &var, VariableDeclarationNode *data)
{
	variableSymbolTable[scope].push_back(data);
}

bool doesFunctionExist(const string &name)
{
	return functionSymbolTable.find(name) != functionSymbolTable.end();
}

void addFunction(const string &name, FunctionDeclarationNode *data, int scope)
{
	if (functionSymbolTable.find(name) != functionSymbolTable.end())
	{
		if (functionSymbolTable[name]->hasFunctionBody() || !functionSymbolTable[name]->hasSameArguments(data))
		{
			throw runtime_error("cannot redefine function");
		}
		auto funcNode = functionSymbolTable[name];
		nodeToScopeMap.erase((ASTNode *)funcNode);
		delete funcNode;
		for (size_t i = 0; i < functionsInOrder.size(); ++i)
		{
			if (functionsInOrder[i]->getName() == name)
			{
				functionsInOrder[i] = data;
				break;
			}
		}
	}
	else
	{
		functionsInOrder.push_back(data);
	}
	functionSymbolTable[name] = data;
	scopeArgumentsMap[scope] = data->getArguments();
	return;
}

bool checkFunctionCall(FunctionInvocationNode *invocation)
{
	return functionSymbolTable.find(invocation->getName()) != functionSymbolTable.end() && functionSymbolTable[invocation->getName()]->getArgCount() == invocation->getArgCount();
}

InstructionArgument *getArgumentOffsetByName(int scope, const string &name)
{
	if (scope == 0)
		return nullptr;
	while (scopeParents[scope] != 0)
		scope = scopeParents[scope];

	int offset = 0;
	auto args = scopeArgumentsMap[scope];

	bool found = false;
	ValueType type;
	for (int i = args.size() - 1; i >= 0; --i)
	{
		if (args[i]->name == name)
		{
			type = args[i]->type;
			found = true;
			break;
		}
		offset += getValueTypeOffset(args[i]->type);
	}

	if (!found)
		return nullptr;
	return getRegisterOffsetArgument(REG_RBP, type, offset + 8);
}

bool variableContainedInScope(const string &var, int scope)
{
	return (getLocalVariableOffset(var, scope) || getArgumentOffsetByName(scope, var) || getGlobalVariableOffset(var));
}

int getArgumentOffset(FunctionInvocationNode *node, int index)
{
	auto funcDecl = functionSymbolTable[node->getName()];
	auto args = funcDecl->getArguments();

	int offset = 0;
	for (size_t i = args.size() - 1; i > index; --i)
	{
		offset += getValueTypeOffset(args[i]->type);
	}

	return offset;
}

ValueType getArgumentType(FunctionInvocationNode *node, int index)
{
	auto funcDecl = functionSymbolTable[node->getName()];
	auto args = funcDecl->getArguments();
	return args[index]->type;
}

int getFunctionArgumentSize(FunctionInvocationNode *node)
{
	int size = 0;
	auto funcDecl = functionSymbolTable[node->getName()];
	auto args = funcDecl->getArguments();
	for (auto arg : args)
	{
		size += getValueTypeOffset(arg->type);
	}

	return size;
}

#endif // SYMBOL_TABLES_H_
