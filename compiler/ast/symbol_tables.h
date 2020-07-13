#ifndef SYMBOL_TABLES_H_
#define SYMBOL_TABLES_H_

#include <iostream>
#include "node_factory.h"
#include "../../common/enums.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

unordered_map<int, int> scopeParents;
unordered_map<int, vector<VariableDeclarationNode*>> variableSymbolTable;
unordered_map<string, FunctionDeclarationNode*> functionSymbolTable;
unordered_map<ASTNode*, int> nodeToScopeMap;
unordered_map<ASTNode*, int> functionBegining;
unordered_map<int, int> scopeVariableSize;
vector<FunctionDeclarationNode*> functionsInOrder;

OP_CODE getOperatorOpCode(const string &op, bool prefix = false)
{
	if (op == "+") return OP_ADD;
	if (op == "-") return OP_SUB;
	if (op == "*") return OP_MLT;
	if (op == "/") return OP_DIV;
	if (op == "%") return OP_REM;

	if (op == "!") return OP_NOT;
	if (op == "-") return OP_NEG;

	if (op == "++") return OP_INC;
	if (op == "--") return OP_DEC;

	if (op == "==") return OP_EQ;
	if (op == "!=") return OP_NEQ;
	if (op == ">=") return OP_GEQ;
	if (op == ">") return OP_GT;
	if (op == "<=") return OP_LEQ;
	if (op == "<") return OP_LT;
	
	if (op == "&&") return OP_AND;
	if (op == "||") return OP_OR;

	return OP_HALT;
}

ValueType getValueType(string type)
{
	if (type == "int") return INT;
	if (type == "double") return DOUBLE;
	return UNKNOWN;
}

int getValueTypeOffset(ValueType type)
{
	switch (type)
	{
		case INT: 		return 4;
		case DOUBLE: 	return 8;
		default: 		return 0;
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

bool variableContainedInScope(const string &var, int scope)
{
	for (auto x = scope; scopeParents.find(x) != scopeParents.end(); x = scopeParents[x])
	{
		auto scope_vars = variableSymbolTable[x];
		for (size_t i = 0; i < scope_vars.size(); ++i)
		{
			if (scope_vars[i]->getName() == var)
			{
				return true;
			}
		}
	}

	return false;
}

int getVariableOffset(const string &var, int scope)
{
	int offset = 0;
	for (auto x = scope; x != 0; x = scopeParents[x])
	{
		int currScopeOffset = 0;
		auto scope_vars = variableSymbolTable[x];
		for (size_t i = 0; i < scope_vars.size(); ++i)
		{
			if (scope_vars[i]->getName() == var)
			{
				return offset + currScopeOffset;
			}
			currScopeOffset += getValueTypeOffset(scope_vars[i]->getValueType());
		}
		offset -= currScopeOffset;
	}
	return -1;
}

void addScope(int scope, int scopeParent = -1)
{
	variableSymbolTable[scope]; 
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

void addToScopeSize(int scope, int size)
{
	if (scopeVariableSize.find(scope) == scopeVariableSize.end())
	{
		scopeVariableSize[scope] = 0;
	}

	scopeVariableSize[scope] += size;
}

void addScopeVariable(int scope, const string &var, VariableDeclarationNode *data)
{
	addToScopeSize(scope, getValueTypeOffset(data->getValueType()));
	variableSymbolTable[scope].push_back(data);
}

void clearScope(int scope)
{
	variableSymbolTable.erase(scope);
}

bool doesFunctionExist(const string &name)
{
	return functionSymbolTable.find(name) != functionSymbolTable.end();
}

void addFunction(const string &name, FunctionDeclarationNode *data)
{
	if (functionSymbolTable.find(name) != functionSymbolTable.end())
	{
		if (functionSymbolTable[name]->hasFunctionBody() || !functionSymbolTable[name]->hasSameArguments(data))
		{
			throw runtime_error("cannot redefine function");
		}
		auto funcNode = functionSymbolTable[name];
		nodeToScopeMap.erase((ASTNode*)funcNode);
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
	return;
}

void initScope(int scope, const vector<ArgumentData*> &arguments)
{
	vector<VariableDeclarationNode*> argumentVariables;
	for (auto x : arguments)
	{
		if (variableContainedInScope(x->name, scope))
		{
			throw runtime_error("cannot redefine variable " + x->name);
		}
		argumentVariables.push_back(new VariableDeclarationNode(x->name, getValueType(x->type)));	
	}
	variableSymbolTable[scope] = argumentVariables;
}

bool checkFunctionCall(FunctionInvocationNode *invocation)
{
	return functionSymbolTable.find(invocation->getName()) != functionSymbolTable.end() && functionSymbolTable[invocation->getName()]->getArgCount() == invocation->getArgCount();
}

int getArgumentOffset(FunctionInvocationNode *node, int index)
{
	int size = 0;
	auto funcDecl = functionSymbolTable[node->getName()];
	auto scope = nodeToScopeMap[funcDecl];
	for (int i = 0; i < index; ++i)
	{
		size += getValueTypeOffset(variableSymbolTable[scope][i]->getValueType());
	}
	
	return size;
}

int getFunctionArgumentSize(FunctionInvocationNode *node)
{
	int size = 0;
	auto funcDecl = functionSymbolTable[node->getName()];
	auto scope = nodeToScopeMap[funcDecl];
	for (auto vd : variableSymbolTable[scope])
	{
		size += getValueTypeOffset(vd->getValueType());
	}

	return size;
}

void printScopes(int scopeCount)
{
	for (int s = 0; s < scopeCount; ++s)
	{
		cout << "SCOPE " << s << endl;
		cout << "PARENT " << scopeParents[s] << endl;
		cout << "VARS\n";
		for (auto v : variableSymbolTable[s])
		{
			cout << v->getName() << endl;
		}
		cout << endl;
	}
}
#endif // SYMBOL_TABLES_H_
