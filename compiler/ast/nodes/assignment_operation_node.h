#ifndef ASSIGNMENT_NODE_H_
#define ASSIGNMENT_NODE_H_

#include <string>
#include "../ast_node.h"

class AssignmentNode : public ASTNode
{
private:
	ASTNode *variable;
	ASTNode *value;
public:
	AssignmentNode(ASTNode *var, ASTNode *val) : variable(var), value(val) {}
	virtual ~AssignmentNode()
	{
		delete variable;
		delete value;
	}
};

#endif // ASSIGNMENT_NODE_H_
