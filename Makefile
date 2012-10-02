# Makefile for pal executable
#
# Authors:
#  Mike Bujold
#  Dan Chui
#  James Osgood
#  Paul Vandermeer
#
# To Compile, type:
#     make

CC = gcc
FLAGS = -g

GRAMMAR = yacc.y
GRAMMAR_C = yacc.tab.c
LEXER = compiler.lex
LEXER_C = lex.yy.c
SOURCE = myerror.c symbol.c

all: clean pal

pal:
	bison -d -v yacc.y 
	flex compiler.lex
	gcc  myerror.c lex.yy.c yacc.tab.c symbol.c -o lex

clean:
	rm -f pal
	rm -f lex.yy.c yacc.tab.c yacc.tab.h

