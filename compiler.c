#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "myerror.h"
#include "yacc.tab.h"


symbol 		*sList;  
myerror 	*eList;  
int 		iserror;
int 		lineno, oldlineno;
int 		last_column;
int 		token_location;
char 		errortext[4096];
int 		looperrordetection;
extern FILE *yyin;
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
    if ( argc == 2 )
    {
        yyin = fopen( argv[1], "r" );
	    yyparse();
	}
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

