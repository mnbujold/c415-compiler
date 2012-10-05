#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "myerror.h"
#include "compiler.h"
#include "compiler.tab.h"

#define DEBUG 1

symbol 		*sList;  
myerror 	*eList;  
int 		iserror;
int 		lineno, oldlineno;
int 		last_column;
int 		token_location;
char 		errortext[4096];
int 		looperrordetection;


/*
 	Initialize all the variables used in the calculator program
	Start the parser
*/
main(int argc,char** argv)
{
	sList = NULL;
	eList = NULL;
	iserror = 0;
    token_location = 0;
    lineno = 1;
    oldlineno = 1;
    looperrordetection = 0;
    memset(errortext, '\0', 4096);

    parse_args(argc, argv);
    
    yyparse();
    sList = deleteAllSymbols(sList);
    if(eList != NULL) {
		printf("Major errors encountered in input file.  Most likely due to messed up '(' or ')'\n");
		printf("Below is a screen dump of errors caught before the crash:\n");
		updateErrorText(eList, errortext);
		showAllErrors(eList);
	} //if
    eList = deleteAllErrors(eList);
	return 0;
}

void parse_args(int argc, char* argv[]){
  if(argc < 2)
    usage();
  if(argc > 6)
    usage();

  source_file = fopen(argv[argc-1], "r");
  if(source_file == NULL){
    fprintf(stderr, "could not open %s \n", argv[argc-1]);
    exit(-1);
  }
  
}
void usage(void){
  printf("pal [options] [filename].asc\n");
  printf("Options: \n");
  printf("-S Leave ASC code in [filename].asc instead of removing it\n");
  printf("-n Do not produce a program listing. Default is to produce one.\n");
  printf("-a Do not generate run-time array subscript bounds checking. Default is to do the checking. \n");
  printf("-c Compiles program into ASC code, but does not execute it.\n");
  exit(-1);
}

  


