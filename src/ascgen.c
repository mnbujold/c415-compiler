/**
 * Generate code in ASC assembly given a syntax tree 
 * Author: Daniel Chui
 */


#include <glib.h>
 
#include "ascgen.h"
FILE *output;
//somethign to store labels
//somethign to store registers we are using

void genASCCode (GNode *tree, char *fileName) {
  //open the file up here and any other stuff
  
  
  
  //start generating code
  node_type = tree->node_info->type; 
  if (node_type != NT_PROGRAM ||g_node_n_children (tree) != 2 ) {
      //exit gracefully. The syntax tree is malformed
  }
  GNode *declarations = tree->children;
  if (getNodeType (declarations) == NT_DECLS) {
      addVariables (declarations->children);
      //recursively call this function, but without file output
  }
  
  //do the declarations stuff here
  
  //if decl list is null, then do nothing
  
  //code for statements
  
  
  
}


void addVariables(GNode *varDeclNode) {
    //node must be of type NT_VAR_DECL_LIST
    if (getNodeType (varDeclNode) != NT_VAR_DECL_LIST) {
        //uh oh... boo boo
    }

    int numVariables = g_node_n_children (varDeclNode);
    if (numVariables == 0) {
        return;
    }
    generateComment ("Vars");
    g_node_children_foreach (varDeclNode, G_TRAVERSE_ALL, (GNodeForEachFunc) variableIterator, NULL);
    
    //while s
    

}

void variableIterator (GNode *node, gpointer data) {
    symbol *symbol = getSymbol (node);
    type_class varType = getTypeClass (symbol);
    if (varType == TC_INTEGER) {
        pushConstantInt (0);
        
    }
    else if (varType == TC_REAL) {
        pushConstantReal (0);
    }
    else if (varType == TC_BOOLEAN) {
        pushConstantInt (0);
    }
    else {
        
        DEBUG_PRINT (("NOt implemented yet"));
    }
    
    //check if they are array. if yes, then need to do somethign special
    
}


int getNodeType (GNode *node) {
    return node->node_info->type;
}

symbol *getSymbol (GNode *node) {
    if (getNodeType == NT_SYMBOL) {
        return node->node_info->symbol;
    }
    return NULL;
}

/**
 * Wrapper functions for ASC instructions
 */

void pushConstantInt (int constant) {
    //sprintf (
    char instruction[strlen ("CONSTI") + 11];
    sprintf (instruction, "CONSTI %d", constant);
    generateFormattedInstruction (instruction);
    //generateFormattedInstruction(
}

void pushConstantReal (double constant) {
    char instruction [strlen ("CONSTR") + 11];
    sprintf (instruction, "CONSTR %d", constant);
    generateFormattedInstruction (instruction);
    
}

/**
 * generateFormattedInstructino prints the instruction
 * given to the ASC file
 */
void generateFormattedInstruction(char *instruction) {


  fprintf (output, "\t %s\n", instruction);

}

void generateComment (char *comment) {
    fprintf (output, "#%s\n", comment);
}

void generateLabel (char *labelName) {
    fprintf (output, "%s:\n", labelName);
}