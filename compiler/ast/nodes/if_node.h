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

	ASTNode *getCondition() { return condition; }
	ASTNode *getBody()		{ return body;		}
	ASTNode *getElseBody()	{ return elseBody;	}

	void setElseBody(ASTNode *body) { elseBody = body; }
	virtual void accept(Visitor *v);
};

#endif //IF_NODE_H_
