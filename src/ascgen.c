/**
 * Generate code in ASC assembly given a syntax tree 
 * Author: Daniel Chui
 */


#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symbol.h" 

#include "debug.h"
#include "syntaxtree.h"
 
#include "builtinasc.h"
#include "ascgen.h"


FILE *output;
//somethign to store labels

//registers stores registers we are using. 
//if it is set to 0, then is free, if set to 1, being used
int *registers;
int globalAddressCounter = 0;

//store the current scope. we will need this for labels
int scope;
GQueue* labelStack = NULL;
GHashTable *variableAddressTable = NULL;

void genASCCode (GNode *tree, char *fileName) {

    //Initialize our own tracking tools we will need for generating code

    labelStack = g_queue_new ();
    variableAddressTable = g_hash_table_new (g_direct_hash, g_direct_equal);
    registers = calloc (NUM_ASC_REGISTERS, sizeof (int));
    scope = 0;
    
    
  //TODO:   
  //open the file up here and any other stuff
    
    output = stdout;
  
 
  
  //start generating code
  
  node_type nodeType = getNodeType(tree); 
  if (nodeType != NT_PROGRAM ||g_node_n_children (tree) != 2 ) {
      //exit gracefully. The syntax tree is malformed
      printf ("head is of type %d, or does nto have 2 children\n", nodeType);
  }
  
  genCodeForFunctionNode (tree, scope);
  
  
  
}

/**
 * Node must be of type PROGRAM, PROC, or FUNC
 */
void genCodeForFunctionNode(GNode *node, int scope) {
    printf ("In gen code for function %d\n", getNodeType(node));
    
    
    if (getNodeType (node) != NT_PROGRAM && getNodeType(node) != NT_PROC_DECL) {
        //invalid node type, exit
        printf ("Invalid node type: not program or proc decl: %d\n", getNodeType (node));
    }

    //need to do this for the program
    
    GNode *declarations = node->children;
    if (getNodeType (declarations) == NT_DECLS) {
        
        //TODO: The syntax tree for proc decl and program are different!
        //Consider the 2 cases
        //potentially unsafe, as children returns first child
        //if the first child is not the var_decl_list for some reason
        //this will not work
        
        //TODO: Actually, we need to add the parameters first
        
        addVariables (declarations->children); //pass in the var_decl_list
        
        
        
        
        GNode *procedureDeclarations = declarations->children->next;
        
        //get the number of children
        
        
        
        
        //recursively call genCodeForFunction Node to generate for nested stuff
        //foreach function declaration
        
        //TODO: Get the procedure declarateions, iterate through, and for each one, generate
        //code
        GNode *funcNode = NULL;
        genCodeForFunctionNode (funcNode, scope+1);
        
        //TODO: WHAT ORDER do I generate the code in!?
    }
    
    //do the declarations stuff here
    
    //if decl list is null, then do nothing
    //TODO: Generate teh code for the statements now...
    //code for statements
}





/**
 * Given a var declaration node, generate space on the stack for it.
 */
void addVariables(GNode *varDeclNode) {
    //node must be of type NT_VAR_DECL_LIST
    printf ("Number of children: %d\n", g_node_n_children(varDeclNode));
    if (getNodeType (varDeclNode) != NT_VAR_DECL_LIST) {
        //uh oh... boo boo
        printf ("Is not of type var decl list\n");
        printf ("Is of  type: %d\n", getNodeType(varDeclNode));
    }

    int numVariables = g_node_n_children (varDeclNode);
    if (numVariables == 0) {
        return;
    }
    generateComment ("Vars");
    //maybe print out stack trace here to make sure they are there?
    g_node_children_foreach (varDeclNode, G_TRAVERSE_ALL, (GNodeForeachFunc) variableIterator, NULL);
    generateStackDump();
    //while s
    

}


//Function that is called for each var declaration
void variableIterator (GNode *node, gpointer data) {
    symbol *symbol = getSymbol (node);
    type_class varType = getTypeClass (symbol);
    if (varType == TC_INTEGER) {
        printf ("Is an integer\n");
        pushConstantInt (0);
        
    }
    else if (varType == TC_REAL) {
        pushConstantReal (0);
    }
    else if (varType == TC_BOOLEAN) {
        pushConstantInt (0);
    }
    else {
        //TODO: Need to do for arrays
        printf ("error, this kind of variable not yet implemented\n");
        DEBUG_PRINT (("Not implemented yet"));
    }
    
    //check if they are array. if yes, then need to do somethign special
    
}


// int getNodeType (GNode *node) {
//     return node->node_info->type;
// }

void outputProcedureStatements () {
    
    
    
}

symbol *getSymbol (GNode *node) {
    
    if (getNodeType(node) == NT_SYMBOL) {
        return ((node_info*)node->data)->symbol;
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
    sprintf (instruction, "CONSTR %f", constant);
    generateFormattedInstruction (instruction);
    
}

/**
 * generateFormattedInstruction prints the instruction
 * given to the ASC file
 */
void generateFormattedInstruction(char *instruction) {


  fprintf (output, "\t%s\n", instruction);

}

void generateComment (char *comment) {
    fprintf (output, "#%s\n", comment);
}

void generateLabel (char *labelName) {
    fprintf (output, "%s\n", labelName);
}

/**
 * Debugging functions for ASC
 */

void generateStackDump() {
#if DEBUG
    fprintf(output, "\t!D\n");
#endif
}

void generateTrace(int number) {
#if DEBUG
    fprintf(output, "\t!T=%d\n", number);
#endif
}