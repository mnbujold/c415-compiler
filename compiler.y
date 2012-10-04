/**
* Bison file for PAL compiler
**/
%{
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include "symbol.h"
#include "myerror.h"

extern symbol 		*sList;  
extern myerror 		*eList;  
extern symbol 		*sSelected;  
extern int 			iserror;
extern int 			lineno, oldlineno;
extern int 			last_column;
extern char 		errortext[4096];
extern int 			looperrordetection;


void yyerror(const char *str)
{
	eList = addError(eList, str, NULL, last_column, lineno);
}

int yywrap()
{
        return 1;
} 

show_error() {
        	if(lineno != oldlineno) {
        		updateErrorText(eList, errortext);
        		showAllErrors(eList);
	        	eList = deleteAllErrors(eList);
	        	oldlineno = lineno;
		        memset(errortext, '\0', 4096);
	        } /* if */
} /*show_error*/

%}

%defines  
%error-verbose

%union 
{
        int number;
        char string[4096];
}
/* Reserved word tokens */
/* note: MOD and DIV are under operator tokens */
%token <string> AND ARRAY BEGIN_ CONST 
%token <string> CONTINUE DO ELSE
%token <string> END EXIT FUNCTION IF  
%token <string> NOT OF OR 
%token <string> PROCEDURE PROGRAM RECORD THEN
%token <string> TYPE VAR WHILE 

/* Relational tokens */
%token <string> ISEQUAL NOTEQUAL LESSTHAN GREATERTHAN 
%token <string> LESSTHANEQUALS GREATERTHANEQUALS

/* Operator tokens */
/* note: DIVIDE is for reals, div is for integer */
%token <string> PLUS MINUS MULTIPLY DIVIDE DIV MOD

/* Miscellaneous tokens */
%token <string> ASSIGN LEFTPAREN RIGHTPAREN PERIOD SEMICOLON COLON
%token <string> LEFTBRACKET RIGHTBRACKET COMMA DOUBLEPERIOD  

%token <string> ID RETURN

/* type tokens */
%token <string> BOOL CHAR INT REAL STRING
%token <string> INT_CONST REAL_CONST

%token <string> End_of_Line //eh...is this even used?
%type <string> expr simple_expr term factor var subscripted_var unsigned_const
%type <string> func_invok unsigned_num plist_finvok

%left LEFTBRACKET ISEQUAL

%%
program : program_head decls compound_stat PERIOD
		| error {
			iserror = 1;
			yyerrok;
			looperrordetection++;
			if(looperrordetection == 300) {
				printf("loop error detected!(%d)\n",looperrordetection);
				looperrordetection = 0;
				yyclearin;
				/*yylex();*/
				return 0;
			} /* if */
		}
;

program_head : PROGRAM ID LEFTPAREN ID COMMA ID RIGHTPAREN SEMICOLON
{
	printf("End of program head\n");
}
;

decls : 		const_decl_part type_decl_part var_decl_part proc_decl_part
		| error var {
			iserror = 1;
			yyerrok;
		}
			;
				
const_decl_part : CONST const_decl_list SEMICOLON
|
;

const_decl_list : const_decl
| const_decl_list SEMICOLON const_decl
;

const_decl : ID ISEQUAL expr { printf("const_decl\n"); }
;


type_decl_part : TYPE type_decl_list SEMICOLON { printf("type_decl_part\n"); }
|
;

type_decl_list : 			type_decl_list SEMICOLON type_decl { printf("type_decl_list\n"); }
							|
							type_decl { printf("type_decl_list\n"); }
							;

type_decl : ID ISEQUAL type { printf("type_decl\n"); }
;

type : simple_type { printf("type\n"); }
| structured_type { printf("type\n"); }
;

simple_type:		scalar_type
					| REAL  { printf("simple_type\n"); }
					| ID  { printf("simple_type\n"); }
					;

scalar_type:		LEFTPAREN scalar_list RIGHTPAREN { printf("scalar_type\n"); }
					| INT { printf("scalar_type\n"); }
					| BOOL { printf("scalar_type\n"); }
					| CHAR { printf("scalar_type\n"); }
					| STRING { printf("scalar_type\n"); }
					;

scalar_list:			ID { printf("scalar_list\n"); }
						| scalar_list COMMA ID { printf("scalar_list\n"); }
						;

structured_type:		ARRAY LEFTBRACKET array_type RIGHTBRACKET OF type  { printf("structured_type\n"); }
						| RECORD field_list END { printf("structured_type\n"); }
						;

array_type:				simple_type { printf("array_type\n"); }
						| simple_type DOUBLEPERIOD simple_type { printf("array_type\n"); }
						;

field_list:				field { printf("field_list\n"); }
						| field_list SEMICOLON field { printf("field_list\n"); }
						;

field : ID COLON type { printf("field\n"); }
;

var_decl_part : VAR var_decl_list SEMICOLON { printf("var_decl_part\n"); }
|
;

var_decl_list:					var_decl  { printf("var non-recursive\n"); }
								|
								var_decl_list SEMICOLON var_decl { printf("var recursive\n"); }
								;

var_decl:						ID COLON type
								| ID COMMA var_decl
								;

proc_decl_part : proc_decl_list
|
;

proc_decl_list:			proc_decl
						| proc_decl_list proc_decl
						;

proc_decl : proc_heading decls compound_stat SEMICOLON
;

proc_heading : 			PROCEDURE ID f_parm_decl SEMICOLON
						| PROCEDURE ID SEMICOLON
						| FUNCTION ID f_parm_decl COLON ID SEMICOLON
						;

f_parm_decl:			LEFTPAREN f_parm_list RIGHTPAREN
						| LEFTPAREN RIGHTPAREN
						;

f_parm_list:			f_parm
						| f_parm_list SEMICOLON f_parm
						;

f_parm: 				ID COLON ID
						| VAR ID COLON ID
						;

compound_stat: 			BEGIN_ stat_list END
						;

stat_list: 				stat
						| stat_list SEMICOLON stat
		| IF error {
			iserror = 1;
			yyerrok;
		}
		| error var{
			iserror = 1;
			yyerrok;
		}
						;

stat: 					simple_stat
						| struct_stat
						;

simple_stat:		var ASSIGN expr
					| proc_invok
					| compound_stat
					;

proc_invok : plist_finvok RIGHTPAREN
| ID LEFTPAREN RIGHTPAREN
;

var : ID
| var PERIOD ID
| subscripted_var RIGHTBRACKET
;

subscripted_var : var LEFTBRACKET expr
| subscripted_var COMMA expr
;
expr : simple_expr
| expr ISEQUAL simple_expr
| expr LESSTHAN GREATERTHAN simple_expr
| expr LESSTHAN ISEQUAL simple_expr
| expr LESSTHAN simple_expr
| expr GREATERTHAN ISEQUAL simple_expr
| expr GREATERTHAN simple_expr
;
simple_expr: term
| PLUS term
| MINUS term
| simple_expr PLUS term
| simple_expr MINUS term
| simple_expr OR term
;
term : factor
| term MULTIPLY factor
| term DIVIDE factor
| term DIV factor
| term MOD factor
| term AND factor
;

factor : var
| unsigned_const
| LEFTPAREN expr RIGHTPAREN
| func_invok
| NOT factor
;

unsigned_const : 		unsigned_num
						| STRING
						;

unsigned_num : 			INT_CONST
						| REAL_CONST
						;

func_invok : plist_finvok RIGHTPAREN
| ID LEFTPAREN RIGHTPAREN
;

plist_finvok : ID LEFTPAREN parm
| plist_finvok COMMA parm
;

parm: 			expr
				;
struct_stat: IF expr THEN matched_stat ELSE stat
| IF expr THEN stat
| WHILE expr DO stat
| CONTINUE
| EXIT
;

matched_stat : simple_stat
| IF expr THEN matched_stat ELSE matched_stat
| WHILE expr DO matched_stat
| CONTINUE
| EXIT
;

%%
