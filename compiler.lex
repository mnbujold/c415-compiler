/**
* Lex file for parsing input into tokens for
* PAL language
* made by Daniel Chui
**/
%{
#include <stdio.h>
#include <string.h>
#include "yacc.tab.h"
#include "myerror.h"
extern char errortext[4096];
extern int last_column;
extern int lineno;
extern int oldlineno;
extern myerror *eList;

void updateError(void) {
    if(lineno != oldlineno) {
        updateErrorText(eList, errortext);
        showAllErrors(eList);
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
strcat (errortext, yytext);
}%

/* reserved keywords in PAL */
"and"						{ return AND;}
"array"						{ return ARRAY;}
"begin"						{ return BEGIN;}
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
"record"					{ return RECORD;}
"then"						{ return THEN;}
"type"						{ return TYPE;}
"var"						{ return VAR;}
"while"						{ return WHILE;}

/* relational operators in PAL */
"="							{ return ISEQUAL;}
"<>"						{ return NOTEQUAL;}
"<"							{ return LESSTHAN;}
">"							{ return GREATERTHAN;}
"<="						{ return LESSTHANEQUALS;}
">="						{ return GREATERTHANEQUALS;}

/* arithmetic operators in PAL */
"+"							{ return PLUS;}
"-"							{ return MINUS;}
"*"							{ return MULTIPLY;}
"/"							{ return DIVIDE;}
"div"						{ return DIV;}
"mod"						{ return MOD;}

/* other lexical characters */
":="						{ return ASSIGN;}
"("							{ return LEFTPAREN;}
")"							{ return RIGHTPAREN;}
"."							{ return PERIOD;}
";"							{ return SEMICOLON;}
":"							{ return COLON;}
"["							{ return LEFTBRACKET;}
"]"							{ return RIGHTBRACKET;}
","							{ return COMMA;}
".."						{ return DOUBLEPERIOD;}
/* comments */
"//"[^\n]*""        		{ /* do nothing, one line comment */              	     }
"{"[\^{}}]*"}"				{ /* do nothing, a block comment }

/* other */
\'[a-zA-Z+ \t]+\'[;]* 	 	{ return ID;}
int_const 					{ return INT_CONST;}
real_const					{ return REAL_CONST;}
[a-zA-Z0-9]+ 				{ return ID;}
\n                      	lineno++; updateError(); last_column=0;/*return RETURNN;/* ignore end of line */;
[ \t]+                  	add(); strcat(errortext, yytext); /* ignore whitespace */;

%%

void add() {
last_column += yyleng;
}