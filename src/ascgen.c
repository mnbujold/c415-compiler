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

//registers stores registers we are using. 
//if it is set to 0, then is free, if set to 1, being used
int *registers;
int globalAddressCounter = 0;

//TODO: We need to store the number of words allocated on teh stack for us, so we can adjust and
//get rid of the,

//store the current scope. we will need this for labels
int scope;
GHashTable *labelTable = NULL;
GHashTable *variableAddressTable = NULL;
GHashTable *procedureInfoTable = NULL;

procInfo *mainProcInfo;
//TODO: Will need to refer to the main label somehow
void genASCCode (GNode *tree, char *fileName) {
    DEBUG_PRINT(("!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
    DEBUG_PRINT (("Inside code generation\n"));
    DEBUG_PRINT(("!!!!!!!!!!!!!!!!!!!!!!!!!\n"));

    //Initialize our own tracking tools we will need for generating code

    labelTable = g_hash_table_new (g_direct_hash, g_direct_equal);
    variableAddressTable = g_hash_table_new (g_direct_hash, g_direct_equal);
    procedureInfoTable = g_hash_table_new (g_direct_hash, g_direct_equal);
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
        
        procInfo *procedureInfo = calloc (1, sizeof (procInfo));
        procedureInfo->procLabel = "main";
        procedureInfo->indexingRegister = -1;
        mainProcInfo = procedureInfo;
        //store this globally. If all else fails we jump to this symbol
        //g_hash_table_insert (procedureInfoTable, procedureSymbol, procedureInfo);
        GNode *varDeclarationsList = declarations->children;
        GNode *procDeclarations = declarations->children->next;
        if (getNiceType (declarations) == NT_DECLS) {
            
           varDeclarationsList = declarations->children;
           procDeclarations = declarations->children->next;
            addVariables (varDeclarationsList, -1, 0); //pass in the var_decl_list
            DEBUG_PRINT(("Type of procDeclarations: %d", getNiceType(procDeclarations)));
        }
        DEBUG_PRINT (("TYpe of statements: %d", getNiceType(statements)));
        //do the declarations stuff here
        //showVariableAddressTable();


        //GOTO the actual instructions in program
        genGOTO("main");
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
        generateLabel ("mainend");
        generateStackDump();
        //generateAdjust (number of vars allocated);
        generateStackDump();
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
        int callingRegister = getFirstFreeRegister();
        procedureInfo->indexingRegister = callingRegister;
        g_hash_table_insert (procedureInfoTable, procedureSymbol, procedureInfo);
        GNode *declarations = node->children->next;
        GNode *statements = declarations->next;
        
        GNode *varDeclarationsList = declarations->children;
        GNode *procDeclarations = declarations->children->next;
        
        //TODO: Actually, we need to add the parameters first
        //go through the procedureSymbol, and get the parameters and
        //allocate space for them
        //TODO: Get the count of params
        if (getNiceType (declarations) == NT_DECLS) {
            varDeclarationsList = declarations->children;
            procDeclarations = declarations->children->next;
            addVariables (varDeclarationsList, callingRegister, 0); //pass in the var_decl_list
            //DEBUG_PRINT(("Type of procDeclarations: %d", getNiceType(procDeclarations)));
        }
        DEBUG_PRINT (("TYpe of statements: %d", getNiceType(statements)));
        //do the declarations stuff here
        //showVariableAddressTable();

        genCodeForStatementList (statements);
        //TODO: remember where stack counter is at since we got control
        //Adjust stack counter so that stack is empty, except for the return values
        
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
void addVariables(GNode *varDeclNode, int indexingRegister, int offset) {
    //node must be of type NT_VAR_DECL_LIST
    //printf ("Number of children: %d\n", g_node_n_children(varDeclNode));
    printf ("In add variables. Value of register: %d, offset: %d\n", indexingRegister, offset);
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
    //g_node_children_foreach (varDeclNode, G_TRAVERSE_ALL, (GNodeForeachFunc) variableIterator, &scope);
    GNode *varNode = varDeclNode->children;
    while (varNode != NULL) {
      variableIterator (varNode, indexingRegister, offset);
      offset++;
      varNode = varNode->next;
    }
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
    //TODO: Broken;
    varAddressStruct *lala;
    variableHandler(recSymbol, getTypeClass(recSymbol), lala);
  }
  
}
//TODO: Split variableIterator into an iterator fcn and a 'handler' fcn
//Function that is called for each var declaration
void variableIterator (GNode *varNode, int indexingRegister, int offset) {
    
    //int indexingRegister = *scope;
    //int indexingRegister = 0;
    symbol *sym = getSymbol (varNode);
    printf ("Adding this symbol: %s\n", sym->name);
    printf ("The address: %p\n", sym);
    printf ("Register: %d Offset: %d\n", indexingRegister, offset);
    type_class varType = getTypeClass (sym);

    varAddressStruct *addressDescription = calloc (1, sizeof (varAddressStruct));
    addressDescription->indexingRegister = indexingRegister;
    addressDescription->offset = offset;
    variableHandler(sym, getTypeClass(sym), addressDescription);
    
    //TODO: each variable needs to have its address added to the variableAddressTable
}

void variableHandler(symbol *symb, type_class varType, varAddressStruct *addDescription){

  printf ("Inside variable handler\n");
  printf ("Address inside variable handler: %p\n", symb);
  generateComment(symb->name);

  g_hash_table_insert(variableAddressTable, symb, addDescription);
  if (g_hash_table_lookup (variableAddressTable, symb) == NULL) {
    printf ("WHAT HTE HECK. empty when we just inserted...?\n");
  }

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
            
            //remember there is a level of indrirection here
            GNode *varNode = statement->children;
            node_type varType = getNiceType (varNode->children);
            if (varType == NT_SYMBOL) {
              GNode *symbolNode = (statement->children)->children;
              printf ("type of var's first child: %d\n", getNiceType (symbolNode));

              symbol *varSymbol = getSymbol (symbolNode);
              printf ("symbol name: %s\n", varSymbol->name);
              printf ("Symbol address: %p\n", varSymbol);
              varAddressStruct *addressDescription = g_hash_table_lookup (variableAddressTable, varSymbol);
              GNode *expressionNode = statement->children->next;
              genCodeForExpression (expressionNode);
              printf ("Address of address description from hash: %p\n", addressDescription);
              genVarAssign (addressDescription);
            }
            else if (varType== NT_ARRAY_ACCESS) {
              //TODO: Implement
            }
            else if (varType ==NT_RECORD_ACCESS) {
              //TODO: Implement
            }
            else {
            }

         
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
            
            procInfo *procedureInfo = (procInfo *) g_hash_table_lookup (procedureInfoTable, procSymbol);
            //printf ("Returned proc label: %s\n", procLabel);
//             printf ("procedure info returned\n");
            //generate CALL
            if (procedureInfo == NULL) {
                printf ("Returned proc info is null, must be a builtin\n");
                //look it up in builtins
                //TODO: look up in builtins table...
                procInfo *returnedInfo = getBuiltinInfo(procSymbol);
            }
            genProcCall (procedureInfo);
            //genGOTO (procLabel);
            //generate Label right after
            //global lookup on symbol table?
            //if writeln or readln, we have to do special stuff
            //const char *procName = procSymbol->name;
            //look up this name label
            break;
        }   
        case NT_IF:
        {
          printf ("Inside an if conditional statement\n");
          GNode *ifExpression = statement->children;
          GNode *ifStatementList = ifExpression->next;
          printf ("successfully got the nodes of if\n");
          genCodeForExpression (ifExpression);
          printf ("successfully generated code for conditional\n");
          //GET THE label, append if, 
          //IFNZ FUNCTIONIFNUMBERXEND
          ////generateLabel functionNameIfNumberX
          symbol *procSymbol = getFirstProcParent(statement);
          printf ("Address of proc symbol: %p\n", procSymbol);
          procInfo *procedureInfo;
          if (procSymbol == NULL) {
            printf ("Could not find parent proc symbol\n");
            procedureInfo = mainProcInfo;
          }
          else {
            procedureInfo = g_hash_table_lookup (procedureInfoTable, procSymbol);
          }
          char *label = genProcLabel (procedureInfo);
          char instruction [strlen (label) + strlen ("IFZ")];
          sprintf (instruction, "IFZ %s", label);
          generateFormattedInstruction (instruction);
          // sprintf (instruction, "IFZ %s", label);
          genCodeForStatementList (ifStatementList);
          generateLabel (label);
          //don't forget to add so the next guy knows how many labels
          
          //generateLabel (functionNameIfNumberXEnd)
          //TODO: Implement
            break;
        }
        case NT_IF_ELSE:
        {
            GNode *ifExpression = statement->children;
            GNode *takenStatementList = ifExpression->next;
            GNode *elseStatementList = takenStatementList->next;
            symbol *procSymbol = getFirstProcParent (statement);
            procInfo *procedureInfo;
            if (procSymbol == NULL) {
                procedureInfo = mainProcInfo;
            }
            else {
                procedureInfo = g_hash_table_lookup (procedureInfoTable, procSymbol);
            }
            
            char *label = genProcLabel (procedureInfo);
            char elseLabel [strlen (label) + strlen ("else")];
            char endLabel [strlen (label) + strlen ("ifend")];
            sprintf (elseLabel, "%selse", label);
            sprintf (endLabel, "%sifend", label);
            genCodeForExpression (ifExpression);
            char branchInstruction [strlen (elseLabel) + strlen("IFZ")];
            sprintf (branchInstruction, "IFZ %s", elseLabel);
            generateFormattedInstruction (branchInstruction);
            //Genreate IFZ 
            genCodeForStatementList (takenStatementList);
            genGOTO (endLabel);
            //generate GOTO statement end
            //generateLabel (branch)
            generateLabel (elseLabel);
            genCodeForStatementList (elseStatementList);
            generateLabel (endLabel);
            //generate end label;
            break;
        }
        case NT_WHILE: {
            GNode *conditionalExpression = statement->children;
            GNode *whileStatementList = conditionalExpression->next;
            symbol *procSymbol = getFirstProcParent (statement);
            procInfo *procedureInfo;
            if (procSymbol == NULL) {
              procedureInfo = mainProcInfo;
            }
            else {
              procedureInfo = g_hash_table_lookup (procedureInfoTable, procSymbol);
            }
            char *label = genProcLabel (procedureInfo);
            char beginLabel [strlen (label) + strlen ("whilebegin")];
            char endLabel [strlen (label) + strlen ("whileend")];
            sprintf (endLabel, "%send", label);            
            sprintf (beginLabel, "%sbegin", label);
            generateLabel (beginLabel);
            genCodeForExpression (conditionalExpression);
            char branchinstruction [strlen ("IFZ") + strlen (endLabel)];

            sprintf (branchinstruction, "IFZ %s", endLabel);
            generateFormattedInstruction (branchinstruction);
            genCodeForStatementList (whileStatementList);
            genGOTO (beginLabel);
            generateLabel (endLabel);
            
            structInfo *whileInfo = calloc (1, sizeof (structInfo));
            whileInfo->beginLabel = beginLabel;
            whileInfo->endLabel = endLabel;
            
            g_hash_table_insert (labelTable, statement, whileInfo);

            
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
            //current node is now while (theoretically)
            structInfo *whileInfo = g_hash_table_lookup (labelTable, currentNode);
            char *gotoLabel = whileInfo->beginLabel;
            genGOTO (gotoLabel);
            //TODO: get tehe label for this while
            //genGOTO (whatever the daddy node is)
            break;
        }
        case NT_EXIT:
        {
            GNode *currentNode = statement->parent;
            while (getNiceType(currentNode) != NT_WHILE) {
                currentNode = currentNode->parent;
            }
            //current node is now while (theoretically)
            structInfo *whileInfo = g_hash_table_lookup (labelTable, currentNode);
            char *gotoLabel = whileInfo->endLabel;
            genGOTO (gotoLabel);

            //TODO: Get  the label for lowest scope funciton/procedure
            //genGOTO to the END of this procedure
            //will need to generate label for all return jumps...
            break;
        }
                    
    }
        
}

//Get the first parent of this node that is 
symbol *getFirstProcParent(GNode *node) {
  printf ("inside proc parent\n");
  printf ("Node address: %p\n", node);
  while (node != NULL) {
    if (getNiceType (node) == NT_PROC_DECL) {
      return getSymbol (node->children);
    }
    else if (getNiceType (node) == NT_PROGRAM) {
      return NULL;
      //return getSymbol (node->children);
    }
    node = node->parent;
  }
  return NULL;
  
}


/**
 * Return information needed to call a builtin here
 */
procInfo *getBuiltinInfo (symbol *builtinSymbol) {
    
}

void genCodeForExpression (GNode *expressionNode) {
    expressionNode = expressionNode->children;
    node_type exprType = getNiceType(expressionNode);
    switch (exprType) {
        case NT_VAR:
        {
            //TODO: implement
            node_type varType = getNiceType(expressionNode->children);
            if (varType == NT_SYMBOL) {
            
              symbol *varSymbol = getSymbol(expressionNode->children);
              varAddressStruct *address = g_hash_table_lookup (variableAddressTable, varSymbol);
            }
            else if (varType == NT_ARRAY_ACCESS) {
            }
            else if (varType == NT_RECORD_ACCESS) {
            }
            else {
            }
            break;
        }
        case NT_FUNC_INVOK:
        {
            symbol *funcSymbol = getSymbol (expressionNode->children);
            //TODO: push the parameters onto the stack
            //TODO: push the parameters onto the stack
            procInfo *functionInfo = g_hash_table_lookup (procedureInfoTable, funcSymbol);
            genProcCall (functionInfo);
            break;
        }
        case NT_CONST:
        {
            symbol *varSymbol = getSymbol (expressionNode->children);
            //getTypeClass (varSymbol);
            type_class constType = getTypeClass (varSymbol);
            //TODO: Actually check which type it is. Right now 
            //just get the integer value
            //TODO: figure out what to do for strings
            
            int value = varSymbol->desc.const_attr->value.integer;
            char instruction [strlen ("CONSTI") + 11];
            sprintf (instruction, "CONSTI %d", value);
            generateFormattedInstruction (instruction);
            
            break;
        }
        default:
        {
          printf ("I'm an operation!!!!!!\n");
          // if ((exprType >= NT_ISEQUAL) && (exprType <=NT_INVERSION)) 
            genCodeForOperation (expressionNode);
          // else
            // genCodeForExpression (expressionNode-
            //call gen for operations
        }

    }
}

//TODO: Don't forget that each one of these has to have a recursive call to
//expression
void genCodeForOperation (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    if ((exprType >= NT_INT_ISEQUAL) && (exprType <=NT_INT_GREATERTHANEQUALS)) {
        genCodeForIntegerComparison (expressionNode);
    }
    else if ((exprType >=NT_REAL_ISEQUAL) && (exprType <=NT_REAL_GREATERTHANEQUALS)) {
        //TODO: Implement
    }
    else if ((exprType >=NT_AND) && (exprType <= NT_NOT)) {

        genCodeForLogical (expressionNode);
    }
    else if ((exprType >= NT_INT_PLUS) && (exprType <= NT_INT_MULTIPLY)) {
        printf ("IS an int math thinger\n");
        genCodeForIntMath (expressionNode);
    }
    else if ((exprType >= NT_REAL_PLUS) && (exprType <= NT_REAL_MULTIPLY)) {
        genCodeForIntMath (expressionNode);
    }
    else if ((exprType == NT_INT_IDENTITY) || (exprType == NT_REAL_IDENTITY)) {
        //uh...do nothing.
    }
    else if ((exprType == NT_INT_INVERSION) || (exprType == NT_REAL_INVERSION)) {
    //NEED TO GET THE TYPE OF the thing we're inverting
      generateFormattedInstruction ("CONSTI 0");
    }
    //TODO: Now do this for reals
    else {
        printf ("Error. Tried to gernerate code for an operation when it was not an operation\n");
        printf ("is of type: %d\n", getNiceType (expressionNode));
    }
    
}

void genCodeForIntegerComparison (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    GNode *leftExpressionNode = expressionNode->children;
    GNode *rightExpressionNode = leftExpressionNode->next;
    genCodeForExpression (leftExpressionNode);
    genCodeForExpression (rightExpressionNode);
    switch (exprType) {
        case NT_ISEQUAL:
        {

          generateFormattedInstruction ("EQI");
          break;

        }
        case NT_NOTEQUAL:
        {          

          generateFormattedInstruction ("EQI");
          generateFormattedInstruction ("NOT");
          break;
          
        }
        case NT_LESSTHAN:
        {
          generateFormattedInstruction ("LTI");
          break;
        }
        case NT_GREATERTHAN:
        {

          generateFormattedInstruction ("GTI");
          break;

        }
        case NT_LESSTHANEQUALS:
        { 
          generateFormattedInstruction ("LTI");
          genCodeForExpression (leftExpressionNode);
          genCodeForExpression (rightExpressionNode);
          generateFormattedInstruction ("EQI");
          generateFormattedInstruction ("OR");
          break;
        }
        case NT_GREATERTHANEQUALS:
        {
          generateFormattedInstruction ("GTI");
          genCodeForExpression (leftExpressionNode);
          genCodeForExpression (rightExpressionNode);
          generateFormattedInstruction ("EQI");
          generateFormattedInstruction ("OR");
          break;
        }
    }
    
}

void genCodeForLogical (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    switch (exprType) {
        case NT_AND:
        {
          GNode *leftExpressionNode = expressionNode->children;
          GNode *rightExpressionNode = leftExpressionNode->next;
          genCodeForExpression (leftExpressionNode);
          genCodeForExpression (rightExpressionNode);
          generateFormattedInstruction("AND");
          break;
        }
        case NT_OR:
        {
          GNode *leftExpressionNode = expressionNode->children;
          GNode *rightExpressionNode = leftExpressionNode->next;
          genCodeForExpression (leftExpressionNode);
          genCodeForExpression (rightExpressionNode);
          generateFormattedInstruction("OR");
          break;
        }
        case NT_NOT:
        {
          GNode *expressionNode = expressionNode->children;
          genCodeForExpression (expressionNode);
          generateFormattedInstruction("NOT");
          break;
        }
    }
}

//TODO: Change to int math
void genCodeForIntMath (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    GNode *leftExpressionNode = expressionNode->children;
    genCodeForExpression (leftExpressionNode);
    GNode *rightExpressionNode = leftExpressionNode->next;
    genCodeForExpression (rightExpressionNode);
    switch (exprType) {
        case NT_INT_PLUS:
        {
          printf ("I'm IN PLUS\n");
          generateFormattedInstruction ("ADDI");
            break;
        }
        case NT_INT_MINUS:
        {
          generateFormattedInstruction ("SUBI");
            break;
        }
        case NT_INT_MULTIPLY:
        {
          generateFormattedInstruction ("MULI");
            break;
        }
        case NT_DIV:
        {
          generateFormattedInstruction ("DIVI");
            break;
        }
        case NT_MOD:
        {
          generateFormattedInstruction ("MOD");
            break;
        }
    }
}

//TODO: Implement
void genCodeForRealMath (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    GNode *leftExpressionNode = expressionNode->children;
    genCodeForExpression (leftExpressionNode);
    GNode *rightExpressionNode = leftExpressionNode->next;
    genCodeForExpression (rightExpressionNode);
    switch (exprType) {
        case NT_REAL_PLUS:
        {
            generateFormattedInstruction ("ADDR");
            break;
        }
        case NT_REAL_MINUS:
        {
            generateFormattedInstruction ("SUBR");
            break;
        }
        case NT_REAL_MULTIPLY:
        {
            generateFormattedInstruction ("MULR");
            break;
        }
        case NT_DIVIDE:
        {
            generateFormattedInstruction ("DIVR");
            break;
        }

    }

}

/**
 * Returns a label based on previous labels within the procedure
 * Also incremements label count for the procedure by 1
 */
char *genProcLabel (procInfo *procedureInfo) {
  int numLabels = procedureInfo->numLabels;
  char *procLabel = procedureInfo->procLabel;
  char *label = calloc ((strlen (procLabel) + 11 + strlen("label")), sizeof (char));
  sprintf (label, "%slabel%d", procLabel, numLabels);
  //generateLabel (label);
  numLabels++;
  procedureInfo->numLabels = numLabels;
  return label;
}
/**
 * Generates a string on the stack, with the first character at the lowest
 * index and the last character of the highest index
 */
void generateString (char *string) {
  int i=0;
  int charvalue;
  //yay, guaranteed by standard to work!
  char instruction [ strlen ("CONSTI") + 3];
  do {
    charvalue = string[i];
    sprintf (instruction, "CONSTI %d", charvalue);
    generateFormattedInstruction (instruction);
    i++;
  }
  while (string [i] != '\0');

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

void genGOTO (char const *label) {
    
    //TODO: find out if there is max label size in ASC
    char instruction [strlen ("GOTO") + 256];
    sprintf (instruction, "GOTO %s", label);
    generateFormattedInstruction (instruction);
}

void genVarAssign (varAddressStruct *addressDescription) {
    printf ("In gen var assign\n");
    printf ("Address of addressDescription: %p\n", addressDescription);
    int indexingRegister = addressDescription->indexingRegister;
    //programs are absolute, no index.

    int offset = addressDescription->offset;
    char instruction [strlen ("POPI []") + 32];
    if (indexingRegister < 0) {
      printf ("We are accessing program variable\n");
      sprintf (instruction, "POP %d", offset);
      generateFormattedInstruction (instruction);
    }
    else {
      sprintf (instruction, "POP %d[%d]", offset, indexingRegister);
      generateFormattedInstruction (instruction);

    }
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
