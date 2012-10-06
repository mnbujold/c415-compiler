/**
* Lex file for parsing input into tokens for
* PAL language
* made by Daniel Chui
* Mike was here
**/
%{
#include <stdio.h>
#include <string.h>
#include "compiler.tab.h"
#include "myerror.h"
extern char errortext[4096];
extern int last_column;
extern int lineno;
extern int oldlineno;
extern myerror *eList;
extern int prog_listing;
extern FILE *listing_file;

void updateError(void) {

    if(lineno != oldlineno) {
        updateErrorText(eList, errortext);
        showAllErrors(eList);
        writeAllErrors(eList,listing_file);
    	eList = deleteAllErrors(eList);
	    oldlineno = lineno;
		memset(errortext, '\0', 4096);
	} 
} 

%}
%%

%{
/**
* Do the adding here, instead of putting it in every line
**/
add();
if (yytext != NULL) {
  strcat (errortext, yytext);
}


%}
 /* comments */
"//"[^\n]*""		{ /* do nothing, one line comment */}
"{"[^}]*"}"		{ lineno += countlines(yytext); 
                          strcat (errortext, yytext);
                          /* do nothing, a block comment */ }

    /* reserved keywords in PAL */
"and"						{ return AND;}
"array"						{ return ARRAY;}
"begin"						{ return BEGIN_; /* BEGIN causes compilation errors */}
"const"						{ return CONST;}
"continue"					{ /* note: not in PASCAL */ return CONTINUE;}
"div"						{ return DIV;}
"do"						{ return DO;}
"else"						{ return ELSE;}
"end"						{ return END;}
"exit"						{ /* note: not in PASCAL */ return EXIT;}
"function"					{ return FUNCTION;}
"if"						{ return IF;}
"mod"						{ return MOD;}
"not"						{ return NOT;}
"of"						{ return OF;}
"or"						{ return OR;}
"procedure"					{ return PROCEDURE;}
"program"					{ return PROGRAM;}
"record"						{ return RECORD;}
"then"						{ return THEN;}
"type"						{ return TYPE;}
"var"						{ return VAR;}
"while"						{ return WHILE;}

 /* Numbers and Vars */
[ \t]                                           { strcat (errortext, yytext); 
                                                  last_column += strlen (yytext); 
                                                  /* ignore whitespace */ }
[a-zA-Z][a-zA-Z0-9]*				{ return ID;}
[0-9]+						{ return INT_CONST; }
[0-9]+.[0-9]+					{ return REAL_CONST; } 
   /*cheating: scan for decimal reals */
[0-9]+.[0-9]+E[+|-]?[0-9]+			{ return REAL_CONST; }
[0-9]+E[+|-]?[0-9]+				{ return REAL_CONST; } /*for exponents */
'[^']*'						{ return STRING; }
\n                      			{ lineno++;
						  if(prog_listing) {
                                                    fprintf(listing_file, "%s \n", errortext);
                                                    printf("{%d} %s\n",lineno, errortext);
						  } /* if */ 
                                                  last_column=1; 
                                                  updateError(); 
						}
                                              
    /* relational operators in PAL */
"="						{ return ISEQUAL;}
"<>"						{ return NOTEQUAL;}
"<"						{ return LESSTHAN;}
">"						{ return GREATERTHAN;}
"<="						{ return LESSTHANEQUALS;}
">="						{ return GREATERTHANEQUALS;}

    /* arithmetic operators in PAL */
"+"						{ return PLUS;}
"-"						{ return MINUS;}
"*"						{ return MULTIPLY;}
"/"						{ return DIVIDE;}
    /* div and mod are under reserved words */

    /* other lexical characters */
":="						{ return ASSIGN;}
"("						{ return LEFTPAREN;}
")"						{ return RIGHTPAREN;}
"."						{ return PERIOD;}
";"						{ return SEMICOLON;}
":"						{ return COLON;}
"["						{ return LEFTBRACKET;}
"]"						{ return RIGHTBRACKET;}
","						{ return COMMA;}
".."						{ return DOUBLEPERIOD;}

    /* built ins  NO LONGER DEFINED*/

    /* other */

[&!@#$%^{}"]                      		{ /*printf("\n\nwhat:>%s\n\n", yytext); */ 
                                                  return INVALIDTOKEN; /*invalid character */ 
                                                }
.                                               { return INVALIDTOKEN; }						

%%
void add() {
last_column += yyleng;
}

/**
*Count the number of lines in a block comment
**/

int countlines (char * comment) {
  int count = 0;
  char *charptr = comment;
  while (*charptr != '}') {
    if (*charptr == '\n') {
      count++;
    }
    charptr++;      
  }
  return count;

}
