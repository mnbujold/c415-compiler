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
#include "syntaxtree.h"
#include "debug.h"

extern myerror *eList;
extern int iserror;
extern int lineno;
extern int last_column;
extern int looperrordetection;

extern GNode *syntaxTree;

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
    char character;
    int integer;
    double real;
    GNode *node;
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
%token <character> CHAR

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
/* %type <symbol> var_decl */
/* %type <symbol> var subscripted_var parm */
/* %type <symbol> expr simple_expr term factor unsigned_const unsigned_num */
/* %type <symbol> proc_heading */
%type <garray> f_parm_decl f_parm_list
%type <symbol> f_parm
/* %type <pf_invok> plist_finvok */
/* %type <symbol> func_invok */

%type <node> program decls
%type <node> var_decl_part var_decl_list var_decl
%type <node> proc_decl_part proc_decl_list proc_decl proc_heading
%type <node> compound_stat stat_list stat simple_stat struct_stat
%type <node> stat_assignment proc_invok var plist_finvok subscripted_var
%type <node> expr simple_expr term factor unsigned_const unsigned_num func_invok parm
%type <node> matched_stat if_header while_header

%left LEFTBRACKET ISEQUAL

%%
program                 : program_head decls compound_stat PERIOD
                            {
                                popLevel();
                                syntaxTree = createNode(NT_PROGRAM, $2, $3, NULL);
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
                            {
                                $$ = createNode(NT_DECLS, $3, $4, NULL);
                            }
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
                                addNewConst($1, extractSymbol($3));
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
                                if (noError(1, $1, $3, NULL)) {

                                    $$ = createArrayIndex(extractSymbol($1), extractSymbol($3));
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
                            {
                                $$ = createSingleNode(NT_VAR_DECL_PART);
                            }
                        | VAR var_decl_list SEMICOLON
                            {
                                $$ = createNode(NT_VAR_DECL_PART, $2, NULL);
                            }
                        | error /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        ;

var_decl_list           : var_decl
                            {
                                $$ = createNode(NT_VAR_DECL_LIST, $1, NULL);
                            }
                        | var_decl_list SEMICOLON var_decl
                            {
                                $$ = createNode(NT_VAR_DECL_LIST, $1, $3, NULL);
                            }
                        ;

var_decl                : ID COLON type
                            {
                                if ($3 != NULL) {
                                    $$ = createSymbolNode(addNewVar($1, $3));
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | ID COMMA var_decl
                            {
                                if (getNodeType($3) != NT_NONE) {
                                    $$ = createSymbolNode(addNewVar($1, extractType($3)));
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | error /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        ;

proc_decl_part          : /* empty */
                            {
                                $$ = createSingleNode(NT_PROC_DECL_PART);
                            }
                        | proc_decl_list
                            {
                                $$ = createNode(NT_PROC_DECL_PART, $1, NULL);
                            }
                        ;

proc_decl_list          : proc_decl
                            {
                                $$ = createNode(NT_PROC_DECL_LIST, $1, NULL);
                            }
                        | proc_decl_list proc_decl
                            {
                                $$ = createNode(NT_PROC_DECL_LIST, $1, $2, NULL);
                            }
                        ;

proc_decl               : proc_heading decls compound_stat SEMICOLON
                            {
                                if (noError(1, $1, NULL)) {
                                    checkFuncValSet(extractSymbol($1));
                                    $$ = createNode(NT_PROC_DECL, $1, $2, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                                popLevel();
                            }
                        | error SEMICOLON /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                                popLevel();
                            }
                        ;
                        
proc_heading            : PROCEDURE ID f_parm_decl SEMICOLON
                            {
                                if ($3 != NULL) {
                                    $$ = createSymbolNode(addNewProc($2, $3));
                                } else {
                                    addNewProc($2, NULL);
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | FUNCTION ID f_parm_decl COLON ID SEMICOLON
                            {
                                if ($3 != NULL) {
                                    $$ = createSymbolNode(addNewFunc($2, $5, $3));
                                } else {
                                    $$ = createSingleExprNode(NT_NONE, addNewFunc($2, $5, NULL));
                                }
                            }
                        | PROCEDURE ID error SEMICOLON /* ERROR */
                            {
                                addNewProc($2, NULL);
                                $$ = createSingleNode(NT_NONE);
                            }
                        | FUNCTION ID error SEMICOLON /* ERROR */
                            {
                                addNewFunc($2, NULL, NULL);
                                $$ = createSingleNode(NT_NONE);
                            }
                        | PROCEDURE error SEMICOLON /* ERROR */
                            {
                                pushLevel();
                                $$ = createSingleNode(NT_NONE);
                            }
                        | FUNCTION error SEMICOLON /* ERROR */
                            {
                                pushLevel();
                                $$ = createSingleNode(NT_NONE);
                            }
                        | error /* ERROR */
                            {
                                pushLevel();
                                $$ = createSingleNode(NT_NONE);
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
                            {
                                if (noError(0, $2, NULL)) {
                                    $$ = createNode(NT_COMPOUND_STAT, $2, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

stat_list               : stat
                            {
                                if (noError(0, $1, NULL)) {
                                    $$ = createNode(NT_STAT_LIST, $1, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | stat_list SEMICOLON stat
                            {
                                if (noError(0, $3, NULL)) {
                                    $$ = createNode(NT_STAT_LIST, $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | error SEMICOLON stat /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        ;

stat                    : simple_stat
                            {
                                if (noError(0, $1, NULL)) {
                                    $$ = createNode(NT_STAT, $1, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | struct_stat
                            {
                                if (noError(0, $1, NULL)) {
                                    $$ = createNode(NT_STAT, $1, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

simple_stat             : /* empty */
                            {
                                $$ = createSingleNode(NT_SIMPLE_STAT);
                            }
                        | stat_assignment
                            {
                                if (noError(0, $1, NULL)) {
                                    $$ = createNode(NT_SIMPLE_STAT, $1, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | proc_invok
                            {
                                if (noError(0, $1, NULL)) {
                                    $$ = createNode(NT_SIMPLE_STAT, $1, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | compound_stat
                            {
                                if (noError(0, $1, NULL)) {
                                    $$ = createNode(NT_SIMPLE_STAT, $1, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

stat_assignment         : var ASSIGN expr
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    doVarAssignment(extractSymbol($1), extractSymbol($3));
                                    $$ = createNode(NT_ASSIGNMENT, $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | error /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        ;
                        
proc_invok              : plist_finvok RIGHTPAREN
                            {
                                if (noError(1, $1, NULL)) {
                                    if (callProc(extractID($1), extractParamList($1))) {
                                        $$ = createNode(NT_PROC_INVOK, $1, NULL);
                                    } else {
                                        $$ = createSingleNode(NT_NONE);
                                    }
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | ID LEFTPAREN RIGHTPAREN
                            {
                                if (callProc($1, NULL)) {
                                    $$ = createNode(NT_PROC_INVOK, getProcNode($1), NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | IOPROC
                            {
                                if (checkIOProc($1, 1)) {
                                    $$ = createNode(NT_PROC_INVOK, getProcNode($1), NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | IOPROC LEFTPAREN RIGHTPAREN
                            {
                                if (callProc($1, NULL)) {
                                    $$ = createNode(NT_PROC_INVOK, getProcNode($1), NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

var                     : ID
                            {
                                symbol *tmpVar = getVarSymbol($1);
                                $$ = createExprNode(NT_VAR, tmpVar, createSymbolNode(tmpVar), NULL);
                            }
                        | var PERIOD ID
                            {
                                if (noError(1, $1, NULL)) {
                                    symbol *tmpField = getRecordField(extractSymbol($1), $3);
                                    $$ = createExprNode(NT_VAR, tmpField, $1, createSymbolNode(tmpField), NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | subscripted_var RIGHTBRACKET
                            {
                                $$ = $1;
                            }
                        | error RIGHTBRACKET /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        ;

subscripted_var         : var LEFTBRACKET expr
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createArrayNode(accessArray(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                                DEBUG_PRINT (("In subscripted var"));
                            }
                        | subscripted_var COMMA expr
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createArrayNode(accessArray(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

expr                    : simple_expr
                            {
                                $$ = createExprNode(NT_EXPR, extractSymbol($1), $1, NULL);
                            }
                        | expr ISEQUAL simple_expr
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_ISEQUAL, equalOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | expr NOTEQUAL simple_expr
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_NOTEQUAL, notEqualOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | expr LESSTHANEQUALS simple_expr
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_LESSTHANEQUALS, lessThanEqualOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | expr LESSTHAN simple_expr
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_LESSTHAN, lessThanOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | expr GREATERTHANEQUALS simple_expr
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_GREATERTHANEQUALS, greaterThanEqualOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | expr GREATERTHAN simple_expr
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_GREATERTHAN, greaterThanOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

simple_expr             : term
                            {
                                $$ = createExprNode(NT_EXPR, extractSymbol($1), $1, NULL);
                            }
                        | PLUS term
                            {
                                if (noError(1, $2, NULL)) {
                                    $$ = createExprNode(NT_IDENTITY, identity(extractSymbol($2)), $2, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | MINUS term
                            {
                                if (noError(1, $2, NULL)) {
                                    $$ = createExprNode(NT_INVERSION, inversion(extractSymbol($2)), $2, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | simple_expr PLUS term
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_PLUS, addOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | simple_expr MINUS term
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_MINUS, subtractOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | simple_expr OR term
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_OR, orOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

term                    : factor
                            {
                                $$ = createExprNode(NT_EXPR, extractSymbol($1), $1, NULL);
                            }
                        | term MULTIPLY factor
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_MULTIPLY, multOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | term DIVIDE factor
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_DIVIDE, realDivOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | term DIV factor
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_DIV, intDivOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | term MOD factor
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_MOD, modOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | term AND factor
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createExprNode(NT_AND, andOp(extractSymbol($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | error /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        ;

factor                  : var
                            {
                                $$ = createExprNode(NT_EXPR, extractSymbol($1), $1, NULL);
                            }
                        | unsigned_const
                            {
                                $$ = createExprNode(NT_EXPR, extractSymbol($1), $1, NULL);
                            }
                        | LEFTPAREN expr RIGHTPAREN
                            {
                                $$ = $2;
                            }
                        | LEFTPAREN error RIGHTPAREN /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        | func_invok
                            {
                                $$ = createExprNode(NT_EXPR, extractSymbol($1), $1, NULL);
                            }
                        | NOT factor
                            {
                                if (noError(1, $2, NULL)) {
                                    $$ = createExprNode(NT_NOT, notOp(extractSymbol($2)), $2, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

unsigned_const          : unsigned_num
                            {
                                $$ = $1;
                            }
                        | CHAR
                            {
                                union constant_values value = { .character = $1 };
                                $$ = createSymbolNode(createConstant(TC_CHAR, value));
                            }
                        | STRING
                            {
                                union constant_values value = { .string = $1 };
                                $$ = createSymbolNode(createConstant(TC_STRING, value));
                            }
                        ;

unsigned_num            : INT_CONST
                            {                                
                                union constant_values value = { .integer = $1 };
                                $$ = createSymbolNode(createConstant(TC_INTEGER, value));
                            }
                        | REAL_CONST
                            {                                
                                union constant_values value = { .real = $1 };
                                $$ = createSymbolNode(createConstant(TC_REAL, value));
                            }
                        ;

func_invok              : plist_finvok RIGHTPAREN
                            {
                                if (noError(1, $1, NULL)) {
                                    $$ = createExprNode(NT_FUNC_INVOK, callFunc(extractID($1), extractParamList($1)), $1, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | ID LEFTPAREN RIGHTPAREN
                            {
                                $$ = createSingleExprNode(NT_FUNC_INVOK, callFunc($1, NULL));
                            }
                        ;

plist_finvok            : ID LEFTPAREN parm
                            {
                                if (noError(1, $3, NULL)) {
                                    $$ = createPF_InvokNode(createArgList($1, extractSymbol($3)), getProcNode($1), $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | IOPROC LEFTPAREN parm
                            {
                                if (noError(1, $3, NULL)) {
                                    $$ = createPF_InvokNode(createArgList($1, extractSymbol($3)), getProcNode($1), $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | plist_finvok COMMA parm
                            {
                                if (noError(1, $1, $3, NULL)) {
                                    $$ = createPF_InvokNode(addArgument(extractPF_Invok($1), extractSymbol($3)), $1, $3, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

parm                    : expr
                            {                                
                                $$ = $1;
                            }
                        ;

struct_stat             : if_header THEN matched_stat ELSE stat
                            {
                                $$ = createNode(NT_IF_ELSE, $1, $3, $5, NULL);
                            }
                        | error ELSE stat /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        | if_header THEN stat
                            {
                                $$ = createNode(NT_IF, $1, $3, NULL);
                            }
                        | error THEN stat /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        | while_header stat
                            {
                                loopLevel -= 1;
                                $$ = createNode(NT_WHILE, $1, $2, NULL);
                            }
                        | error DO stat /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        | CONTINUE
                            {
                                if (checkControlFlow(loopLevel, "continue")) {
                                    $$ = createSingleNode(NT_CONTINUE);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | EXIT
                            {
                                if (checkControlFlow(loopLevel, "exit")) {
                                    $$ = createSingleNode(NT_EXIT);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

matched_stat            : simple_stat
                            {
                                $$ = createNode(NT_SIMPLE_STAT, $1, NULL);
                            }
                        | if_header THEN matched_stat ELSE matched_stat
                            {
                                $$ = createNode(NT_IF_ELSE, $1, $3, $5, NULL);
                            }
                        | error ELSE matched_stat /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        | while_header matched_stat
                            {
                                loopLevel -= 1;
                                $$ = createNode(NT_WHILE, $1, $2, NULL);
                            }
                        | error DO matched_stat /* ERROR */
                            {
                                $$ = createSingleNode(NT_NONE);
                            }
                        | CONTINUE
                            {
                                if (checkControlFlow(loopLevel, "continue")) {
                                    $$ = createSingleNode(NT_CONTINUE);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        | EXIT
                            {
                                if (checkControlFlow(loopLevel, "exit")) {
                                    $$ = createSingleNode(NT_EXIT);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;

if_header               : IF expr
                            {
                                if (noError(1, $2, NULL) && checkConditional(extractSymbol($2))) {
                                    $$ = createNode(NT_EXPR, $2, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                            }
                        ;
                        
while_header            : WHILE expr DO
                            {
                                if (noError(1, $2, NULL) && checkConditional(extractSymbol($2))) {
                                    $$ = createNode(NT_EXPR, $2, NULL);
                                } else {
                                    $$ = createSingleNode(NT_NONE);
                                }
                                loopLevel += 1;
                            }                            
                        ;

%%
