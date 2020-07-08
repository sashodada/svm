#ifndef AST_NODE_H_
#define AST_NODE_H_

enum ValueType
{
	EMPTY = 0,
	INT = 1,
	DOUBLE = 2,
	BOOLEAN = 3
};

class ASTNode
{
public:
	ASTNode() {}
	virtual ~ASTNode() {}

	// virtual void parse();
};

#endif // AST_NODE_H_
