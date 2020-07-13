#ifndef VARIABLE_REFERENCE_NODE_H_
#define VARIABLE_REFERENCE_NODE_H_

#include <string>
#include "../ast_node.h"
using namespace std;

class VariableReferenceNode : public ASTNode
{
private:
	string variableName;

public:
	VariableReferenceNode(const string &name) : variableName(name) {}
	virtual ~VariableReferenceNode() {}

	string getName() { return variableName; }
	virtual void accept(Visitor *v);
};

#endif // VARIABLE_REFERENCE_NODE_H_

