#ifndef STATEMENT_NODE_H_
#define STATEMENT_NODE_H_

#include "../ast_node.h"
#include <string>

class StatementNode : public ASTNode
{
public:
	StatementNode() {}
	virtual ~StatementNode() {}
	virtual void accept(Visitor *v);
};

#endif // STATEMENT_NODE_H_
