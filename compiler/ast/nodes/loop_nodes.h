#ifndef LOOP_NODES_H_
#define LOOP_NODES_H_

#include "../ast_node.h"

class ForLoopNode : public ASTNode
{
private:
	ASTNode *initialization;
	ASTNode *condition;
	ASTNode *iteration;
	ASTNode *body;

public:
	ForLoopNode(ASTNode *init, ASTNode *cond, ASTNode *it, ASTNode *_body) : initialization(init), condition(cond), iteration(it), body(_body) {}
	virtual ~ForLoopNode()
	{
		delete initialization;
		delete condition;
		delete iteration;
		delete body;
	}
	virtual void accept(Visitor *v);

	ASTNode *getInitialization() { return initialization; }
	ASTNode *getCondition() { return condition; }
	ASTNode *getIteration() { return iteration; }
	ASTNode *getBody() { return body; }
};

class WhileLoopNode : public ASTNode
{
private:
	ASTNode *condition;
	ASTNode *body;
public:
	WhileLoopNode(ASTNode *cond, ASTNode *_body): condition(cond), body(_body) {}
	virtual ~WhileLoopNode()
	{
 		delete condition;
		delete body;
	}
	virtual void accept(Visitor *v);
	ASTNode *getCondition() { return condition; }
	ASTNode *getBody() { return body; }
};

#endif //LOOP_NODES_H_
