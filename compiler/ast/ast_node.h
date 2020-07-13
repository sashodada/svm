#ifndef AST_NODE_H_
#define AST_NODE_H_

#include "../../common/enums.h"

enum ValueType
{
	UNKNOWN = 0,
	INT = 1,
	DOUBLE = 2,
};

class Visitor;

class ASTNode
{
public:
	ASTNode() {}
	virtual ~ASTNode() {}

	virtual void accept(Visitor *v);

	virtual void compile() {}
};

#endif // AST_NODE_H_
