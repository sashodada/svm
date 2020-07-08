#ifndef LITERAL_NODES_H_
#define LITERAL_NODES_H_

#include "../ast_node.h"

class IntLiteralNode : public ASTNode
{
private:
	int value;

public:
	IntLiteralNode(int _value) : value(_value) {}
	virtual ~IntLiteralNode() {}

	int getValue() { return value; }
};

class DoubleLiteralNode : public ASTNode
{
private:
	double value;

public:
	DoubleLiteralNode(double _value) : value(_value) {}
	virtual ~DoubleLiteralNode() {}
	
	double getValue();
};

#endif

