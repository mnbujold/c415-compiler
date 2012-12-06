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
SOURCE = src/myerror.c src/symbol.c src/operators.c src/type.c src/typeerrors.c src/compiler.c src/builtin.c src/syntaxtree.c src/transformations.c src/ascgen.c
GLIB = `pkg-config --cflags --libs glib-2.0`

RUNNER = tests/runner.py
TESTS = 0 1

CP3DIR = c415g05_cp3
SOLUTION = Makefile README src doc
SUBTESTS = submitted_tests

# Stops bison from overwriting compiler.c
%.c: %.y
%.c: %.l

all: clean pal

man:
	groff -man -T ascii doc/pal.1

docs:
	dot -Tps doc/assignment.dot -o doc/assignment.ps
	dot -Tps doc/program.dot -o doc/program.ps
	dot -Tps doc/stat.dot -o doc/stat.ps
	epstopdf doc/assignment.ps
	epstopdf doc/program.ps
	epstopdf doc/stat.ps
	pdflatex -output-directory doc doc/pal.tex

pal: ${GRAMMAR} ${LEXER} ${SOURCE}
	flex -i -o src/${LEXER_C} ${LEXER} 
	bison -d -v ${GRAMMAR} -o src/${GRAMMAR_C} 
	$(CC) $(FLAGS) src/${GRAMMAR_C} src/${LEXER_C} ${SOURCE} -o pal $(GLIB)

debug: ${GRAMMAR} ${LEXER} ${SOURCE}
	flex -i -o src/${LEXER_C} ${LEXER} 
	bison -d -v ${GRAMMAR} -o src/${GRAMMAR_C} 
	$(CC) $(FLAGS) src/${GRAMMAR_C} src/${LEXER_C} ${SOURCE} -D DEBUG=1 -o pal $(GLIB)

cp3:
	make pal
	$(PY) $(RUNNER) -sn $(TESTS)
	mkdir $(CP3DIR)
	make clean
	make docs
	rm -f doc/pal.log doc/pal.pdf doc/pal.aux
	cp -r $(SOLUTION) $(SUBTESTS) $(CP3DIR)
	tar -cvzf $(CP3DIR).tgz $(CP3DIR)
	rm -rf $(CP3DIR) $(SUBTESTS)
	
clean:
	rm -f pal core *.output
	rm -f src/lex.yy.c src/*.tab.c src/*.tab.h 
	rm -f doc/pal.log doc/pal.pdf doc/pal.aux
	find . -name \*~ -type f -delete
	find . -name \*.lst -type f -delete

