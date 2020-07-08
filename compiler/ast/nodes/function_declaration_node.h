#ifndef FUNCTION_DECLARATION_NODE_H_
#define FUNCTION_DECLARATION_NODE_H_

#include <string>
#include <unordered_map>
#include "../ast_node.h"
using namespace std;

class FunctionDeclarationNode : public ASTNode
{
private:
	unordered_map<string, string> arguments;
	ASTNode *body;

public:
	FunctionDeclarationNode(unordered_map<string, string> &args, ASTNode *_body) : arguments(args), body(_body) {}
	virtual ~FunctionDeclarationNode()
	{
		delete body;
	}
};
	

#endif // FUNCTION_DECLARATION_NODE_H_
