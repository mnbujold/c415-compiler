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
    DEBUG_PRINT(("!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
    DEBUG_PRINT (("Inside code generation\n"));
    DEBUG_PRINT(("!!!!!!!!!!!!!!!!!!!!!!!!!\n"));

    //Initialize our own tracking tools we will need for generating code

    labelStack = g_queue_new ();
    variableAddressTable = g_hash_table_new (g_direct_hash, g_direct_equal);
    registers = calloc (NUM_ASC_REGISTERS, sizeof (int));
    scope = 0;
    
    
  //TODO:   
  //open the file up here and any other stuff
    
    output = stdout;
  
 
  
  //start generating code
  
  node_type nodeType = getNiceType(tree); 
  if (nodeType != NT_PROGRAM ||g_node_n_children (tree) != 2 ) {
      //exit gracefully. The syntax tree is malformed
      printf ("head is of type %d, or does not have 2 children\n", nodeType);
      exit (1);
  }
  
  //TODO: print out a comment header stating the name of the file
  
  genCodeForFunctionNode (tree, scope);
  
  
  
}

/**
 * Node must be of type PROGRAM, PROC, or FUNC
 */
void genCodeForFunctionNode(GNode *node, int scope) {
    printf ("In gen code for function %d\n", getNiceType(node));
    
    //we'll consider program as a special case of a procedure declaration    
    if (getNiceType (node) == NT_PROGRAM) {
    //need to do this for the program
    
        generateLabel("main");
        DEBUG_PRINT (("Inside program node generation"));
        GNode *declarations = node->children;
        GNode *statements = node->children->next;
        
        GNode *varDeclarationsList = declarations->children;
        GNode *procDeclarations = declarations->children->next;
        if (getNiceType (declarations) == NT_DECLS) {
            
           varDeclarationsList = declarations->children;
           procDeclarations = declarations->children->next;
            addVariables (varDeclarationsList); //pass in the var_decl_list
            DEBUG_PRINT(("Type of procDeclarations: %d", getNiceType(procDeclarations)));
        }
        DEBUG_PRINT (("TYpe of statements: %d", getNiceType(statements)));
        //do the declarations stuff here
        showVariableAddressTable();

        genCodeForStatementList (statements);
        
        if (procDeclarations->children != NULL) {        
            //recursively call genCodeForFunction Node to generate for nested stuff
            //foreach function declaration
            
            //TODO: Get the procedure declarateions, iterate through, and for each one, generate
            //code
            printf ("proc delcarations type: %d\n", getNiceType (procDeclarations));
            printf ("Generating for other procedures now\n");
            GNode *procNode = procDeclarations->children;
            while (procNode != NULL) {
                genCodeForFunctionNode (procNode, scope+1);
                procNode = procNode->next;
                
            }
            
        }
        
    }
    else if (getNiceType(node) == NT_PROC_DECL) {
        //need to do this for the program
        DEBUG_PRINT (("Inside procedure node generation"));

        GNode *procedureSymbol = getSymbol (node->children);
        printf ("Procedure name: %s\n", ((symbol *)procedureSymbol)->name);
        GNode *declarations = node->children->next;
        GNode *statements = declarations->next;
        
        GNode *varDeclarationsList = declarations->children;
        GNode *procDeclarations = declarations->children->next;
        
        //TODO: Actually, we need to add the parameters first
        //go through the procedureSymbol, and get the parameters and
        //allocate space for them
        if (getNiceType (declarations) == NT_DECLS) {
            varDeclarationsList = declarations->children;
            procDeclarations = declarations->children->next;
            addVariables (varDeclarationsList); //pass in the var_decl_list
            //DEBUG_PRINT(("Type of procDeclarations: %d", getNiceType(procDeclarations)));
        }
        DEBUG_PRINT (("TYpe of statements: %d", getNiceType(statements)));
        //do the declarations stuff here
        //showVariableAddressTable();
        //if decl list is null, then do nothing
        //TODO: Generate teh code for the statements now...
        //code for statements
        genCodeForStatementList (statements);
        
        if (procDeclarations->children != NULL) {        
            //recursively call genCodeForFunction Node to generate for nested stuff
            //foreach function declaration
            
            //TODO: Get the procedure declarateions, iterate through, and for each one, generate
            //code
            printf ("No other procedure declarations\n");
            GNode *funcNode = NULL;
            genCodeForFunctionNode (funcNode, scope+1);
            
        }
        
    }
    else {
        printf ("Invalid node type: not program or proc decl: %d\n", getNiceType (node));
    }
    //TODO: WHAT ORDER do I generate the code in!?
}

genCodeForFunctionCall() {
    //foreach param, 
    //int numParams = getParams
    int numParams = 1;
    int address = 0;
    int callRegister = 1; //getCallRegister 
    char *label = "functionLabel";
    //generateFormattedInstruction ("PUSH %d", address);
    //generateFormattedInstruction ("CALL %d, %s", callRegister, label);
    
    //TODO: THIS IS WRONG, if we have more than 1 return value
    //for example, a var parameter?
    //generateFormattedInstruction ("ADJUST %d", 0-numParams + 1);

}







/**
 * Given a var declaration node, generate space on the stack for it.
 */
void addVariables(GNode *varDeclNode) {
    //node must be of type NT_VAR_DECL_LIST
    printf ("Number of children: %d\n", g_node_n_children(varDeclNode));
    if (getNiceType (varDeclNode) != NT_VAR_DECL_LIST) {
        //uh oh... boo boo
        printf ("Is not of type var decl list\n");
        printf ("Is of  type: %d\n", getNiceType(varDeclNode));
    }

    int numVariables = g_node_n_children (varDeclNode);
    if (numVariables == 0) {
        return;
    }
    generateComment ("Vars");
    //maybe print out stack trace here to make sure they are there?
    g_node_children_foreach (varDeclNode, G_TRAVERSE_ALL, (GNodeForeachFunc) variableIterator, &scope);
    generateStackDump();
    //while s
    

}


//Function that is called for each var declaration
void variableIterator (GNode *node, gpointer data) {
    
    //int indexingRegister = *scope;
    int indexingRegister = 0;
    symbol *symbol = getSymbol (node);
    type_class varType = getTypeClass (symbol);
    
    //TODO: each variable needs to have its address added to the variableAddressTable
    generateComment(symbol->name);
    varAddressStruct addressDescription = {indexingRegister, indexingRegister};
    g_hash_table_insert (variableAddressTable, symbol, &addressDescription);
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
    
    globalAddressCounter++; //do we need global address counter anymore?
}




/**
 * Generate code for a GNode of type statement_list
 */
void genCodeForStatementList (GNode *statementList) {
    
    
    if (getNiceType(statementList) != NT_STAT_LIST) {
        printf ("Node is not of type statement\n");
        
    }
    if  (statementList->children == NULL) {
        return;
    }
    GNode *statement = statementList->children;
    while (statement != NULL) {
        genCodeForStatement (statement->children);
        statement = statement->next;
    }
    
    
    
    
}


//TODO: Finish this

void genCodeForStatement(GNode *statement) {
    node_type statementType = getNiceType(statement);
    switch (statementType) {
        case NT_ASSIGNMENT:
        {
            break;
        }
        case NT_PROC_INVOK:
        {
            printf ("We have a procedure invocation here\n");
            //TODO: Generate a go to to this procedure. 
            symbol *procSymbol = getSymbol (statement);
            //const char *procName = procSymbol->name;
            //look up this name label
            break;
        }   
        case NT_IF:
        {
            break;
        }
        case NT_IF_ELSE:
        {
            break;
        }
        case NT_WHILE: {
            break;
        }
        case NT_CONTINUE: {
            break;
        }
        case NT_EXIT:
        {
            break;
        }
                    
    }
        
}
void outputProcedureStatements () {
    
    
    
}


/**
 * Generates a string on the stack, with the first character at the lowest
 * index and the last character of the highest index
 */
void generateString () {
}
symbol *getSymbol (GNode *node) {
    
    if (getNiceType(node) == NT_SYMBOL) {
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

void generateGOTO (char const *label) {
    
    //TODO: find out if there is max label size in ASC
    char instruction [strlen ("GOTO") + 256];
    sprintf (instruction, "GOTO %s", label);
    generateFormattedInstruction (instruction);
}

/**
 * generateFormattedInstruction prints the instruction
 * given to the ASC file
 */
void generateFormattedInstruction(char *instruction) {


  fprintf (output, "\t%s\n", instruction);

}

void generateComment (const char *comment) {
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


/**
 * My own debugging functions for code generation
 */

void showVariableAddressTable() {
    printf ("Variable address table values: \n");
    g_hash_table_foreach (variableAddressTable, (GHFunc)varaddressTableIterator , NULL);
    
}

void varaddressTableIterator (gpointer key, gpointer value, gpointer user_data) {

    printf ("Variable name: %s\n", ((symbol *)key)->name);
    printf ("Variable type: %d\n", getTypeClass((symbol *)key));
    printf ("indexing register: %d\n", ((varAddressStruct *)value)->indexingRegister);
    printf ("Offset: %d\n", ((varAddressStruct *)value)->offset);
    printf ("*****************************\n");
    
}