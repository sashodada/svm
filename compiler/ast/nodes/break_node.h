#ifndef BREAK_NODE_H_
#define BREAK_NODE_H_

#include "../ast_node.h"

class BreakNode : public ASTNode
{
public:
	BreakNode() {}
	virtual ~BreakNode() {}
	virtual void accept(Visitor *v);
};

#endif // BREAK_NODE_H_

