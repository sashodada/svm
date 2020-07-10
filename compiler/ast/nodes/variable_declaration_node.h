#ifndef VARIABLE_DECLARATION_NODE_H_
#define VARIABLE_DECLARATION_NODE_H_

#include <string>
#include "../ast_node.h"
using namespace std;

class VariableDeclarationNode : public ASTNode
{
private:
	string name;
	string type;
public:
	VariableDeclarationNode(const string &_name, const string &_type) : name(_name), type(_type) {}
	virtual ~VariableDeclarationNode() {}

	string getValueType() { return type; }
	string getName() { return name; }
};

#endif // VARIABLE_DECLARATION_NODE_H_
