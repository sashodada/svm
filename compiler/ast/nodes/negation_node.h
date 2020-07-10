#ifndef NEGATION_NODE_H_
#define NEGATION_NODE_H_

#include "../ast_node.h"

class NegationNode : public ASTNode
{
private:
	ASTNode *expression;
public:
	NegationNode(ASTNode *exp) : expression(exp) {}
	virtual ~NegationNode()
	{
		delete expression;
	}
};

#endif // NEGATION_NODE_H_

