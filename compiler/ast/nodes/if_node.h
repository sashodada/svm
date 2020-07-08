#ifndef IF_NODE_H_
#define IF_NODE_H_

#include "../ast_node.h"

class IfNode : public ASTNode
{
private:
	ASTNode *condition;
	ASTNode *body;
	ASTNode *elseBody;
public:
	IfNode(ASTNode *_condition, ASTNode *_body, ASTNode *_elseBody = nullptr) : condition(condition), body(_body), elseBody(_elseBody) {}

	virtual ~IfNode()
	{
		delete condition;
		delete body;
		if (elseBody) delete elseBody;
	}
}

#endif //IF_NODE_H_
