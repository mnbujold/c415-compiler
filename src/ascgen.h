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
    //how many words we need for vars for this proc
    int numVarWords;
    //how many labels within the statements it currently has
    int numLabels;
};

typedef struct proc_info_struct procInfo;


//TODO: probably need to implement. For now, going to try cheating
//We only need this becuase of continue and exit statements


//right now, we only need this for while loops

enum structured_statement_type {
    ST_IF,                  //0
    ST_IF_ELSE,             //1
    ST_WHILE                //2
};
typedef enum structured_statement_type structType;
struct structured_info_struct {
    structType type;
    char *beginLabel;
    char *endLabel;
    
};
typedef struct structured_info_struct structInfo;


/****************************************************************
 * Functions to generate code based on syntax tree
 ****************************************************************/
void genASCCode (GNode *tree, char *fileName);

GNode *getFirstOperationParent (GNode *expressionNode);

GNode *getFirstParent (GNode *currentNode, node_type parentTypeLowerBound, node_type parentTypeUpperBound) ;

void genCodeForFunctionNode(GNode *node, int scope);


void addVariables(GNode *varDeclNode, int indexingRegister, int offset, procInfo *procedureInfo); 
int variableIterator (GNode *varNode, int indexingRegister, int offset);
int variableHandler(symbol *symb, type_class varType, varAddressStruct *addDescription);

void genCodeForStatementList (GNode *statementList);
void genCodeForStatement(GNode *statement);

void genCodeForExpression (GNode *expressionNode);

void genCodeForOperation (GNode *expressionNode);
//int getNodeType (GNode *node);
void genVarAdjust (int value);





/****************************************************************
 * Functions to retrieve information from symbols and/or Nodes
 * ***************************************************************/
symbol *getSymbol (GNode *node);

void genBuiltinCall (symbol *builtinSymbol);
symbol *getFirstProcParent(GNode *node);
int getExpressionValue(GNode *);

/*****************************************************************
 * Code generation debug functions
 ****************************************************************/

void varaddressTableIterator (gpointer key, gpointer value, gpointer user_data) ;
void showVariableAddressTable();



/***********************************************************
 * Wrapper functions for sequences of atomic ASC instructions
 **********************************************************/
void genCodeForIntComparison (GNode *expressionNode);
void genCodeForRealComparison (GNode *expressionNode);
void genCodeForLogical (GNode *expressionNode);
void genCodeForIntMath (GNode *expressionNode); 
void genCodeForRealMath (GNode *expressionNode);

void genProcCall (procInfo *procedureInfo);
void genProcReturn (procInfo *procedureInfo);


void genBranch (char const *label);
void genGOTO (char const *label);
void genVarAssign (varAddressStruct *addressDescription, int subscript);
void genVarParamAssign (varAddressStruct *addressDescription);
void genVarAccess (varAddressStruct *addressDescription);
void genVarParam (varAddressStruct *addressDescription);

void pushConstantInt (int constant);
void pushConstantReal (double constant);
char *genProcLabel (procInfo *procedureInfo);

int pushRecord(symbol *);
int pushArray(symbol *);
int pushScalar(symbol *);

void genCodeForWrite(GNode *paramNode, int ln);


void genCodeForRead (GNode *paramNode, int ln);

void addLabel (char const *labelName);
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
