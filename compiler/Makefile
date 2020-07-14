CXX=g++

LEX=lex
PARSE=bison

MAIN_EXECUTABLE=main.out

LEX_FILE=lexer/scanner.l
LEX_TARGET=lexer/lex.yy.cpp

GRAMMAR_FILE=parser/language.y
PARSE_TARGET=parser/y.tab.cpp
PARSE_HEADER=parser/y.tab.hpp
PARSE_OUTPUT=parser/y.output

CFLAGS=-I.

compile: a.out

main:	main.cpp $(LEX_TARGET)
	$(CXX) main.cpp $(LEX_TARGET) -o main.out

parse:	$(GRAMMAR_FILE)
	$(PARSE) -v -d -o $(PARSE_TARGET) $(GRAMMAR_FILE) 

lex:	$(LEX_FILE) $(PARSE_TARGET)
	$(LEX) -o $(LEX_TARGET) lexer/scanner.l 

test:	$(LEX_FILE) $(GRAMMAR_FILE)
	make clean && make parse && make lex && make main; cat testfile.cpp | $(MAIN_EXECUTABLE)

clean:
	rm -f *.out $(LEX_TARGET) $(PARSE_TARGET) $(PARSE_HEADER) $(PARSE_OUTPUT)
