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

//TODO: We need to store the number of words allocated on teh stack for us, so we can adjust and
//get rid of the,

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
    output = fopen (fileName, "w");
    //output = stdout;
  
 
  
  //start generating code
  
  node_type nodeType = getNiceType(tree); 
  if (nodeType != NT_PROGRAM ||g_node_n_children (tree) != 2 ) {
      //exit gracefully. The syntax tree is malformed
      printf ("head is of type %d, or does not have 2 children\n", nodeType);
      exit (1);
  }
  
  //TODO: print out a comment header stating the name of the file
  
  genCodeForFunctionNode (tree, scope);
  
  //TODO: Close the file
  
  
  
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


        //GOTO the actual instructions in program
        generateGOTO("main");
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
        generateLabel ("main");
        genCodeForStatementList (statements);
        
        //we need this for exit
        generateLabel ("end");
        //generateAdjust (number of vars allocated);
        generateFormattedInstruction ("STOP");
        
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
        generateLabel (procLabel);
        //printf ("Returned from generate label\n");
        //check which registers are free
        procInfo *procedureInfo = calloc (1, sizeof (procInfo));
        procedureInfo->procLabel = procLabel;
        procedureInfo->indexingRegister = getFirstFreeRegister();
        g_hash_table_insert (procedureLabelTable, procedureSymbol, procedureInfo);
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
        genProcReturn(procedureInfo);
        if (procDeclarations->children != NULL) {        
            //recursively call genCodeForFunction Node to generate for nested stuff
            
//             printf ("proc delcarations type: %d\n", getNiceType (procDeclarations));
//             printf ("Generating for other procedures now\n");
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
    //printf ("Number of children: %d\n", g_node_n_children(varDeclNode));
    if (getNiceType (varDeclNode) != NT_VAR_DECL_LIST) {
        //uh oh... boo boo
      //  printf ("Is not of type var decl list\n");
        //printf ("Is of  type: %d\n", getNiceType(varDeclNode));
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

void pushScalar(symbol *symb){
  int size = symb->desc.type_attr->desc.scalar->len;
  printf ("size: %d \n", size);

  char instruction[strlen("ADJUST") + sizeof(size) + 1];
  sprintf(instruction, "ADJUST %d", size);
  generateFormattedInstruction(instruction);
    
}


void pushArray(symbol *symb){
  //int size = symb->symbol_type->desc.type_attr->desc.array->size;
  int size = symb->desc.type_attr->desc.array->size;
  type_class varType = getTypeClass(symb->desc.type_attr->desc.array->obj_type);
  
  //printf("array tc: %d \n", getTypeClass(symb->symbol_type->desc.type_attr->desc.array->obj_type));

  // Handle 'special' cases of arrays (eg. arrays of arrays or arrays of records)
  printf("ARRAY SIZE: %d , TYPE: %d \n", size, varType);
  if(varType == TC_RECORD){
    //printf("pushing rec\n");

    int i;
    for(i=0; i<size; i++)
      pushRecord(symb->desc.type_attr->desc.array->obj_type);
    
  }
  else if(varType == TC_ARRAY){
    //printf("pushing array\n");
    int i;
    for(i=0; i<size; i++){

      pushArray(symb->desc.type_attr->desc.array->obj_type);
    }
  }
  // All other types of arrays
  else{
    char instruction[strlen("ADJUST") + sizeof(size) + 1];
    sprintf (instruction, "ADJUST %d", size); 
    generateFormattedInstruction (instruction);
  }
}
void pushRecord(symbol *symb){
  //printf("Record Name: %s \n", symb->name);
  //symbol *ss = symb->symbol_type;
  GPtrArray *recFields = symb->desc.type_attr->desc.record->field_list;
  //printf("record : %d \n", symb->symbol_type->desc.type_attr->type);
  symbol *recSymbol;
  
  // Iterate through elements and check sizes to make room on stack 
  //printf("elements: %d\n", recFields->len);
  int i;
  for(i=0; i<recFields->len; i++){
    recSymbol = g_ptr_array_index(recFields, i);
    variableHandler(recSymbol, getTypeClass(recSymbol));
  }
  
}
//TODO: Split variableIterator into an iterator fcn and a 'handler' fcn
//Function that is called for each var declaration
void variableIterator (GNode *node, gpointer data) {
    
    //int indexingRegister = *scope;
    int indexingRegister = 0;
    symbol *sym = getSymbol (node);
    type_class varType = getTypeClass (sym);

    variableHandler(getSymbol(node), getTypeClass(sym));
    
    //TODO: each variable needs to have its address added to the variableAddressTable
}

void variableHandler(symbol *symb, type_class varType){

  
  generateComment(symb->name);

  int indexingRegister = 0;
  varAddressStruct addressDescription = {indexingRegister, indexingRegister};
  g_hash_table_insert(variableAddressTable, symb, &addressDescription);

  //varAddressStruct *check_it = g_hash_table_lookup(variableAddressTable, symb);
  //printf("idxReg: %d, Offset: %d \n", check_it->indexingRegister, check_it->offset);
  
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
      //printf("size array: %d \n", symb->symbol_type->desc.type_attr->desc.array->size);
      pushArray(symb->symbol_type);
    }
    else if(varType == TC_RECORD){
      pushRecord(symb->symbol_type);
    }
    else if(varType == TC_SCALAR){
      // Handles like an array
      printf("scalar here \n");
      pushScalar(symb->symbol_type);
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
            //evaluate the expression
            
            symbol *varSymbol = (symbol *)statement->children;
            varAddressStruct *addressDescription = g_hash_table_lookup (variableAddressTable, varSymbol);
            
            //PUSH addr
            //PUSH (expr)
            //POPI
         
            break;
        }
        case NT_PROC_INVOK:
        {
            //TODO: How are parameters ordered? depending on how they're ordered
            //we may need to reorder them on the stack before calling...
            GNode *currentParamNode = statement->children->next;
            
            while (currentParamNode != NULL) {
                genCodeForExpression(currentParamNode);
            }
            GNode *symbolNode = statement->children;
//             printf ("We have a procedure invocation here\n");
            if (symbolNode == NULL) {
                printf ("Symbol node is null\n");
            }
            //printf ("type of symbolNode: %d\n", getNiceType (symbolNode));
            //TODO: Generate a go to to this procedure. h
            
            symbol *procSymbol = getSymbol (symbolNode);
            symbol *writelnSymbol = globalLookup ("writeln");
//             printf ("proc symbol address %p\n", procSymbol);
//             printf ("writeln address %p\n", writelnSymbol);
            
            procInfo *procedureInfo = (procInfo *) g_hash_table_lookup (procedureLabelTable, procSymbol);
            //printf ("Returned proc label: %s\n", procLabel);
//             printf ("procedure info returned\n");
            //generate CALL
            if (procedureInfo == NULL) {
                printf ("Returned proc info is null, must be a builtin\n");
                //look it up in builtins
                //TODO:
                procInfo *returnedInfo = getBuiltinInfo(procSymbol);
            }
            genProcCall (procedureInfo);
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
            //evaluate expressoin
            //ifZ GOTO blah blah
            //GOTO While beginnning
            break;
        }
        case NT_CONTINUE: {
            GNode *currentNode = statement->parent;
            while (getNiceType(currentNode) != NT_WHILE) {
                currentNode = currentNode->parent;
            }
            //TODO: get tehe label for this while
            //generateGOTO (whatever the daddy node is)
            break;
        }
        case NT_EXIT:
        {
            GNode *currentNode = statement->parent;
            while (getNiceType(currentNode) != NT_PROC_DECL) {
                currentNode = currentNode->parent;
            }
            //TODO: Get  the label for lowest scope funciton/procedure
            //generateGOTO to the END of this procedure
            //will need to generate label for all return jumps...
            break;
        }
                    
    }
        
}


/**
 * Return information needed to call a builtin here
 */
procInfo *getBuiltinInfo (symbol *builtinSymbol) {
    
}

void genCodeForExpression (GNode *expressionNode) {
    node_type exprType = getNiceType(expressionNode);
    switch (exprType) {
        case NT_VAR:
        {
            //TODO: implement
            break;
        }
        case NT_FUNC_INVOK:
        {
            //TODO: implement
            break;
        }
        case NT_CONST:
        {
            //TODO: Not actually sure what to do here..return it?
            break;
        }
        default:
        {
            genCodeForOperation (expressionNode);
            //call gen for operations
        }

    }
}

//TODO: Don't forget that each one of these has to have a recursive call to
//expression
void genCodeForOperation (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    if ((exprType >= NT_ISEQUAL) && (exprType <=NT_GREATERTHANEQUALS)) {
        genCodeForComparison (expressionNode);
    }
    else if ((exprType >=NT_AND) && (exprType <= NT_NOT)) {

        genCodeForLogical (expressionNode);
    }
    else if ((exprType >= NT_PLUS) && (exprType <= NT_MOD)) {
        genCodeForMath (expressionNode);
    }
    else if (exprType == NT_IDENTITY) {
        //uh...do nothing.
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
    node_type exprType = getNiceType (expressionNode);
    switch (exprType) {
        case NT_PLUS:
        {
            break;
        }
        case NT_MINUS:
        {
            break;
        }
        case NT_MULTIPLY:
        {
            break;
        }
        case NT_DIVIDE:
        {
            break;
        }
        case NT_DIV:
        {
            break;
        }
        case NT_MOD:
        {
            break;
        }
    }
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
void genProcCall (procInfo *procedureInfo) {
    char instruction [strlen ("CALL") + 2 + 256];
    //char *hm = procedureInfo->procLabel;
    //printf ("lala %s\n", hm);
    int lala = procedureInfo->indexingRegister;
    //printf ("Past indexing register");
    sprintf (instruction, "CALL %d %s", procedureInfo->indexingRegister, procedureInfo->procLabel);
    generateFormattedInstruction (instruction);
}

//TODO: We also need to adjust -x, get rid of our variables on teh stack
void genProcReturn (procInfo *procedureInfo) {
    
    char label [strlen (procedureInfo->procLabel) + strlen("end")];
    sprintf (label, "%send", procedureInfo->procLabel);
    generateLabel (label);
    char instruction[strlen ("RET") + 2];
    sprintf (instruction, "RET %d", procedureInfo->indexingRegister);
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

void generateLabel (const char *labelName) {
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
