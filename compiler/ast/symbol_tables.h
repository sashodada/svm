#ifndef SYMBOL_TABLES_H_
#define SYMBOL_TABLES_H_

#include <iostream>
#include "node_factory.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

unordered_map<int, int> scopeParents;
unordered_map<int, vector<VariableDeclarationNode*>> variableSymbolTable;
unordered_map<string, FunctionDeclarationNode*> functionSymbolTable;
unordered_map<ASTNode*, int> nodeToScopeMap;
vector<FunctionDeclarationNode*> functionsInOrder;

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

void addScopeVariable(int scope, const string &var, VariableDeclarationNode *data)
{
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
		delete funcNode;
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
		argumentVariables.push_back(new VariableDeclarationNode(x->name, x->type));	
	}
	variableSymbolTable[scope] = argumentVariables;
}

bool checkFunctionCall(FunctionInvocationNode *invocation)
{
	return functionSymbolTable.find(invocation->getName()) != functionSymbolTable.end() && functionSymbolTable[invocation->getName()]->getArgCount() == invocation->getArgCount();
}

#endif // SYMBOL_TABLES_H_
