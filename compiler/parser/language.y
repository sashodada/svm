%{
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include "../ast/node_factory.h"
#include "../ast/symbol_tables.h"

using namespace std;
extern int yylex();
void yyerror(const char *);

ProgramNode *program;
int scopeNumber = 0;
stack<int> scopes;
%}

%union {
	int i;
	double d;
	bool b;
	char *t;
	vector<ASTNode*>*				nodes;
	ASTNode*						node;
	FunctionDeclarationNode*		fds;
	FunctionInvocationNode*			fis;
	vector<ArgumentData*>* 			args;
	ArgumentData*					arg;
	pair<string, string>*			keyval;
	VariableDeclarationNode*		vardecl;
	VariableReferenceNode*			varref;
}

%start start

%token IF_TOKEN ELSE_TOKEN
%token FOR_TOKEN WHILE_TOKEN BREAK_TOKEN
%token RETURN_TOKEN

%token GEQ LEQ NEQ EQ
%token AND OR
%token PLUS_EQ MIN_EQ MULT_EQ DIV_EQ REM_EQ
%token INCREMENT DECREMENT

%token <t> IDENTIFIER_TOKEN
%token <i> INTEGER_TOKEN
%token <d> FLOAT_TOKEN

%token UNKNOWN_TOKEN

%type <t> 		type
%type <t>		identifier
%type <program> start
%type <nodes> 	global_statements
%type <node> 	global_statement
%type <node> 	break_statement
%type <node> 	loop_statement
%type <nodes> 	loop_statements
%type <node>	loop_block
%type <node>	for_statement
%type <node>	while_statement
%type <node>	loop
%type <node>	loop_init
%type <node> 	return_statement
%type <node> 	block_statement
%type <node> 	if_statement
%type <node>	else_statement
%type <node> 	conditional_statement
%type <node>	argument
%type <node>	expression
%type <nodes>	non_empty_argument_list
%type <nodes> 	argument_list
%type <fis>		function_call
%type <node>	assignment
%type <node>	unary_operation
%type <arg>		declaration_constexpr
%type <vardecl>	declaration_expression
%type <node>	unary_lvalue_expression
%type <node> 	lvalue_expression
%type <fds> 	function_declaration
%type <node> 	binary_expr
%type <node>	literal
%type <nodes> 	local_statements
%type <node> 	local_statement
%type <node>	generic_local_statement
%type <fds>		function_decl_statement	
%type <args>	function_args
%type <args>	arguments_decl
%type <args>	default_arg_decl
%type <arg>		argument_decl

%nonassoc NO_TOKEN 
%nonassoc ELSE_TOKEN

%right '=' PLUS_EQ MIN_EQ MULT_EQ DIV_EQ REM_EQ
%left OR
%left AND
%nonassoc '<' '>' GEQ LEQ NEQ EQ
%left INCREMENT DECREMENT
%left '+' '-'
%left '*' '/' '%'
%right '!'

%%

start	: global_statements { program = new ProgramNode(*$1); scopes.pop(); } 
		;

global_statements	: global_statements global_statement	{ $$ = $1; $$->push_back($2);}
					| %empty	{ $$ = new vector<ASTNode*>(); }
					;

local_statements	: local_statements local_statement 	{ $$ = $1; $$->push_back($2); }
				   	| %empty							{ $$ = new vector<ASTNode*>(); addScope(++scopeNumber, scopes.top()); scopes.push(scopeNumber); } 
					;

generic_local_statement	: expression ';'			{ $$ = $1; }
						| conditional_statement		{ $$ = $1; } 
						| loop						{ $$ = $1; }
						| return_statement ';'		{ $$ = $1; }
						;

local_statement : block_statement			{ $$ = $1; }
				| generic_local_statement	{ $$ = $1; }
				;

function_declaration	: function_decl_statement '{' local_statements '}' 	{ 
																				$1->setFunctionBody(new BlockStatementNode(*$3)); 
																				setNodeScope($1, scopeNumber);
																				$$ = $1;
																				scopes.pop();
																			}
						| function_decl_statement ';' { $$ = $1; clearScope(scopeNumber + 1); }
						;

function_decl_statement	: type identifier '(' function_args ')' { $$ = new FunctionDeclarationNode($1, $2, *$4); addFunction($2, $$); } 
						;

identifier	: IDENTIFIER_TOKEN	{ $$ = $1; }
			;

type	: IDENTIFIER_TOKEN	{ $$ = $1; }
		;

function_args	: arguments_decl	{ $$ = $1; initScope(scopeNumber + 1, *$1); }
				| %empty			{ $$ = new vector<ArgumentData*>(); }
				;

arguments_decl	: argument_decl ',' arguments_decl	{ $$ = $3; $$->push_back($1); }
				| argument_decl			{ $$ = new vector<ArgumentData*>(); $$->push_back($1); }
				| default_arg_decl		{ $$ = $1; }
				;

argument_decl	: declaration_expression	{ $$ = new ArgumentData(); $$->name = $1->getName(); $$->type = $1->getValueType(); }
				;

default_arg_decl	: default_arg_decl ',' declaration_constexpr { $$ = $1; $$->push_back($3); } 
					| declaration_constexpr { $$ = new vector<ArgumentData*>(); $$->push_back($1); }	
					; 


expression	: literal			{ $$ = $1; }
			| '(' expression ')' { $$ = $2; }
			| binary_expr		{ $$ = $1; }
			| unary_operation	{ $$ = $1; }
			| function_call		{ if (!checkFunctionCall($1)) { throw runtime_error($1->getName() + " function not declared"); } $$ = $1; }
			| lvalue_expression	{ $$ = $1; }
			;

literal	: INTEGER_TOKEN	{ $$ = new IntLiteralNode($1); }
		| FLOAT_TOKEN	{ $$ = new DoubleLiteralNode($1); }
		;

binary_expr	: expression '+' expression	{ $$ = new BinaryExpressionNode($1, $3, "+"); }
			| expression '-' expression { $$ = new BinaryExpressionNode($1, $3, "-"); }
			| expression '*' expression { $$ = new BinaryExpressionNode($1, $3, "*"); }
			| expression '/' expression { $$ = new BinaryExpressionNode($1, $3, "/"); }
			| expression '%' expression { $$ = new BinaryExpressionNode($1, $3, "%"); }
			| expression AND expression { $$ = new BinaryExpressionNode($1, $3, "&&"); }
			| expression OR expression { $$ = new BinaryExpressionNode($1, $3, "||"); }
			| expression '<' expression { $$ = new BinaryExpressionNode($1, $3, "<"); }
			| expression '>' expression { $$ = new BinaryExpressionNode($1, $3, ">"); }
			| expression GEQ expression { $$ = new BinaryExpressionNode($1, $3, ">="); }
			| expression LEQ expression { $$ = new BinaryExpressionNode($1, $3, "<="); }
			| expression NEQ expression { $$ = new BinaryExpressionNode($1, $3, "!="); }
			| expression EQ expression { $$ = new BinaryExpressionNode($1, $3, "=="); }
			;

global_statement	: function_declaration 	{ $$ = $1; }
					| assignment ';'		{ $$ = $1; }
					;

lvalue_expression	: declaration_expression	{ 
													$$ = $1; 
													if (variableContainedInScope($1->getName(), scopes.top()))
													{
														throw runtime_error("cannot redeclare variable");
													}
												
													addScopeVariable(scopes.top(), $1->getName(), $1); 
												} 
					| identifier				{ $$ = new VariableReferenceNode($1); } 
					| assignment				{ $$ = $1; }
					| unary_lvalue_expression	{ $$ = $1; }
					;

unary_lvalue_expression	: lvalue_expression INCREMENT %prec NO_TOKEN	{ $$ = new PostfixUnaryExpressionNode($1, "++"); } 
						| INCREMENT lvalue_expression	{ $$ = new PrefixUnaryExpressionNode($2, "++"); }
						| lvalue_expression DECREMENT %prec NO_TOKEN	{ $$ = new PostfixUnaryExpressionNode($1, "--"); } 
						| DECREMENT lvalue_expression	{ $$ = new PrefixUnaryExpressionNode($2, "--"); }

	/* Declarations */
declaration_expression	: type identifier	{ $$ = new VariableDeclarationNode($2, $1); }
						;

declaration_constexpr	: declaration_expression '=' literal { $$ = new ArgumentData(); $$->name = $1->getName(); $$->type = $1->getValueType(); $$->defaultValue = $3; } 
						;

unary_operation	: '!' expression	{ $$ = new PrefixUnaryExpressionNode($2, "!"); }
				| '-' expression	{ $$ = new PrefixUnaryExpressionNode($2, "-"); }
				;

assignment	: lvalue_expression '=' expression		{ $$ = new AssignmentNode($1, $3, "="); }
			| lvalue_expression PLUS_EQ expression	{ $$ = new AssignmentNode($1, $3, "+="); }
			| lvalue_expression MIN_EQ expression	{ $$ = new AssignmentNode($1, $3, "-="); }
			| lvalue_expression MULT_EQ expression	{ $$ = new AssignmentNode($1, $3, "*="); }
			| lvalue_expression DIV_EQ expression	{ $$ = new AssignmentNode($1, $3, "/="); }
			| lvalue_expression REM_EQ expression	{ $$ = new AssignmentNode($1, $3, "%="); }
			;

function_call	: identifier '(' argument_list ')'	{ $$ = new FunctionInvocationNode($1, *$3); }
				;

argument_list	: non_empty_argument_list	{ $$ = $1; }
				| %empty	{ $$ = new vector<ASTNode*>(); }
				;

non_empty_argument_list	: argument_list ',' argument	{ $$ = $1; $$->push_back($3); }
						| argument	{ $$ = new vector<ASTNode*>(); $$->push_back($1); }
						;

argument: expression	{ $$ = $1; }
		;

conditional_statement	: if_statement else_statement	{ dynamic_cast<IfNode*>($1)->setElseBody($2); $$ = $1; } 
						| if_statement %prec NO_TOKEN	{ $$ = $1; }
						;

if_statement: IF_TOKEN '(' expression ')' local_statement { $$ = new IfNode($3, $5); }
			;

else_statement	: ELSE_TOKEN local_statement	{ $$ = $2; }
				;

return_statement	: RETURN_TOKEN				{ $$ = new ReturnNode(); }
					| RETURN_TOKEN expression	{ $$ = new ReturnNode($2); }
					;

block_statement	: '{' local_statements '}' { $$ = new BlockStatementNode(*$2); } 
				;

	/* Loops */ 
loop	: for_statement		{ $$ = $1; }
		| while_statement	{ $$ = $1; }
		;

for_statement	: FOR_TOKEN '(' expression ';' expression ';' expression ')' loop_init	{ $$ = new ForLoopNode($3, $5, $7, $9); } 
				;

while_statement	: WHILE_TOKEN '(' expression ')' loop_init	{ $$ = new WhileLoopNode($3, $5); }
				;

loop_init	: generic_local_statement	{ $$ = $1; }
			| break_statement			{ $$ = $1; }
			| loop_block				{ $$ = $1; }
			;

loop_statements	: loop_statements loop_statement	{ $$ = $1; $$->push_back($2); }
				| %empty	{ $$ = new vector<ASTNode*>(); }
				;

loop_block	: '{' loop_statements '}'	{ $$ = new BlockStatementNode(*$2); }
			;

loop_statement	: local_statement	{ $$ = $1; }
				| break_statement	{ $$ = $1; }
				;

break_statement	: BREAK_TOKEN ';'	{ $$ = new BreakNode(); }
				;

%%

void yyerror(const char *msg)
{
	cerr << msg << endl;
}

int main()
{
	scopes.push(0);
	addScope(0);
	yyparse();
	for (auto x : functionsInOrder)
	{
		cout << "function name: ";
		cout << x->getName() << endl;

		cout << "function variables:\n";
		int scope = getNodeScope(x);
		for (auto v : variableSymbolTable[scope])
		{
			cout << v->getName() << endl;
		}
		cout << "\n\n";
	}
	return 0;
}
