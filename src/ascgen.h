/**
 * Header file for Ascgen.c
 * generate ASC assembly based on syntax tree
 * Author: Daniel Chui
 */

#ifndef ASCGEN_H
#define ASCGEN_H

void genASCCode (GNode *tree, char *fileName);

void addVariables(GNode *varDeclNode);





/**
 * Print generated code to output
 */
void generateFormattedInstruction (char *instruction);

void generateComment (char *comment);

void generateLabel (char *labelName);
#endif
