#ifndef PROGRAM_NODE_H_
#define PROGRAM_NODE_H_

#include "../ast_node.h"
#include "statement_node.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class ProgramNode : public ASTNode
{
private:
	vector<ASTNode*> statements;
public:
	ProgramNode(const vector<ASTNode*> &_statements) : statements(_statements) {}
	virtual ~ProgramNode()
	{
		for (auto s : statements) delete s;
	}

	vector<ASTNode*> &getStatements() { return statements; }
	virtual void accept(Visitor *v);
};

#endif // PROGRAM_NODE_H_

