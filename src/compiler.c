#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "symbol.h"
#include "builtin.h"

#include "compiler.h"
#include "compiler.tab.h"

#include "myerror.h"



symbol 		*sList;  
myerror 	*eList;  
int 		iserror;
int 		lineno, oldlineno;
int 		last_column;
int 		token_location;
char *errortext;
extern char illegalChar;
int 		looperrordetection;
extern 		FILE *yyin;
int errorTextLength;
int prog_listing;
FILE *listing_file;
char listing_filename[1024];
extern int numErrors;


#ifdef DEBUG
# define DEBUG_PRINT(x) do {printf ("DEBUG: "); printf x;} while (0)
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

/*
 	Initialize all the variables used in the calculator program
	Start the parser
*/
main(int argc,char** argv)
{
  DEBUG_PRINT (("Hello, testing debug: %d\n", 1));
    prog_listing = 1;
    setvbuf(stdout, (char*) _IONBF, 0, 0);
    setvbuf(stderr, (char*) _IONBF, 0, 0);
	sList = NULL;
	eList = NULL;
	iserror = 0;
    token_location = 0;
    lineno = 1;
    oldlineno = 0;
    looperrordetection = 0;
	errorTextLength = 0;
	errortext = NULL;
    illegalChar = 0;

    parse_args(argc, argv);
#if DEBUG
    DEBUG_PRINT(("leave_asc: %d \n", leave_asc));
    printf("prog_listing: %d \n", prog_listing);
    printf("bounds_check: %d \n", bounds_check);
    printf("execute: %d \n", execute);
#endif
    
    init_table ();
    //exit (1);
#if DEBUG
    showAllSymbols();
#endif
    //add_builtins();
    while (yyparse() > 0) {
        yylex();

        if(prog_listing){
            close(listing_file);
        }
        //sList = deleteAllSymbols(sList);
        if(eList != NULL) {
            updateErrorText(eList, errortext);
            showAllErrors(eList);
        }
        eList = deleteAllErrors(eList);
    }
    if (iserror) {
        printf ("%d errors found\n", getNumErrors());
        printf("Errors exist. Compilation not successful.\n");
    } else {
        printf("Compilation successful.\n");
    } 
    
#if DEBUG
    showAllSymbols();

#endif
    free_symbol_table();
    free(errortext);
    return 0;
}

void parse_args(int argc, char* argv[]){
  if(argc < 2)
    usage();
  if(argc > 6)
    usage();

  int i = 0;
  int p = 0;
  for(i=1; i<argc; i++){
    if(argv[i][0] == '-')
      for(p=1; p<strlen(argv[i]); p++)
        switch(argv[i][p]){
        case 'S': leave_asc = 1; break;
        case 'n': prog_listing = 0; break;
        case 'a': bounds_check = 0; break;
        case 'c': leave_asc = 1; execute = 0; break;
        }
    else{
      /* Apparrently source_file isn't a descriptive enough name, so we re-name it yyin */
      yyin = fopen(argv[i], "r");
      if(yyin == NULL){
        fprintf(stderr, "could not open %s \n", argv[i]);
        exit(-1);
      }
      if(prog_listing){
        int j=0;
        while(argv[i][j] != '.')
          listing_filename[j] = argv[i][j++];
        strcat(listing_filename,".lst");

      }
    }
  }

  if(prog_listing){
    listing_file = fopen(listing_filename, "w");
    if(listing_file == NULL){
      fprintf(stderr, "Could not open %s \n", listing_filename);
      exit(-1);
    }
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

  


