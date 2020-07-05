/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_Y_TAB_H_INCLUDED
# define YY_YY_PARSER_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IF_TOKEN = 258,
    ELSE_TOKEN = 259,
    FOR_TOKEN = 260,
    WHILE_TOKEN = 261,
    BREAK_TOKEN = 262,
    RETURN_TOKEN = 263,
    GEQ = 264,
    LEQ = 265,
    NEQ = 266,
    EQ = 267,
    AND = 268,
    OR = 269,
    PLUS_EQ = 270,
    MIN_EQ = 271,
    MULT_EQ = 272,
    DIV_EQ = 273,
    REM_EQ = 274,
    INCREMENT = 275,
    DECREMENT = 276,
    IDENTIFIER_TOKEN = 277,
    INTEGER_TOKEN = 278,
    FLOAT_TOKEN = 279,
    UNKNOWN_TOKEN = 280,
    NO_TOKEN = 281
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 9 "parser/language.y" /* yacc.c:1909  */

	int i;
	double d;
	bool b;
	char *t;

#line 88 "parser/y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_Y_TAB_H_INCLUDED  */
