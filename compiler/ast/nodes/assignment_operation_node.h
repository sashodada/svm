#ifndef ASSIGNMENT_NODE_H_
#define ASSIGNMENT_NODE_H_

#include <string>
#include "../ast_node.h"
using namespace std;

class AssignmentNode : public ASTNode
{
private:
	ASTNode *variable;
	ASTNode *value;
	string operatorSign;
public:
	AssignmentNode(ASTNode *var, ASTNode *val, const string &op) : variable(var), value(val), operatorSign(op) {}
	virtual ~AssignmentNode()
	{
		delete variable;
		delete value;
	}

	virtual void accept(Visitor *v);

	string getOperator()	{ return operatorSign; }
	ASTNode *getVariable() { return variable; }
	ASTNode *getValue()		{ return value; }
};

#endif // ASSIGNMENT_NODE_H_
