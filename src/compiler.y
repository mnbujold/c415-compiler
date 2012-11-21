/**
* Bison file for PAL compiler
* 
* Authors: Paul, James
* 
**/

%{
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

#include "symbol.h"
#include "type.h"
#include "operators.h"
#include "myerror.h"

#include "debug.h"

extern myerror *eList;
extern int iserror;
extern int lineno;
extern int last_column;
extern int looperrordetection;

void yyerror(const char *str) {
    iserror = 1;
    eList = addError(eList, str, last_column, lineno);
}

int yywrap() {
        return 1;
}

// For control flow checking:
int loopLevel = 0;

%}

%defines
%error-verbose

%union {
    char *id;
    char *string;
    int integer;
    double real;
    symbol *symbol;
    GPtrArray *garray;
    struct pf_invok *pf_invok;
}

/* Reserved word tokens */
/* note: MOD and DIV are under operator tokens */
%token AND ARRAY BEGIN_ CONST 
%token CONTINUE DO ELSE
%token END EXIT FUNCTION IF  
%token NOT OF OR 
%token PROCEDURE PROGRAM RECORD THEN
%token TYPE VAR WHILE 

/* Relational tokens */
%token ISEQUAL NOTEQUAL LESSTHAN GREATERTHAN 
%token LESSTHANEQUALS GREATERTHANEQUALS

/* Operator tokens */
/* note: DIVIDE is for reals, div is for integer */
%token PLUS MINUS MULTIPLY DIVIDE DIV MOD

/* type tokens */

%token <integer> INT_CONST
%token <real> REAL_CONST 
%token <string> STRING

/* Miscellaneous tokens */
%token ASSIGN LEFTPAREN RIGHTPAREN PERIOD SEMICOLON COLON
%token LEFTBRACKET RIGHTBRACKET COMMA DOUBLEPERIOD 
%token RETURN
%token <id> ID
/* Special cases need special rules */
%token <id> IOPROC

%token <string> UNKNOWN_CHARACTER

/* Symbol types */
%type <symbol> type 
%type <symbol> structured_type closed_array_type array_type
%type <symbol> simple_type
%type <garray> scalar_type scalar_list field_list 
%type <symbol> field
%type <symbol> var_decl
%type <symbol> var subscripted_var parm
%type <symbol> expr simple_expr term factor unsigned_const unsigned_num
%type <symbol> proc_heading
%type <garray> f_parm_decl f_parm_list
%type <symbol> f_parm
%type <pf_invok> plist_finvok
%type <symbol> func_invok

%left LEFTBRACKET ISEQUAL

%%
program                 : program_head decls compound_stat PERIOD
                            {
                                popLevel();
                            }
                        | error PERIOD /* ERROR */
                            {
                                popLevel();
                            }
                        | error /* ERROR */
                            {
                                iserror = 1;
                                yyerrok;
                                looperrordetection++;
                                if(looperrordetection == 300) {
                                    looperrordetection = 0;
                                    yyclearin;
                                    /*yylex();*/
                                    return 1;
                                }
                            }
                        ;

program_head            : PROGRAM ID LEFTPAREN ID COMMA ID RIGHTPAREN SEMICOLON
                            {
                                pushLevel();
                                addProgramSymbols($2, $4, $6);
                            }
                        | PROGRAM ID LEFTPAREN error RIGHTPAREN SEMICOLON /* ERROR */
                            {
                                pushLevel();
                            }
                        | error /* ERROR */
                            {
                                pushLevel();
                            }
                        ;

decls                   : const_decl_part type_decl_part var_decl_part proc_decl_part
                        ;

const_decl_part         : /* empty */
                        | CONST const_decl_list SEMICOLON
                        | error SEMICOLON /* ERROR */
                        | error /* ERROR */
                        ;

const_decl_list         : const_decl
                        | const_decl_list SEMICOLON const_decl
                        ;

const_decl              : ID ISEQUAL expr
                            {
                                addNewConst($1, $3);
                            }
                        | error /* ERROR */ 
                        ;


type_decl_part          : /* empty */
                        | TYPE type_decl_list SEMICOLON
                        | error SEMICOLON /* ERROR */
                        | error /* ERROR */
                        ;

type_decl_list           :  /* empty */
                        | type_decl
                        | type_decl_list SEMICOLON type_decl
                        ;

type_decl               : ID ISEQUAL type
                            {
                                if ($3 != NULL) {
                                    addNewType($1, $3);
                                }
                            }
                        | error /* ERROR */
                        ;

type                    : simple_type
                            {
                                $$ = $1;
                            }
                        | structured_type
                            {
                                $$ = $1;
                            }
                        | error /* ERROR */
                            {
                                $$ = NULL;
                            }
                        ;

simple_type             : scalar_type
                            {
                                if ($1 != NULL) {
                                    $$ = createScalarList($1);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | ID
                            {
                                DEBUG_PRINT (("magic In get type"));
                                $$ = getType($1);
                            }
                        ;

scalar_type             : LEFTPAREN scalar_list RIGHTPAREN
                            {
                                $$ = $2;
                            }
                        | LEFTPAREN error RIGHTPAREN /* ERROR */
                            {
                                $$ = NULL;
                            }
                        | LEFTPAREN error /* ERROR */
                            {
                                $$ = NULL;
                            }
                        | error RIGHTPAREN /* ERROR */
                            {
                                $$ = NULL;
                            }
                        ;

scalar_list             : ID
                            {
                                $$ = addScalar(NULL, $1);
                            }
                        | scalar_list COMMA ID
                            {
                                if ($1 != NULL) {
                                    $$ = addScalar($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        ;

structured_type         : ARRAY closed_array_type OF type
                            {
                                DEBUG_PRINT (("Inside structured type\n"));
                                if ($2 != NULL && $4 != NULL) {
                                    DEBUG_PRINT (("Type: %s\n", $4->name));
                                    $$ = createArray($2, $4);
                                } else {
                                    $$ = NULL;
                                }
                                DEBUG_PRINT(("Finished calling structured type\n"));
                            }
                        | RECORD field_list END
                            {
                                if ($2 != NULL) {
                                    $$ = createRecord($2);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        ;

closed_array_type       : LEFTBRACKET array_type RIGHTBRACKET
                            {
                                DEBUG_PRINT (("inside closed aray type: %p", $$));
                                $$ = $2;
                            }
                        | LEFTBRACKET error RIGHTBRACKET /* ERROR */
                            {
                                $$ = NULL;
                            }
                        | LEFTBRACKET error /* ERROR */
                            {
                                $$ = NULL;
                            }
                        | error RIGHTBRACKET /* ERROR */
                            {
                                $$ = NULL;
                            }
                        ;

array_type              : ID
                            {
                                symbol *type = getType($1);
                                if (type != NULL) {
                                    $$ = createArrayIndex(NULL, type);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | expr DOUBLEPERIOD expr
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = createArrayIndex($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        ;
                     
                        
field_list              : field
                            {
                                DEBUG_PRINT(("Inside field list"));
                                if ($1 != NULL) {
                                    $$ = addField(NULL, $1);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | field_list SEMICOLON field
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = addField($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        ;

field                   : ID COLON type
                            {
                                DEBUG_PRINT(("In field\n"));
                                if ($3 != NULL) {
                                    $$ = createSymbol($1, $3, OC_PARAM, NULL);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | error /* ERROR */
                            {
                                $$ = NULL;
                            }
                        ;

var_decl_part           : /* empty */
                        | VAR var_decl_list SEMICOLON
                        | error /* ERROR */
                        ;

var_decl_list           : var_decl
                        | var_decl_list SEMICOLON var_decl
                        ;

var_decl                : ID COLON type
                            {
                                if ($3 != NULL) {
                                    $$ = addNewVar($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | ID COMMA var_decl
                            {
                                if ($3 != NULL) {
                                    $$ = addNewVar($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | error /* ERROR */
                            {
                                $$ = NULL;
                            }
                        ;

proc_decl_part          : /* empty */
                        | proc_decl_list
                        ;

proc_decl_list          : proc_decl
                        | proc_decl_list proc_decl
                        ;

proc_decl               : proc_heading decls compound_stat SEMICOLON
                            {
                                if ($1 != NULL) {
                                    checkFuncValSet($1);
                                }
                                popLevel();
                            }
                        | error SEMICOLON /* ERROR */
                            {
                                popLevel();
                            }
                        ;

proc_heading            : PROCEDURE ID f_parm_decl SEMICOLON
                            {
                                if ($3 != NULL) {
                                    $$ = addNewProc($2, $3);
                                } else {
                                    $$ = NULL;
                                    addNewProc($2, NULL);
                                }
                            }
                        | FUNCTION ID f_parm_decl COLON ID SEMICOLON
                            {
                                if ($3 != NULL) {
                                    $$ = addNewFunc($2, $5, $3);
                                } else {
                                    $$ = NULL;
                                    addNewFunc($2, $5, NULL);
                                }
                            }
                        | PROCEDURE ID error SEMICOLON /* ERROR */
                            {
                                $$ = NULL;
                                addNewProc($2, NULL);
                            }
                        | FUNCTION ID error SEMICOLON /* ERROR */
                            {
                                $$ = NULL;
                                addNewFunc($2, NULL, NULL);
                            }
                        | PROCEDURE error SEMICOLON /* ERROR */
                            {
                                $$ = NULL;
                                pushLevel();
                            }
                        | FUNCTION error SEMICOLON /* ERROR */
                            {
                                $$ = NULL;
                                pushLevel();
                            }
                        | error /* ERROR */
                            {
                                $$ = NULL;
                                pushLevel();
                            }
                        ;

f_parm_decl             : LEFTPAREN f_parm_list RIGHTPAREN
                            {
                                $$ = $2;
                            }
                        | LEFTPAREN RIGHTPAREN
                            {
                                $$ = addParam(NULL, NULL);
                            }
                        | error RIGHTPAREN /* ERROR */
                            {
                                $$ = NULL;
                            }
                        | error /* ERROR */
                            {
                                $$ = NULL;
                            }
                        ;

f_parm_list             : f_parm
                            {
                                if ($1 != NULL) {
                                    $$ = addParam(NULL, $1);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | f_parm_list SEMICOLON f_parm
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = addParam($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        ;

f_parm                  : ID COLON ID
                            {
                                $$ = addNewParam($1, $3, 0);
                            }
                        | VAR ID COLON ID
                            {
                                $$ = addNewParam($2, $4, 1);
                            }
                        | error /* ERROR */
                            {
                                $$ = NULL;
                            }
                        ;

compound_stat           : BEGIN_ stat_list END
                        ;

stat_list               : stat
                        | stat_list SEMICOLON stat
                        | error SEMICOLON stat /* ERROR */
                        ;

stat                    : simple_stat
                        | struct_stat
                        ;

simple_stat             : /* empty */
                        | stat_assignment
                        | proc_invok
                        | compound_stat
                        ;

stat_assignment         : var ASSIGN expr
                            {
                                //DEBUG_PRINT(("Inside a var assignment"));
                                if ($1 != NULL && $3 != NULL) {
                                    doVarAssignment ($1, $3);
                                }
                            }
                        | error /* ERROR */
                        ;
                        
proc_invok              : plist_finvok RIGHTPAREN
                            {
                                if ($1 != NULL) {
                                    callProc($1->id, $1->paramList);
                                }
                            }
                        | ID LEFTPAREN RIGHTPAREN
                            {
                                callProc($1, NULL);
                            }
                        | IOPROC
                            {
                                checkIOProc($1, 1);
                            }
                        | IOPROC LEFTPAREN RIGHTPAREN
                            {

                                callProc($1, NULL);
                            }
                        ;

var                     : ID
                            {
                                $$ = getVarSymbol($1);
                            }
                        | var PERIOD ID
                            {
                                if ($1 != NULL) {
                                    $$ = getRecordField($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | subscripted_var RIGHTBRACKET
                            {
                                $$ = $1;
                            }
                        | error RIGHTBRACKET /* ERROR */
                            {
                                $$ = NULL;
                            }
                        ;

subscripted_var         : var LEFTBRACKET expr
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = accessArray($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                                DEBUG_PRINT (("In subscripted var"));
                            }
                        | subscripted_var COMMA expr
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = accessArray($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        ;

expr                    : simple_expr
                            {
                                if ($1 != NULL) {
                                }
                                $$ = $1;
                            }
                        | expr ISEQUAL simple_expr
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = equalOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | expr NOTEQUAL simple_expr
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = notEqualOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | expr LESSTHANEQUALS simple_expr
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = lessThanEqualOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | expr LESSTHAN simple_expr
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = lessThanOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | expr GREATERTHANEQUALS simple_expr
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = greaterThanEqualOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | expr GREATERTHAN simple_expr
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = greaterThanOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        ;

simple_expr             : term
                            {
                                $$ = $1;
                            }
                        | PLUS term
                            {
                                if ($2 != NULL) {
                                    $$ = identity($2);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | MINUS term
                            {
                                if ($2 != NULL) {
                                    $$ = inversion($2);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | simple_expr PLUS term
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = addOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | simple_expr MINUS term
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = subtractOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | simple_expr OR term
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = orOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        ;

term                    : factor
                            {
                                $$ = $1;
                            }
                        | term MULTIPLY factor
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = multOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | term DIVIDE factor
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = realDivOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | term DIV factor
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = intDivOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | term MOD factor
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = modOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | term AND factor
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = andOp($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | error /* ERROR */
                            {
                                $$ = NULL;
                            }
                        ;

factor                  : var
                            {
                                $$ = $1;
                            }
                        | unsigned_const
                            {
                                $$ = $1;
                            }
                        | LEFTPAREN expr RIGHTPAREN
                            {
                                $$ = $2;
                            }
                        | LEFTPAREN error RIGHTPAREN /* ERROR */
                            {
                                $$ = NULL;
                            }
                        | func_invok
                            {
                                $$ = $1;
                            }
                        | NOT factor
                            {
                                $$ = notOp($2);
                            }
                        ;

unsigned_const          : unsigned_num
                            {
                                $$ = $1;
                            }
                        | STRING
                            {
                                if (strlen($1) == 1) {
                                    union constant_values value = { .character = $1[0] };
                                     $$ = createConstant(TC_CHAR, value);
                                } else {
                                    union constant_values value = { .string = $1 };
                                    $$ = createConstant(TC_STRING, value);
                                }
                            }
                        ;

unsigned_num            : INT_CONST
                            {                                
                                union constant_values value = { .integer = $1 };
                                $$ = createConstant(TC_INTEGER, value);
                            }
                        | REAL_CONST
                            {                                
                                union constant_values value = { .real = $1 };
                                $$ = createConstant(TC_REAL, value);
                            }
                        ;

func_invok              : plist_finvok RIGHTPAREN
                            {
                                if ($1 != NULL) {
                                    $$ = callFunc($1->id, $1->paramList);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | ID LEFTPAREN RIGHTPAREN
                            {
                                $$ = callFunc($1, NULL);
                            }
                        ;

plist_finvok            : ID LEFTPAREN parm
                            {
                                if ($3 != NULL) {
                                    $$ = createArgList($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | IOPROC LEFTPAREN parm
                            {
                                if ($3 != NULL) {
                                    $$ = createArgList($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        | plist_finvok COMMA parm
                            {
                                if ($1 != NULL && $3 != NULL) {
                                    $$ = addArgument($1, $3);
                                } else {
                                    $$ = NULL;
                                }
                            }
                        ;

parm                    : expr
                            {                                
                                $$ = $1;
                            }
                        ;

struct_stat             : if_header THEN matched_stat ELSE stat
                        | error ELSE stat /* ERROR */
                        | if_header THEN stat
                        | error THEN stat /* ERROR */
                        | while_header stat
                            {
                                loopLevel -= 1;
                            }
                        | error DO stat /* ERROR */
                        | CONTINUE
                            {
                                checkControlFlow(loopLevel, "continue");
                            }
                        | EXIT
                            {
                                checkControlFlow(loopLevel, "exit");
                            }
                        ;

matched_stat            : simple_stat
                        | if_header THEN matched_stat ELSE matched_stat
                        | error ELSE matched_stat /* ERROR */
                        | while_header matched_stat
                            {
                                loopLevel -= 1;
                            }
                        | error DO matched_stat /* ERROR */
                        | CONTINUE
                            {
                                checkControlFlow(loopLevel, "continue");
                            }
                        | EXIT
                            {
                                checkControlFlow(loopLevel, "exit");
                            }
                        ;

if_header               : IF expr
                            {
                                if ($2 != NULL) {
                                    checkConditional($2);
                                }
                            }
                        ;
                        
while_header            : WHILE expr DO
                            {
                                if ($2 != NULL) {
                                    checkConditional($2);
                                }
                                loopLevel += 1;
                            }                            
                        ;

%%
