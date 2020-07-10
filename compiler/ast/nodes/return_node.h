#ifndef RETURN_NODE_H_
#define RETURN_NODE_H_

#include "../ast_node.h"

class ReturnNode : public ASTNode
{
private:
	ASTNode *returnValue;

public:
	ReturnNode(ASTNode *returnVal = nullptr) : returnValue(returnVal) {}
	virtual ~ReturnNode()
	{
		delete returnValue;
	}

	ASTNode *getReturnValue() { return returnValue; }
};

#endif // RETURN_NODE_H_

