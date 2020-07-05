# svm
Basic stack-based virtual machine and compiler for a subset of the C++ programming language. Developed for the purposes of the System Programming Seminar course, held at FMI, Sofia University 2019/20.

#Setup

### Prerequisites
	make, lex, yacc, g++

### Usage
```bash
	cd compiler
	make test #uses "make parse" and "make lex" and passes a random test source code to test parsing
