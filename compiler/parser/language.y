%{
#include <iostream>
#include <vector>
#include <string>
#include "../ast/node_factory.h"

using namespace std;
extern int yylex();
void yyerror(const char *);

ProgramNode *program;
int scopeCount = 0;
%}

%union {
	int i;
	double d;
	bool b;
	char *t;
	vector<StatementNode*>* statements;
	StatementNode* 			statement;
	ProgramNode* 			program;
	ASTNode*				node;
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
%token <f> FLOAT_TOKEN

%token UNKNOWN_TOKEN

%type <t> type
%type <t> identifier
%type <program> start;
%type <statements> global_statements;
%type <statement> global_statement;

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

start	: global_statements { program = new ProgramNode(*$1); delete $1; }
		;

global_statements	: global_statements global_statement	{ $$->push_back($2); }
					| %empty	{ $$ = new vector<StatementNode*>(); }
					;

local_statements	: local_statement local_statements { cout << endl; }
				   	| %empty 
					;

generic_local_statement	: expression ';'
						| conditional_statement
						| loop
						| return_statement ';'
						;

local_statement : block_statement
				| generic_local_statement
				;

function_declaration	: function_decl_statement '{' local_statements '}' { cout << "declare function" << endl; }
						| function_decl_statement ';'
						;

function_decl_statement	: type identifier '(' arguments_decl ')'
						;

identifier	: IDENTIFIER_TOKEN	{ cout << "identifier " << $1 << endl; $$ = $1; }
			;

type	: IDENTIFIER_TOKEN	{ cout << "arbitrary type" << $1 << "\n";  $$ = $1; }
		;


arguments_decl	: argument_decl ',' arguments_decl	{ cout << "multiple arguments\n"; }
				| argument_decl				{ cout << "last argument\n"; }
				| default_arg_decl
				| %empty				{ cout << "no arguments\n"; }
				;

argument_decl	: declaration_expression
				;

default_arg_decl	: declaration_constexpr ',' default_arg_decl
					| declaration_constexpr
					; 


expression	: literal
			| '(' expression ')'
			| binary_expr
			| unary_operation
			| assignment
			| function_call { cout << "FUNCTION CALL\n\n"; }
			| lvalue_expression
			;

literal	: INTEGER_TOKEN
		| FLOAT_TOKEN
		;

binary_expr	: expression '+' expression
			| expression '-' expression
			| expression '*' expression
			| expression '/' expression 
			| expression '%' expression
			| expression AND expression
			| expression OR expression
			| expression '<' expression
			| expression '>' expression
			| expression GEQ expression
			| expression LEQ expression
			| expression NEQ expression
			| expression EQ expression
			;

global_statement	: function_declaration 	{ $$ = new StatementNode(); }
					| assignment ';'		{ $$ = new StatementNode(); }
					;

lvalue_expression	: declaration_expression { cout << "lvalue expression\n"; }
					| identifier { cout << "lvalue identifier\n"; }
					;

	/* Declarations */
declaration_expression	: type identifier
						;

declaration_constexpr	: declaration_expression '=' literal {cout << "declaration constexpr\n"; } 
						;

unary_operation	: lvalue_expression INCREMENT  
				| INCREMENT lvalue_expression
				| lvalue_expression DECREMENT 
				| DECREMENT lvalue_expression
				| '!' expression
				| '-' expression
				;

assignment	: lvalue_expression '=' expression
			| lvalue_expression PLUS_EQ expression
			| lvalue_expression MIN_EQ expression
			| lvalue_expression MULT_EQ expression
			| lvalue_expression DIV_EQ expression
			| lvalue_expression REM_EQ expression
			;

function_call	: identifier '(' argument_list ')'
				;

argument_list	: argument ',' argument_list
				| argument
				| %empty
				;

argument: expression
		;

conditional_statement	: if_statement else_statement
						| if_statement %prec NO_TOKEN
						;

if_statement: IF_TOKEN '(' expression ')' local_statement { cout << "IF statement\n\n"; }
			;

else_statement	: ELSE_TOKEN local_statement
				;

return_statement	: RETURN_TOKEN
					| RETURN_TOKEN expression	{ cout << "RETURN\n\n"; }
					;

block_statement	: '{' local_statements '}' { cout << "Block statement\n"; }
				;

	/* Loops */ 
loop	: for_statement
		| while_statement
		;

for_statement	: FOR_TOKEN '(' expression ';' expression ';' expression ')' loop_init 
				;

while_statement	: WHILE_TOKEN '(' expression ')' loop_init 
				;

loop_init	: generic_local_statement
			| break_statement
			| loop_block
			;

loop_statements	: loop_statement loop_statements
				| loop_statement
				;

loop_block	: '{' loop_statements '}'
			;

loop_statement	: local_statement
				| break_statement
				;

break_statement	: BREAK_TOKEN ';'
				;

%%

void yyerror(const char *msg)
{
	cerr << msg << endl;
}

int main()
{
	yyparse();
	program->print();
	return 0;
}
