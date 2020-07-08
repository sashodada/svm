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
	vector<StatementNode*> statements;
public:
	ProgramNode(vector<StatementNode*> &_statements) : statements(_statements) {}
	virtual ~ProgramNode()
	{
		for (auto s : statements) delete s;
	}

	void print() { cout << statements.size() << endl; }
};

#endif // PROGRAM_NODE_H_
