#ifndef PREFIX_UNARY_EXPRESSION_NODE_H_
#define PREFIX_UNARY_EXPRESSION_NODE_H_

#include <string>
#include "../ast_node.h"
using namespace std;

class PrefixUnaryExpressionNode : public ASTNode
{
private:
	ASTNode *lvalue;
	string operation;

public:
	PrefixUnaryExpressionNode(ASTNode *l, const string &op) : lvalue(l), operation(op) {}
	virtual ~PrefixUnaryExpressionNode()
	{
		delete lvalue;
	}
	virtual void accept(Visitor *v);
	ASTNode *getVariable() { return lvalue; }
	string getOpreation() { return operation; }
};

#endif // PREFIX_UNARY_EXPRESSION_NODE_H_
