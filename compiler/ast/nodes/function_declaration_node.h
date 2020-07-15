#ifndef FUNCTION_DECLARATION_NODE_H_
#define FUNCTION_DECLARATION_NODE_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include "../ast_node.h"
#include "block_statement_node.h"
using namespace std;

struct ArgumentData
{
	string name;
	ValueType type;
	ASTNode *defaultValue;
};

class FunctionDeclarationNode : public ASTNode
{
private:
	string type;
	string name;
	vector<ArgumentData*> arguments;
	BlockStatementNode *body;

public:
	FunctionDeclarationNode(const string &_type, const string& _name, const vector<ArgumentData*> &args, BlockStatementNode *_body = nullptr) : type(_type), name(_name), arguments(args), body(_body) { }
	virtual ~FunctionDeclarationNode()
	{
		if (body)
		{
			delete body;
		}

		for (auto a : arguments)
		{
			delete a;
		}
	}

	bool hasFunctionBody() { return body != nullptr; }
	void setFunctionBody(BlockStatementNode *_body) { body = _body; }
	ASTNode *getFunctionBody() { return body; }

	bool hasSameArguments(FunctionDeclarationNode *other)
	{
		for (size_t i = 0; i < arguments.size(); ++i)
		{
			if (arguments[i]->name != other->arguments[i]->name || arguments[i]->type != other->arguments[i]->type)
			{
				return false;
			}
		}
		return true;
	}

	string getType() { return type; }
	string getName() { return name; }
	BlockStatementNode *getBody() { return body; }
	vector<ArgumentData*> getArguments() { return arguments; }
	size_t getArgCount() { return arguments.size(); }
	virtual void accept(Visitor *v);
};
	

#endif // FUNCTION_DECLARATION_NODE_H_
