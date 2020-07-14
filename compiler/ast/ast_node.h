#ifndef AST_NODE_H_
#define AST_NODE_H_

#include "../../common/enums.h"

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
