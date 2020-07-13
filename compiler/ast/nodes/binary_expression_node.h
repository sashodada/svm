#ifndef BINARY_EXPRESSION_NODE_H_
#define BINARY_EXPRESSION_NODE_H_

#include <string>
#include "../ast_node.h"
using namespace std;

class BinaryExpressionNode : public ASTNode
{
private:
	ASTNode *left;
	ASTNode *right;
	string operation;
public:
	BinaryExpressionNode(ASTNode *l, ASTNode *r, const string &op) : left(l), right(r), operation(op) {}
	virtual ~BinaryExpressionNode()
	{
		delete left;
		delete right;
	}

	virtual void accept(Visitor *v);
	ASTNode *getLeft() { return left; }
	ASTNode *getRight() { return right; }
	string getOperation() { return operation; }
};

#endif // BINARY_EXPRESSION_H_
