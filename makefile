all:
	bison -d -v yacc.y 
	flex compiler.lex
	gcc  compiler.c myerror.c lex.yy.c yacc.tab.c symbol.c -o pal

	