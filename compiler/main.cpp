#include <iostream>
#include "lexer/lang_def.h"

extern int yylex();
extern int yylineno;
extern char *yytext;

using namespace std;

int main(int argc, char **argv)
{
	
	int token = yylex();
	while (token)
	{
		cout << token << " " << yytext << endl;
		token = yylex();
	}
	return 0;
}
