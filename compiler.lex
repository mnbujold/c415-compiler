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
	        } /* if */
} /* if */

%}
%%

"and"
"array"
"begin"
"const"
"continue"
"div"
"do"
"else"
"end"
"exit"
"function"
"if"
"mod"
"not"
"of"
"or"
"procedure"
"program"
"record"
"then"
"type"
"var"
"while"
"="
"<>"
"<"
"<="
">="
"+"
"-"
"/"
"div"
"mod"
":="
"("
")"
"."
";"
":"
"["
"]"
","

,	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Comma;
\(	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Left_Perentheses;
\)	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Right_Perentheses;
\[	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Left_Token;
\]	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Right_Token;
\>	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Greater_Than;
\<	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Less_Than;

;	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Semi_Colon;
:	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Colon;
:=	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Assign;
=	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Equals;
\-	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Minus;
\*	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Multiply;
\+	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Plus;
\.\. 						last_column += strlen(yytext); strcat(errortext, yytext);  return Double_Period;
\. 							last_column += strlen(yytext); strcat(errortext, yytext);  return Period;
and 						last_column += strlen(yytext); strcat(errortext, yytext);  return AND;
\/	 						last_column += strlen(yytext); strcat(errortext, yytext);  return Divide;
array 						last_column += strlen(yytext); strcat(errortext, yytext); return ARRAY;
begin 						last_column += strlen(yytext); strcat(errortext, yytext); return BEGINN;
bool 						last_column += strlen(yytext); strcat(errortext, yytext); return BOOL;
char 						last_column += strlen(yytext); strcat(errortext, yytext); return CHAR;
string 						last_column += strlen(yytext); strcat(errortext, yytext); return STRING;
const 						last_column += strlen(yytext); strcat(errortext, yytext); return CONSTT;
continue 					last_column += strlen(yytext); strcat(errortext, yytext); return CONTINUE;
div 						last_column += strlen(yytext); strcat(errortext, yytext); return DIV;
do 							last_column += strlen(yytext); strcat(errortext, yytext); return DO;
else 						last_column += strlen(yytext); strcat(errortext, yytext); return ELSE;
end 						last_column += strlen(yytext); strcat(errortext, yytext); return END;
exit 						last_column += strlen(yytext); strcat(errortext, yytext); return EXIT;
function 					last_column += strlen(yytext); strcat(errortext, yytext); return FUNCTION;
if 							last_column += strlen(yytext); strcat(errortext, yytext); return IF;
int 						last_column += strlen(yytext); strcat(errortext, yytext); return INT;
mod 						last_column += strlen(yytext); strcat(errortext, yytext); return MOD;
not							last_column += strlen(yytext); strcat(errortext, yytext); return NOT;
of 							last_column += strlen(yytext); strcat(errortext, yytext); return OF;
or 							last_column += strlen(yytext); strcat(errortext, yytext); return OR;
procedure 					last_column += strlen(yytext); strcat(errortext, yytext); return PROCEDURE;
program 					last_column += strlen(yytext); strcat(errortext, yytext); return PROGRAM;
real 						last_column += strlen(yytext); strcat(errortext, yytext); return REAL;
record 						last_column += strlen(yytext); strcat(errortext, yytext); return RECORD;
\'[a-zA-Z+ \t]+\'[;]* 						last_column += strlen(yytext); strcat(errortext, yytext); return ID;
then 						last_column += strlen(yytext); strcat(errortext, yytext); return THEN;
type 						last_column += strlen(yytext); strcat(errortext, yytext); return TYPE;
var 						last_column += strlen(yytext); strcat(errortext, yytext); return VAR;
while 						last_column += strlen(yytext); strcat(errortext, yytext); return WHILE;
int_const 					last_column += strlen(yytext); strcat(errortext, yytext); return INT_CONST;
real_const					return REAL_CONST;
[a-zA-Z0-9]+ 				last_column += strlen(yytext); strcat(errortext, yytext); return ID;
\n                      	lineno++; updateError(); last_column=0;/*return RETURNN;/* ignore end of line */;
[ \t]+                  	last_column += strlen(yytext); strcat(errortext, yytext); /* ignore whitespace */;
%%