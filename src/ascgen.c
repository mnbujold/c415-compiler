/**
 * Generate code in ASC assembly given a syntax tree 
 * Author: Daniel Chui
 */

 FILE *output;
#include <glib.h>
 
#include "ascgen.h"


void genASCCode (GNode *tree, char *fileName) {
  //open the file up here and any other stuff
  
  
  
  //start generating code
  node_type = tree->node_info->type; 
  if (node_type != NT_PROGRAM ||g_node_n_children (tree) != 2 ) {
      //exit gracefully. The syntax tree is malformed
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
        generateFormattedInstruction ("CONSTI 0");
        
    }
    else if (varType == TC_REAL) {
        generateFormattedInstruction("CONSTR 0");
    }
    else if (varType == TC_BOOLEAN) {
        generateFormattedInstruction("CONSTI 0");
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
    //generateFormattedInstruction(
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