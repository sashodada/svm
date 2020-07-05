#ifndef AST_NODE_H_
#define AST_NODE_H_

class Visitor
{
public:
	Visitor() {}
	virtual ~Visitor() {};
	
	visit(ASTNode *node)
	{
		this->visit(*node);
	}
};

class ASTNode
{
public:
	ASTNode() {}
	virtual ~ASTNode() {}
	
	virtual void visit(Visiton &visitor)
	{
		visitor.visit(this);
	}
};

#endif // AST_NODE_H_
