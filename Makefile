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
SOURCE = myerror.c symbol.c compiler.c

all: clean pal

man:
	groff -man -T ascii doc/pal.1

pal: ${GRAMMAR} ${LEXER} ${SOURCE}
	flex -i ${LEXER}
	bison -d -v ${GRAMMAR}
	$(CC) $(FLAGS) ${GRAMMAR_C} ${LEXER_C} ${SOURCE} -o pal

clean:
	rm -f pal
	rm -f lex.yy.c yacc.tab.c yacc.tab.h
	rm -rf *~

