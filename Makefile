compiler: lex.yy.c parser.tab.o main.cc symboltable.cpp
	g++ -g -w -ocompiler parser.tab.o lex.yy.c main.cc symboltable.cpp -std=c++14
parser.tab.o: parser.tab.cc
	g++ -g -w -c parser.tab.cc -std=c++14
parser.tab.cc: parser.yy
	bison parser.yy
lex.yy.c: lexer.flex parser.tab.cc
	flex lexer.flex
tree: 
	 dot -Tpdf tree.dot -otree.pdf
st:
	 dot -Tpdf symboltable.dot -osymboltable.pdf
clean:
	rm -f parser.tab.* lex.yy.c* compiler stack.hh position.hh location.hh tree.dot tree.pdf symboltable.dot symboltable.pdf
	rm -f compiler.dSYM


