/**
 * Generate code in ASC assembly given a syntax tree 
 * Author: Daniel Chui, Mike Bujold
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
GHashTable *procedureLabelTable = NULL;

void genASCCode (GNode *tree, char *fileName) {
    DEBUG_PRINT(("!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
    DEBUG_PRINT (("Inside code generation\n"));
    DEBUG_PRINT(("!!!!!!!!!!!!!!!!!!!!!!!!!\n"));

    //Initialize our own tracking tools we will need for generating code

    labelStack = g_queue_new ();
    variableAddressTable = g_hash_table_new (g_direct_hash, g_direct_equal);
    procedureLabelTable = g_hash_table_new (g_direct_hash, g_direct_equal);
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
    //printf ("In gen code for function %d\n", getNiceType(node));
    
    //we'll consider program as a special case of a procedure declaration    
    if (getNiceType (node) == NT_PROGRAM) {
    
        //All we should have here is a goto to the main...becuase of order we generate code in
        generateLabel("start");
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
        //showVariableAddressTable();


        
        if (procDeclarations->children != NULL) {        
            //recursively call genCodeForFunction Node to generate for nested stuff
            //foreach function declaration
            
            printf ("proc delcarations type: %d\n", getNiceType (procDeclarations));
            printf ("Generating for other procedures now\n");
            GNode *procNode = procDeclarations->children;
            while (procNode != NULL) {
                genCodeForFunctionNode (procNode, scope+1);
                procNode = procNode->next;
                
            }
            
        }
        //TODO: We need to add a goto to the main, 
        genCodeForStatementList (statements);
        
    }
    else if (getNiceType(node) == NT_PROC_DECL) {
        //need to do this for the program
        DEBUG_PRINT (("Inside procedure node generation"));

        //symbol *procedureSymbol = (symbol *)getSymbol (node->children);
        //printf ("Procedure name: %s\n", procedureSymbol->name);

//        symbol *procedureSymbol = getSymbol (node->children);

        symbol *procedureSymbol = (symbol *)getSymbol (node->children);
        printf ("Procedure name: %s\n", procedureSymbol->name);


        const char *procName = ((symbol *) procedureSymbol)->name;
        printf ("Procedure name: %s\n", procName);
        
        //append scope so we don't get duplicate labels
        char *procLabel = calloc ((strlen ("procName") + 2), sizeof (char));
        sprintf (procLabel, "%s%d", procName, scope);
        printf ("PROCEDURE LABEL: %s\n", procLabel);
        generateLabel (procLabel);
        //printf ("Returned from generate label\n");
        //check which registers are free
        procInfo *procedureInfo = calloc (1, sizeof (procInfo));
        procedureInfo->procLabel = procLabel;
        printf ("42 procedure label: %s %p \n", procedureInfo->procLabel, procedureInfo->procLabel);
        procedureInfo->indexingRegister = getFirstFreeRegister();
        g_hash_table_insert (procedureLabelTable, procedureSymbol, procedureInfo);
        printf ("Done inserting\n");
        printf ("Address of %s: %p\n", procedureSymbol->name, procedureSymbol);
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
            
            printf ("proc delcarations type: %d\n", getNiceType (procDeclarations));
            printf ("Generating for other procedures now\n");
            GNode *procNode = procDeclarations->children;
            while (procNode != NULL) {
                genCodeForFunctionNode (procNode, scope+1);
                procNode = procNode->next;
                
            }
            
        }
        
    }
    else {
        printf ("Invalid node type: not program or proc decl: %d\n", getNiceType (node));
    }
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


int getFirstFreeRegister () {
    int i = 0;
    while (i < NUM_ASC_REGISTERS) {
        if (registers[i] == 0) {
            registers[i] = 1;
            return i;
        }
        i++;
    }
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

void getLength(symbol *symbol, type_class tc){
  // Get the size of the array
  if(tc == TC_STRING){
    printf("length: %d \n", symbol->desc.type_attr->desc.string->len);
    
  }

  else if(tc == TC_ARRAY){
  }

  else if(tc == TC_RECORD){

  }
}
void pushArray(symbol *symb){
  int size = symb->symbol_type->desc.type_attr->desc.array->size;
  char instruction[strlen("ADJUST") + sizeof(size) + 1];
  
  sprintf (instruction, "ADJUST -%d", size);
  
  generateFormattedInstruction (instruction);
}
void pushRecord(symbol *symb){
  printf("Record Name: %s \n", symb->name);
  GPtrArray *recFields = symb->symbol_type->desc.type_attr->desc.record->field_list;

  // Iterate through elements and check sizes to make room on stack 
  printf("elements: %d\n", recFields->len);
  int i;
  for(i=0; i<recFields->len; i++){
    symbol *recSymbol = g_ptr_array_index(recFields, i);
    printf("val: %s \n", recSymbol->name);
    printf("type: %d \n", getTypeClass(recSymbol));
//    variableHandler(recSymbol, 
    // Get the size of each element and push onto stack, maybe call variableIterator recursively?
    //variableIterator( );
  }
  
}
//TODO: Split variableIterator into an iterator fcn and a 'handler' fcn
//Function that is called for each var declaration
void variableIterator (GNode *node, gpointer data) {
    
    //int indexingRegister = *scope;
    int indexingRegister = 0;
    symbol *sym = getSymbol (node);
    type_class varType = getTypeClass (sym);

    //variableHandler(getSymbol(node), getTypeClass(sym));
    
    //TODO: each variable needs to have its address added to the variableAddressTable
    generateComment(sym->name);
    varAddressStruct addressDescription = {indexingRegister, indexingRegister};
    g_hash_table_insert (variableAddressTable, sym, &addressDescription);
    if (varType == TC_INTEGER) {
        pushConstantInt (0);
    }
    else if (varType == TC_REAL) {
        pushConstantReal (0);
    }
    else if (varType == TC_BOOLEAN) {
        pushConstantInt (0);
    }
    else if (varType == TC_STRING){
      printf("string: %s\n", sym->name);
        //TODO: Need to do for arrays
        printf ("error, this kind of variable not yet implemented\n");
        DEBUG_PRINT (("Not implemented yet"));
    }
    else if(varType == TC_ARRAY){
      pushArray(sym);
    }

    else if(varType == TC_RECORD){
      pushRecord(sym);
    }
    else
      // For debugging: Spit out the typeclass we don't yet handle here
      printf("Type: %d \n", varType);
    
    globalAddressCounter++; //do we need global address counter anymore?
}

void variableHandler(symbol *symb, type_class varType){

  
  generateComment(symb->name);

  int indexingRegister = 0;
  varAddressStruct addressDescription = {indexingRegister, indexingRegister};
  g_hash_table_insert(variableAddressTable, symb, &addressDescription);

   if (varType == TC_INTEGER) {
        pushConstantInt (0);
    }
    else if (varType == TC_REAL) {
        pushConstantReal (0);
    }
    else if (varType == TC_BOOLEAN) {
        pushConstantInt (0);
    }
    else if (varType == TC_STRING){
      printf("string: %s\n", symb->name);
        //TODO: Need to do for arrays
        printf ("error, this kind of variable not yet implemented\n");
        DEBUG_PRINT (("Not implemented yet"));
    }
    else if(varType == TC_ARRAY){
      pushArray(symb);
    }

    else if(varType == TC_RECORD){
      pushRecord(symb);
    }
    else
      // For debugging: Spit out the typeclass we don't yet handle here
      printf("Type: %d \n", varType);
    
    globalAddressCounter++; //do we need global address counter anymore?
}
// int getNiceType (GNode *node) {
//     return node->node_info->type;
// }


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
            //TODO: Evaluate the procedure invocation paramters first..
            
            GNode *symbolNode = statement->children;
            printf ("We have a procedure invocation here\n");
            if (symbolNode == NULL) {
                printf ("Symbol node is null\n");
            }
            //printf ("type of symbolNode: %d\n", getNiceType (symbolNode));
            //TODO: Generate a go to to this procedure. 
            symbol *procSymbol = getSymbol (symbolNode);
            symbol *writelnSymbol = globalLookup ("writeln");
            printf ("proc symbol address %p\n", procSymbol);
            printf ("writeln address %p\n", writelnSymbol);
            
            procInfo *procedureInfo = (procInfo *) g_hash_table_lookup (procedureLabelTable, procSymbol);
            //printf ("Returned proc label: %s\n", procLabel);
            printf ("procedure info returned\n");
            //generate CALL
            if (procedureInfo == NULL) {
                printf ("Returned proc info is null, must be a builtin\n");
                //look it up in builtins
                if (procSymbol->name == "writeln") {
                    printf ("Is writeln");
                }
            }
            generateProcCall (procedureInfo);
            //generateGOTO (procLabel);
            //generate Label right after
            //global lookup on symbol table?
            //if writeln or readln, we have to do special stuff
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
            //while node->parent != NT_WHILE, node = node->pparent
            //get tehe label for this while
            //generateGOTO (whatever the daddy node is)
            break;
        }
        case NT_EXIT:
        {
            break;
        }
                    
    }
        
}

void genCodeForExpression (GNode *expressionNode) {
    node_type exprType = getNiceType(expressionNode);
    switch (exprType) {
        case NT_VAR:
        {
            //TODO: implement
        }
        case NT_FUNC_INVOK:
        {
            //TODO: implement
        }
        default:
        {
            //call gen for operations
        }

    }
}

void genCodeForOperation (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    if ((exprType >= NT_ISEQUAL) && (exprType <=NT_GREATERTHANEQUALS)) {
        //gen code for comparison
    }
    else if ((exprType >=NT_AND) && (exprType <= NT_NOT)) {
        //gen code for logical operation
    }
    else if ((exprType >= NT_PLUS) && (exprType <= NT_MOD)) {
        
       //gen code for math operations
    }
    else if (exprType == NT_IDENTITY) {
    }
    else if (exprType == NT_INVERSION) {
    }
    else {
        printf ("Error. Tried to gernerate code for an operation when it was not an operation\n");
    }
    
}

void genCodeForComparison (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    switch (exprType) {
        case NT_ISEQUAL:
        {
        }
        case NT_NOTEQUAL:
        {
        }
        case NT_LESSTHAN:
        {
        }
        case NT_GREATERTHAN:
        {
        }
        case NT_LESSTHANEQUALS:
        {
        }
        case NT_GREATERTHANEQUALS:
        {
        }
    }
    
}

void genCodeForLogical (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    switch (exprType) {
        case NT_AND:
        {
        }
        case NT_OR:
        {
        }
        case NT_NOT:
        {
        }
    }
}

void genCodeForMath (GNode *expressionNode) {
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
 * generates a CALL instruction to the relevant procedure stored in procedureInfo
 */
void generateProcCall (procInfo *procedureInfo) {
    char instruction [strlen ("CALL") + 2 + 256];
    char *hm = procedureInfo->procLabel;
    printf ("lala %s\n", hm);
    int lala = procedureInfo->indexingRegister;
    printf ("Past indexing register");
    sprintf (instruction, "CALL %d %s", procedureInfo->indexingRegister, procedureInfo->procLabel);
    generateFormattedInstruction (instruction);
}
/**
 * generateFormattedInstruction prints the instruction
 * given to the ASC file
 */
void generateFormattedInstruction(char *instruction) {


  fprintf (output, "\tINSTRUCTION: %s\n", instruction);

}

void generateComment (const char *comment) {
    fprintf (output, "#%s\n", comment);
}

void generateLabel (const char *labelName) {
    fprintf (output, "LABEL: %s\n", labelName);
    printf ("Done...\n");
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

