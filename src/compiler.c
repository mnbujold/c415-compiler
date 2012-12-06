#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include <signal.h>
#include <stdbool.h>

#include "symbol.h"
#include "builtin.h"

#include "compiler.h"
#include "compiler.tab.h"

#include "myerror.h"

#include "syntaxtree.h"

#include "ascgen.h"



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
int incodegen;


#ifdef DEBUG
# define DEBUG_PRINT(x) do {printf ("DEBUG: "); printf x;} while (0)
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif



bool segv_detected = false;

void setHandler (void* handler) {
    struct sigaction* newAction;
    newAction = malloc (sizeof (struct sigaction));
    newAction-> sa_handler = (handler);
    sigaction (SIGSEGV, newAction, NULL);
    
}

static void my_handler (int signalNum) {
    segv_detected = true;
    if (iserror) {
      printf ("Fatal error encountered! Exiting gracefully\n");
    }
    else if (incodegen) {
      printf ("Fatal exception encountered during code generation\n");
    }
    else {
      printf ("Fatal exception encountered by compiler\n");

    }
    setHandler (SIG_DFL);
    exit (1);
}


/*
 	Initialize all the variables used in the calculator program
	Start the parser
*/
main(int argc,char** argv)
{
  setHandler (my_handler);
  DEBUG_PRINT (("Hello, testing debug: %d\n", 1));
    prog_listing = 1;
    setvbuf(stdout, (char*) _IONBF, 0, 0);
    setvbuf(stderr, (char*) _IONBF, 0, 0);
	sList = NULL;
	eList = NULL;
	iserror = 0;
  incodegen = 0;
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
    
    if(eList != NULL) {
            showAllErrors(eList);
    }
    
    if (getNumWarnings()) {
        printf ("%d warning(s) found.\n", getNumWarnings());
    }
    
    if (iserror) {
        printf ("%d error(s) found.\n", getNumErrors());
        printf("Errors exist. Compilation not successful.\n");
    } else {
        incodegen = 1;
        genASCCode(getSyntaxTree(), "test.asc");
        printf("Compilation successful.\n");
    } 
    
#if DEBUG
   // showAllSymbols();

#endif
    free_symbol_table();
    free(errortext);
    setHandler (SIG_DFL);
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




  


