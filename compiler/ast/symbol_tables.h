#ifndef SYMBOL_TABLES_H_
#define SYMBOL_TABLES_H_

#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

struct VariableData
{
	string declaration_line;
	string type;
};

struct FunctionData
{
	string declaration_line;
	string type;
	vector<string> variables;
	unordered_map<string, string> default_variables;
}
	
unordered_map<string, string> scopeParents;
unordered_map<string, unordered_map<string, VariableData>> variableSymbolTable;
unordered_map<string, FunctionData> functionSymbolTable;

bool variableContainedInScope(string &var, string &scope)
{
	auto x = scope;
	for (auto x = scope; scopeParents.find(x) != scopeParents.end(); x = *(scopeParents.find(x)))
	{
		auto scope_vars = *(variableSymbolTable.find(x));
		if (scope_vars.find(var) != scope_vars.end()) return true;
	}
}

void addScope(string &scopeName, string &scopeParent = "")
{
	variableSymboltable[scopeName] = unorderedMap<string, VariableData>();
	if (scopeParent != "")
	{
		scopeParents[scope] = scopeParent;
	}
}

void addScopeVariable(string &scope, string &var, VariableData &data)
{
	variableSymbolTable[scope][var] = data;
}

bool doesFunctionExist(string &name)
{
	return functionSymbolTable.find(name) != functionSymbolTable.end();
}

void addFunction(string &name, FunctionData &data)
{
	functionSymboltable[name] = data;
}

#endif // SYMBOL_TABLES_H_
