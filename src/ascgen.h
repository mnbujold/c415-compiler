/**
 * Header file for Ascgen.c
 * generate ASC assembly based on syntax tree
 * Author: Daniel Chui
 */

#ifndef ASCGEN_H
#define ASCGEN_H



#define NUM_ASC_REGISTERS 16

void genASCCode (GNode *tree, char *fileName);


/**
 * Generate code for a function declaration, procedure declaration, or for program
 * Note that for the purposes of this code, a program is considered a procedure
 */
void genCodeForFunctionNode(GNode *node);

void addVariables(GNode *varDeclNode);
void variableIterator (GNode *node, gpointer data);

//int getNodeType (GNode *node);
symbol *getSymbol (GNode *node);

void pushConstantInt (int constant);
void pushConstantReal (double constant);

/**
 * Print generated code to output
 */
void generateFormattedInstruction (char *instruction);

void generateComment (char *comment);

void generateLabel (char *labelName);

/**
 * Debug functions
 */

void generateStackDump();
void generateTrace(int number);
#endif