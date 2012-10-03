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

%token <string> AND ARRAY BEGINN BOOL CHAR CONSTT CONTINUE DIV
%token <string> DO ELSE END EXIT FUNCTION IF INT MOD End_of_Line
%token <string> NOT OF OR PROCEDURE PROGRAM REAL RECORD
%token <string> STRING THEN TYPE VAR WHILE INT_CONST REAL_CONST
%token <string> Left_Perentheses Right_Perentheses Comma Semi_Colon Equals Colon
%token <string> Left_Token Right_Token Double_Period Period Less_Than Greater_Than Assign
%token <string> Plus Multiply Minus Divide
%token <string> ID RETURNN

%type <string> expr simple_expr term factor var subscripted_var unsigned_const
%type <string> func_invok unsigned_num plist_finvok

%left Left_Token Equals

%%
program : program_head decls compound_stat Period
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

program_head : PROGRAM ID Left_Perentheses ID Comma ID Right_Perentheses Semi_Colon
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
				
const_decl_part : CONSTT const_decl_list Semi_Colon
|
;

const_decl_list : const_decl
| const_decl_list Semi_Colon const_decl
;

const_decl : ID Equals expr { printf("const_decl\n"); }
;

type_decl_part : TYPE type_decl_list Semi_Colon { printf("type_decl_part\n"); }
|
;

type_decl_list : 			type_decl_list Semi_Colon type_decl { printf("type_decl_list\n"); }
							|
							type_decl { printf("type_decl_list\n"); }
							;

type_decl : ID Equals type { printf("type_decl\n"); }
;

type : simple_type { printf("type\n"); }
| structured_type { printf("type\n"); }
;

simple_type:		scalar_type
					| REAL  { printf("simple_type\n"); }
					| ID  { printf("simple_type\n"); }
					;

scalar_type:		Left_Perentheses scalar_list Right_Perentheses { printf("scalar_type\n"); }
					| INT { printf("scalar_type\n"); }
					| BOOL { printf("scalar_type\n"); }
					| CHAR { printf("scalar_type\n"); }
					| STRING { printf("scalar_type\n"); }
					;

scalar_list:			ID { printf("scalar_list\n"); }
						| scalar_list Comma ID { printf("scalar_list\n"); }
						;

structured_type:		ARRAY Left_Token array_type Right_Token OF type  { printf("structured_type\n"); }
						| RECORD field_list END { printf("structured_type\n"); }
						;

array_type:				simple_type { printf("array_type\n"); }
						| simple_type Double_Period simple_type { printf("array_type\n"); }
						;

field_list:				field { printf("field_list\n"); }
						| field_list Semi_Colon field { printf("field_list\n"); }
						;

field : ID Colon type { printf("field\n"); }
;

var_decl_part : VAR var_decl_list Semi_Colon { printf("var_decl_part\n"); }
|
;

var_decl_list:					var_decl  { printf("var non-recursive\n"); }
								|
								var_decl_list Semi_Colon var_decl { printf("var recursive\n"); }
								;

var_decl:						ID Colon type
								| ID Comma var_decl
								;

proc_decl_part : proc_decl_list
|
;

proc_decl_list:			proc_decl
						| proc_decl_list proc_decl
						;

proc_decl : proc_heading decls compound_stat Semi_Colon
;

proc_heading : 			PROCEDURE ID f_parm_decl Semi_Colon
						| PROCEDURE ID Semi_Colon
						| FUNCTION ID f_parm_decl Colon ID Semi_Colon
						;

f_parm_decl:			Left_Perentheses f_parm_list Right_Perentheses
						| Left_Perentheses Right_Perentheses
						;

f_parm_list:			f_parm
						| f_parm_list Semi_Colon f_parm
						;

f_parm: 				ID Colon ID
						| VAR ID Colon ID
						;

compound_stat: 			BEGINN stat_list END
						;

stat_list: 				stat
						| stat_list Semi_Colon stat
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

simple_stat:		var Assign expr
					| proc_invok
					| compound_stat
					;

proc_invok : plist_finvok Right_Perentheses
| ID Left_Perentheses Right_Perentheses
;

var : ID
| var Period ID
| subscripted_var Right_Token
;

subscripted_var : var Left_Token expr
| subscripted_var Comma expr
;
expr : simple_expr
| expr Equals simple_expr
| expr Less_Than Greater_Than simple_expr
| expr Less_Than Equals simple_expr
| expr Less_Than simple_expr
| expr Greater_Than Equals simple_expr
| expr Greater_Than simple_expr
;
simple_expr: term
| Plus term
| Minus term
| simple_expr Plus term
| simple_expr Minus term
| simple_expr OR term
;
term : factor
| term Multiply factor
| term Divide factor
| term DIV factor
| term MOD factor
| term AND factor
;

factor : var
| unsigned_const
| Left_Perentheses expr Right_Perentheses
| func_invok
| NOT factor
;

unsigned_const : 		unsigned_num
						| STRING
						;

unsigned_num : 			INT_CONST
						| REAL_CONST
						;

func_invok : plist_finvok Right_Perentheses
| ID Left_Perentheses Right_Perentheses
;

plist_finvok : ID Left_Perentheses parm
| plist_finvok Comma parm
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
