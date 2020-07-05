CXX=g++

LEX=lex
PARSE=bison

LEX_FILE=lexer/scanner.l
LEX_TARGET=lexer/lex.yy.c

GRAMMAR_FILE=parser/language.y
PARSE_TARGET=parser/y.tab.c
PARSE_HEADER=parser/y.tab.h
PARSE_OUTPUT=parser/y.output

CFLAGS=-I.

main:	main.cpp $(LEX_TARGET)
	$(CXX) main.cpp $(LEX_TARGET) -o main.out

parse:	$(GRAMMAR_FILE)
	$(PARSE) -v -d -o $(PARSE_TARGET) $(GRAMMAR_FILE) 

lex:	$(LEX_FILE) $(PARSE_TARGET)
	$(LEX) -o $(LEX_TARGET) lexer/scanner.l 

test:	$(LEX_FILE) $(GRAMMAR_FILE)
	make clean && make parse && make lex && $(CXX) $(PARSE_TARGET) $(LEX_TARGET) -ly; cat testfile.cpp | ./a.out

clean:
	rm -f *.out $(LEX_TARGET) $(PARSE_TARGET) $(PARSE_HEADER) $(PARSE_OUTPUT)
