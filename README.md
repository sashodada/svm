# svm
Basic stack-based virtual machine and compiler for a subset of the C programming language. Developed as a final project for the System Programming Seminar course, held at FMI, Sofia University 2019/20.

### <i>*DISCLAIMER</i>
This project was created for personal educational purposes only.

##Setup

### Prerequisites
	make, lex, yacc, g++

### Usage
```bash
	cd compiler
	make test

	# OR
	# individualy execute each of the steps
	make parse
	make lex
	make main

	# compiler takes code from STDIN, outputs to a fixed file '../output.code'
	cat test1.cpp | ./main.out

	# go to SVM directory
	cd ../svm
	g++ main.cpp
	./a.out
```

### Features
SVM currently supports the `int` data type, `for` and `while` loops, conditional statements (`if`/`else`) and function calls / recursion. There is currently no mechanism to handle IO, so the only way to see a program's result is through executing it and checking the value in the RAX register in the output provided.

### Future improvements
Boilerplate code for future support for `double` data and default arguments is already present and to be taken advantage of in the near future. Other priority future features include:
- support for arrays
- support for pointers
- code cleanup (magic numbers, unused and duplicated code)
- I/O