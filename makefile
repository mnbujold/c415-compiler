all:
	bison -d -v yacc.y 
	flex lex.l
	gcc  myerror.c lex.yy.c yacc.tab.c symbol.c -o lex
