/**
 * Header file for Ascgen.c
 * generate ASC assembly based on syntax tree
 * Author: Daniel Chui
 */
#include <glib.h>

#ifndef ASCGEN_H
#define ASCGEN_H


#define NUM_ASC_REGISTERS 16

/****************************************************************
 * Struct definitions for things code generation needs to keep 
 * track of
 ****************************************************************/
struct var_add_struct {
    int indexingRegister;
    int offset;
};

typedef struct var_add_struct varAddressStruct;

struct proc_info_struct {
    char *procLabel;
    int indexingRegister;
};

typedef struct proc_info_struct procInfo;

/****************************************************************
 * Functions to generate code based on syntax tree
 ****************************************************************/
void genASCCode (GNode *tree, char *fileName);



void genCodeForFunctionNode(GNode *node, int scope);

void addVariables(GNode *varDeclNode);
void variableIterator (GNode *node, gpointer data);
void variableHandler(symbol *, type_class);

void genCodeForStatementList (GNode *statementList);
void genCodeForStatement(GNode *statement);

void genCodeForExpression (GNode *expressionNode);

void genCodeForOperation (GNode *expressionNode);
//int getNodeType (GNode *node);

void generateProcReturn (procInfo *procedureInfo);



/****************************************************************
 * Functions to retrieve information from symbols and/or Nodes
 * ***************************************************************/
symbol *getSymbol (GNode *node);

procInfo *getBuiltinInfo (symbol *builtinSymbol);


/*****************************************************************
 * Code generation debug functions
 ****************************************************************/

void varaddressTableIterator (gpointer key, gpointer value, gpointer user_data) ;
void showVariableAddressTable();



/***********************************************************
 * Wrapper functions for ASC instructions
 **********************************************************/
void generateProcCall (procInfo *procedureInfo);
void generateGOTO (char const *label);

void pushConstantInt (int constant);
void pushConstantReal (double constant);


/*****************************************************************
 * Print generated code to output
 ******************************************************************/
void generateFormattedInstruction (char *instruction);

void generateComment (const char *comment);

void generateLabel (const char *labelName);

/*****************************************************************
 * Debug functions
 ****************************************************************/

void generateStackDump();
void generateTrace(int number);
#endif
