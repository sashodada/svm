CXX=g++

LEX=lex
PARSE=bison

TEST_FILE=testfile.cpp

MAIN_OUTPUT=main.out

LEX_FILE=lexer/scanner.l
LEX_TARGET=lexer/lex.yy.cpp

GRAMMAR_FILE=parser/language.y
PARSE_TARGET=parser/y.tab.cpp
PARSE_HEADER=parser/y.tab.hpp
PARSE_OUTPUT=parser/y.output

CFLAGS=-I.

main:	$(PARSE_TARGET) $(LEX_TARGET)
	$(CXX) $(PARSE_TARGET) $(LEX_TARGET) -o $(MAIN_OUTPUT)

parse:	$(GRAMMAR_FILE)
	$(PARSE) -v -d -o $(PARSE_TARGET) $(GRAMMAR_FILE) 

lex:	$(LEX_FILE) $(PARSE_TARGET)
	$(LEX) -o $(LEX_TARGET) lexer/scanner.l 

test:	$(LEX_FILE) $(GRAMMAR_FILE)
	make clean && make parse && make lex && make main; cat $(TEST_FILE) | ./$(MAIN_OUTPUT)

clean:
	rm -f *.out $(LEX_TARGET) $(PARSE_TARGET) $(PARSE_HEADER) $(PARSE_OUTPUT)
