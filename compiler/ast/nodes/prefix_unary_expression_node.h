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
	PrefixUnaryExpressionNode(ASTNode *l, string &op) : lvalue(l), opeartion(op) {}
	virtual ~PrefixUnaryExpressionNode()
	{
		delete lvalue;
	}
};

#endif // PREFIX_UNARY_EXPRESSION_NODE_H_