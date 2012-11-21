/**
* Lex file for parsing input into tokens for
* PAL language
* made by Daniel Chui
* Mike was here
* James was here
**/
%{
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "symbol.h"
#include "compiler.tab.h"
#include "myerror.h"

#ifdef DEBUG
#define DB_PRINT(body) printf(body)
#else
#define DB_PRINT(body)
#endif

extern char *errortext;
extern int last_column;
extern int lineno;
extern int oldlineno;
extern myerror *eList;
extern int errorTextLength;
extern char illegalChar;
extern int prog_listing;
extern FILE *listing_file;

void updateError(void) {
  if(lineno != oldlineno) {
    updateErrorText(eList, errortext);
    showAllErrors(eList);
    /* If prog_listing flag is TRUE, write errors to .lst file */
    if(prog_listing)
      writeAllErrors(eList,listing_file);
    eList = deleteAllErrors(eList);
    oldlineno = lineno;
    memset(errortext, '\0', errorTextLength);
  } 
} 


%}

%option yylineno 

%s IN_COMMENT
%%

%{
/**
* Do the adding here, instead of putting it in every line
**/
add();
if (yytext != NULL) {
  errortext = appendErrorText(errortext, yytext, &errorTextLength);
}


%}
 /* comments, newlines, etc. */
[\n\r]|(\r\n)                      	{ DB_PRINT("CR\n"); 
                                  //lineno++;
                                  if (prog_listing) {
                                     fprintf(listing_file, "%s \n", errortext);
                                     printf("{%d} %s\n",lineno, errortext);
                                  }
                                  lineno++;
                                  last_column=1;
                                  updateError(); 

                                }
[ \t]                           { 
                                  errortext = appendErrorText(errortext, yytext, &errorTextLength);
                                  last_column += strlen (yytext);
                                  /* ignore whitespace */ 
                                  }
"//"[^\n]*""                    { /*printf("SL_COMMENT \n");*/
                                  errortext = appendErrorText(errortext, yytext, &errorTextLength);}

<INITIAL>{
"{"                             { BEGIN(IN_COMMENT); 
                                  errortext = appendErrorText(errortext, yytext, &errorTextLength);
                                  /*printf("ML_START\n"); */
                                }
}
<IN_COMMENT>{
"}"                             { BEGIN(INITIAL); 
                                  errortext = appendErrorText(errortext, yytext, &errorTextLength);
                                  /*printf("ML_END\n");*/ }
[^}\n]+                         { errortext = appendErrorText(errortext, yytext, &errorTextLength); }
[^}]"*"                         { errortext = appendErrorText(errortext, yytext, &errorTextLength); }
\n                              { lineno++; 
                                  errortext = appendErrorText(errortext, yytext, &errorTextLength);
                                  /*printf("ML_COMMENT \n"); */
                                }
}


'(\\.|[^\\'])*'			{ //lineno += strcountlines (yytext);
				  yylval.string = strdup(yytext); 
                                  /*printf ("lala: %s\n", yytext);*/
                                  return STRING; }

 /* reserved keywords in PAL */
"and"                           { return AND;}
"array"                         { return ARRAY;}
"begin"                         { DB_PRINT("BEGIN "); return BEGIN_; /* BEGIN causes compilation errors */}
"const"                         { return CONST;}
"continue"			{ /* note: not in PASCAL */ return CONTINUE;}
"div"				{ return DIV;}
"do"				{ return DO;}
"else"				{ return ELSE;}
"end"			        { return END;}
"exit"				{ /* note: not in PASCAL */ return EXIT;}
"function"			{ return FUNCTION;}
"if"				{ return IF;}
"mod"				{ return MOD;}
"not"				{ return NOT;}
"of"				{ return OF;}
"or"				{ return OR;}
"procedure"			{ return PROCEDURE;}
"program"			{ DB_PRINT("PROGRAM "); return PROGRAM;}
"record"			{ return RECORD;}
"then"				{ return THEN;}
"type"				{ return TYPE;}
"var"				{ return VAR;}
"while"				{ return WHILE;}

writeln|write|read|readln   { yylval.id = strdup(yytext); return IOPROC; }

 /* Numbers and Vars */
[a-zA-Z][a-zA-Z0-9]*		{ yylval.id = strdup(yytext);  
                                  DB_PRINT("ID ");
                                  return ID;}

(([0-9]+"."[0-9]+(E("+"|"-")?[0-9]+)?)|([0-9]+(E("+"|"-")?[0-9]+))) { 
                                  yylval.real= atof(yytext);
                                  DB_PRINT("REAL_CONST ");
                                  return REAL_CONST; }

[0-9]+                          { yylval.integer = atoi(yytext); 
                                  DB_PRINT("INT_CONST ");
                                  return INT_CONST; }

 /* relational operators in PAL */
"="                             { DB_PRINT("EQUAL "); return ISEQUAL;}
"<>"				{ return NOTEQUAL;}
"<"				{ return LESSTHAN;}
">"				{ return GREATERTHAN;}
"<="				{ return LESSTHANEQUALS;}
">="				{ return GREATERTHANEQUALS;}
 /* arithmetic operators in PAL */
"+"				{ return PLUS;}
"-"				{ DB_PRINT("MINUS "); return MINUS;}
"*"				{ return MULTIPLY;}
"/"				{ return DIVIDE;}
 /* div and mod are under reserved words */
 /* other lexical characters */
":="				{ return ASSIGN;}
"("				{ DB_PRINT("L_PAREN "); return LEFTPAREN;}
")"				{ DB_PRINT("R_PAREN "); return RIGHTPAREN;}
"."				{ return PERIOD;}
";"				{ DB_PRINT("SEMICOLON "); return SEMICOLON;}
":"				{ return COLON;}
"["				{ return LEFTBRACKET;}
"]"				{ return RIGHTBRACKET;}
","				{ DB_PRINT("COMMA "); return COMMA;}
".."				{ return DOUBLEPERIOD;}


.                               { illegalChar = yytext[0];
                                  return UNKNOWN_CHARACTER; }

%%
void add() {
     last_column += yyleng;
}

/**
* Count the number of lines in a string
*/
int strcountlines (char *string) {
  int count = 0;
  char *charptr = string + 1;
  while (*charptr != 0) {
    if (*charptr == '\n') {
      count++;
    }
    charptr++;
  }
  //printf ("Number of newlines: %d\n", count);
  return count;
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
