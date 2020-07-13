%{
#include <vector>
#include "../ast/node_factory.h"
#include "../parser/y.tab.h"
#include <stdlib.h>
using namespace std;
%}


%%
	/* keywords */
"if"							return IF_TOKEN;
"else"							return ELSE_TOKEN;
"for"							return FOR_TOKEN;
"while"							return WHILE_TOKEN;
"break"							return BREAK_TOKEN;
"return"						return RETURN_TOKEN;

	/* identifiers and literals */
[a-zA-Z_][a-zA-Z0-9_]*			{ yylval.t = new char[strlen(yytext) + 1]; strcpy(yylval.t, yytext); return(IDENTIFIER_TOKEN); }
[+-]?([1-9][0-9]*|0)			{ yylval.i = atoi(yytext); return(INTEGER_TOKEN); }
[+-]?([1-9][0-9]*|0)\.[0-9]*	{ yylval.d = atof(yytext); return(FLOAT_TOKEN);	}

	/* multichar operators */
"<="							return(LEQ);
">="							return(GEQ);
"!="							return(NEQ);
"=="							return(EQ); 

"&&"							return(AND);
"||"							return(OR);

"+="							return(PLUS_EQ);
"*="							return(MULT_EQ);
"/="							return(DIV_EQ);
"%="							return(REM_EQ);
"-="							return(MIN_EQ);

"++"							return(INCREMENT);
"--"							return(DECREMENT);

	/* operators */
[\+\-\=\>\<\!\*\/\%]		return(yytext[0]);

	/* braces */
[\(\)\{\}\[\]]					return(yytext[0]);

	/* punctuation */
[\;\,]							return(yytext[0]);

	/* comments */
\/\*.*\*\/|\/\/.*\n				;

	/* other */
[ \t\r\n]						;
.								{ yylval.t = yytext; return(UNKNOWN_TOKEN); }

%%

int yywrap(void)
{
	return 1;
}
