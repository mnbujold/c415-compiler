/**
 * Header file for Ascgen.c
 * generate ASC assembly based on syntax tree
 * Author: Daniel Chui
 */

#ifndef ASCGEN_H
#define ASCGEN_H



#define NUM_ASC_REGISTERS 16


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

void genASCCode (GNode *tree, char *fileName);


/**
 * Generate code for a function declaration, procedure declaration, or for program
 * Note that for the purposes of this code, a program is considered a procedure
 */
void genCodeForFunctionNode(GNode *node, int scope);

void addVariables(GNode *varDeclNode);
void variableIterator (GNode *node, gpointer data);

void genCodeForStatementList (GNode *statementList);
void genCodeForStatement(GNode *statement);
//int getNodeType (GNode *node);
symbol *getSymbol (GNode *node);


void generateProcCall (procInfo *procedureInfo);
void generateGOTO (char const *label);

void pushConstantInt (int constant);
void pushConstantReal (double constant);

/**
 * Print generated code to output
 * 
 * 
 * 
 */
void generateFormattedInstruction (char *instruction);

void generateComment (const char *comment);

void generateLabel (const char *labelName);

/**
 * Debug functions
 */

void generateStackDump();
void generateTrace(int number);

/**
 * Code generation debug functions
 */

void varaddressTableIterator (gpointer key, gpointer value, gpointer user_data) ;
void showVariableAddressTable();
#endif
