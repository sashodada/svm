#ifndef FUNCTION_INVOCATION_NODE_H_
#define FUNCTION_INVOCATION_NODE_H_

#include <vector>
#include <string>
#include "../ast_node.h"
using namespace std;

class FunctionInvocationNode : public ASTNode
{
private:
	vector<ASTNode*> arguments;
	string functionName;

public:
	FunctionInvocationNode(const string &name, const vector<ASTNode*> &args) : functionName(name), arguments(args) {}
	virtual ~FunctionInvocationNode()
	{
		for (auto a : arguments) delete a;
	}

	string getName() { return functionName; }
	vector<ASTNode*> getArguments() { return arguments; }
	size_t getArgCount() { return arguments.size(); }
	virtual void accept(Visitor *v);
};

#endif // FUNCTION_INVOCATION_NODE_H_
