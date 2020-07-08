#ifndef BOOLEAN_NEGATION_NODE_H_
#define BOOLEAN_NEGATION_NODE_H_

#include "../ast_node.h"

class BooleanNegationNode : public ASTNode
{
private:	
	ASTNode *expression;
public:
	BooleanNegationNode(ASTNode *exp) : expression(exp) {}
	virtual ~BooleanNegationNode()
	{
		delete expression;
	}
};

#endif // BOOLEAN_NEGATION_NODE_H_

