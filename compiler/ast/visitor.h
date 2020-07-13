#ifndef AST_VISITOR_H_
#define AST_VISITOR_H_

#include <iostream>
#include "node_factory.h"
using namespace std;

class Visitor
{

public:
	Visitor() {}
	virtual ~Visitor() {}

	virtual void visit(AssignmentNode *node) {} 
	virtual void visit(BinaryExpressionNode *node) {} 
	virtual void visit(BlockStatementNode *node) {} 
	virtual void visit(BooleanNegationNode *node) {} 
	virtual void visit(BreakNode *node) {} 
	virtual void visit(FunctionDeclarationNode *node) {} 
	virtual void visit(FunctionInvocationNode *node) {} 
	virtual void visit(IfNode *node) {} 
	virtual void visit(IntLiteralNode *node) {} 
	virtual void visit(DoubleLiteralNode *node) {} 
	virtual void visit(ForLoopNode *node) {} 
	virtual void visit(WhileLoopNode *node) {} 
	virtual void visit(NegationNode *node) {}
	virtual void visit(PostfixUnaryExpressionNode *node) {} 
	virtual void visit(PrefixUnaryExpressionNode *node) {} 
	virtual void visit(ProgramNode *node) {} 
	virtual void visit(ReturnNode *node) {} 
	virtual void visit(StatementNode *node) {} 
	virtual void visit(VariableDeclarationNode *node) {} 
	virtual void visit(VariableReferenceNode *node) {} 
};

void ASTNode::accept(Visitor *v)		{ this->accept(v); cout << "accept\n"; }
void AssignmentNode::accept(Visitor *v) { v->visit(this); }
void BinaryExpressionNode::accept(Visitor *v) { v->visit(this); }
void BlockStatementNode::accept(Visitor *v) { v->visit(this); }
void BooleanNegationNode::accept(Visitor *v) { v->visit(this); }
void BreakNode::accept(Visitor *v) { v->visit(this); }
void FunctionDeclarationNode::accept(Visitor *v) { v->visit(this); }
void FunctionInvocationNode::accept(Visitor *v) { v->visit(this); }
void IfNode::accept(Visitor *v) { v->visit(this); }
void IntLiteralNode::accept(Visitor *v) { v->visit(this); }
void DoubleLiteralNode::accept(Visitor *v) { v->visit(this); }
void ForLoopNode::accept(Visitor *v) { v->visit(this); }
void WhileLoopNode::accept(Visitor *v) { v->visit(this); }
void NegationNode::accept(Visitor *v) { v->visit(this); }
void PostfixUnaryExpressionNode::accept(Visitor *v) { v->visit(this); }
void PrefixUnaryExpressionNode::accept(Visitor *v) { v->visit(this); }
void ProgramNode::accept(Visitor *v) { v->visit(this); }
void ReturnNode::accept(Visitor *v) { v->visit(this); }
void StatementNode::accept(Visitor *v) { v->visit(this); }
void VariableDeclarationNode::accept(Visitor *v) { v->visit(this); }
void VariableReferenceNode::accept(Visitor *v) { v->visit(this); }
// void ::accept(Visitor *v) { v->visit(this); }

#endif // AST_VISITOR_H_
