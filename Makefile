# Makefile for pal executable
#
# Authors:
#  Mike Bujold
#  Dan Chui
#  James Osgood
#
# To Compile, type:
#     make

CC = gcc
FLAGS = -g

PY = python

GRAMMAR = src/compiler.y
GRAMMAR_C = compiler.tab.c
LEXER = src/compiler.lex
LEXER_C = lex.yy.c
SOURCE = src/myerror.c src/symbol.c src/operators.c src/type.c src/typeerrors.c src/compiler.c src/builtin.c src/syntaxtree.c
GLIB = `pkg-config --cflags --libs glib-2.0`

RUNNER = tests/runner.py
TESTS = 0 1 2 3 4 5 6 7 8 9

CP2DIR = c415g05_cp2
SOLUTION = Makefile README src doc
SUBTESTS = submitted_tests

# Stops bison from overwriting compiler.c
%.c: %.y
%.c: %.l

all: clean pal

man:
	groff -man -T ascii doc/pal.1

pal: ${GRAMMAR} ${LEXER} ${SOURCE}
	flex -i -o src/${LEXER_C} ${LEXER} 
	bison -d -v ${GRAMMAR} -o src/${GRAMMAR_C} 
	$(CC) $(FLAGS) src/${GRAMMAR_C} src/${LEXER_C} ${SOURCE} -o pal $(GLIB)

debug: ${GRAMMAR} ${LEXER} ${SOURCE}
	flex -i -o src/${LEXER_C} ${LEXER} 
	bison -d -v ${GRAMMAR} -o src/${GRAMMAR_C} 
	$(CC) $(FLAGS) src/${GRAMMAR_C} src/${LEXER_C} ${SOURCE} -D DEBUG=1 -o pal $(GLIB)

cp2:
	make pal
	$(PY) $(RUNNER) -sn $(TESTS)
	mkdir $(CP2DIR)
	make clean
	pdflatex -output-directory doc doc/pal.tex
	rm -f doc/pal.log doc/pal.pdf doc/pal.aux
	cp -r $(SOLUTION) $(SUBTESTS) $(CP2DIR)
	tar -cvzf $(CP2DIR).tgz $(CP2DIR)
	rm -rf $(CP2DIR) $(SUBTESTS)
	
clean:
	rm -f pal core *.output
	rm -f src/lex.yy.c src/*.tab.c src/*.tab.h 
	rm -f doc/pal.log doc/pal.pdf doc/pal.aux
	find . -name \*~ -type f -delete
	find . -name \*.lst -type f -delete

