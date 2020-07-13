#ifndef BLOCK_STATEMENT_NODE_H_
#define BLOCK_STATEMENT_NODE_H_

#include <iostream>
#include <vector>
#include "../ast_node.h"
using namespace std;

class BlockStatementNode : public ASTNode
{
private:
	vector<ASTNode*> statements;
public:
	BlockStatementNode(vector<ASTNode*> &_statements) : statements(_statements) {}
	virtual ~BlockStatementNode()
	{
		for (auto s : statements) delete s;
	}

	virtual void accept(Visitor *v);
	vector<ASTNode*> getStatements() { return statements; }
};

#endif // BLOCK_STATEMENT_NODE_H_
