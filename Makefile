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

GRAMMAR = src/compiler.y
GRAMMAR_C = compiler.tab.c
LEXER = src/compiler.lex
LEXER_C = lex.yy.c
SOURCE = src/myerror.c src/symbol.c src/compiler.c

# Stops bison from overwriting compiler.c
%.c: %.y
%.c: %.l

all: clean pal

man:
	groff -man -T ascii doc/pal.1

pal: ${GRAMMAR} ${LEXER} ${SOURCE}
	flex -i ${LEXER}
	bison -d -v ${GRAMMAR}
	$(CC) $(FLAGS) ${GRAMMAR_C} ${LEXER_C} ${SOURCE} -o pal

debug: ${GRAMMAR} ${LEXER} ${SOURCE}
	flex -i ${LEXER}
	bison -d -v ${GRAMMAR}
	$(CC) $(FLAGS) ${GRAMMAR_C} ${LEXER_C} ${SOURCE} -D DEBUG=1 -o pal

clean:
	rm -f pal core *.output
	rm -f lex.yy.c *.tab.c *.tab.h 
	find . -name \*~ -type f -delete
	find . -name \*.lst -type f -delete

