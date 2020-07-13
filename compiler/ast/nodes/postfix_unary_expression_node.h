#ifndef POSTFIX_UNARY_EXPRESSION_NODE_H_
#define POSTFIX_UNARY_EXPRESSION_NODE_H_

#include <string>
#include "../ast_node.h"
using namespace std;

class PostfixUnaryExpressionNode : public ASTNode
{
private:
	ASTNode *lvalue;
	string operation;

public:
	PostfixUnaryExpressionNode(ASTNode *l, const string &op) : lvalue(l), operation(op) {}
	virtual ~PostfixUnaryExpressionNode()
	{
		delete lvalue;
	}
	virtual void accept(Visitor *v);

	ASTNode *getVariable() 	{ return lvalue; }
	string getOperation()	{ return operation; }
};

#endif // POSTFIX_UNARY_EXPRESSION_NODE_H_
