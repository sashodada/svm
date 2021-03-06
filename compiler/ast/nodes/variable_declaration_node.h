#ifndef VARIABLE_DECLARATION_NODE_H_
#define VARIABLE_DECLARATION_NODE_H_

#include <string>
#include "../ast_node.h"
using namespace std;

class VariableDeclarationNode : public ASTNode
{
private:
	string name;
	ValueType type;
public:
	VariableDeclarationNode(const string &_name, ValueType _type) : name(_name), type(_type) {}
	virtual ~VariableDeclarationNode() {}

	ValueType getValueType() { return type; }
	string getName() { return name; }
	virtual void accept(Visitor *v);
};

#endif // VARIABLE_DECLARATION_NODE_H_
