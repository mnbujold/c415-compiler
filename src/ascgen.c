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
#define PROGRAM_REGISTER -1
#define PROGRAM_VAR_OFFSET 0
#define NUM_RETURN_VALUES 2 //How many values on the stack RET consumes
#define MAX_LABEL_LEN 16 //this is the maximum length of the label
FILE *output;

//registers stores registers we are using. 
//if it is set to 0, then is free, if set to 1, being used
int *registers;
int globalAddressCounter = 0;



//store the current scope. we will need this for labels
int scope;
GHashTable *labelTable = NULL;
GHashTable *variableAddressTable = NULL;
GHashTable *procedureInfoTable = NULL;
GHashTable *builtinInfoTable = NULL;
int masterLabelCount = 0;

//We need a master label table because ASC is dumb and only accepts labels of 
//16 chars
GHashTable *masterLabelTable = NULL;

procInfo *mainProcInfo;
void genASCCode (GNode *tree, char *fileName) {
    DEBUG_PRINT(("!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
    DEBUG_PRINT (("Inside code generation\n"));
    DEBUG_PRINT(("!!!!!!!!!!!!!!!!!!!!!!!!!\n"));

    //Initialize our own tracking tools we will need for generating code

    labelTable = g_hash_table_new (g_direct_hash, g_direct_equal);
    variableAddressTable = g_hash_table_new (g_direct_hash, g_direct_equal);
    procedureInfoTable = g_hash_table_new (g_direct_hash, g_direct_equal);
    builtinInfoTable = g_hash_table_new (g_direct_hash, g_direct_equal);
    registers = calloc (NUM_ASC_REGISTERS, sizeof (int));
    scope = 0;
    
    masterLabelTable = g_hash_table_new (g_str_hash, g_str_equal);
    
   output = fopen (fileName, "w");
    //output = stdout;
  
 
  //start generating code
  
  node_type nodeType = getNiceType(tree); 
  if (nodeType != NT_PROGRAM ||g_node_n_children (tree) != 2 ) {
      //exit gracefully. The syntax tree is malformed
      printf ("head is of type %d, or does not have 2 children\n", nodeType);
      exit (1);
  }
  
  generateComment ("Program for");
  generateComment (fileName);
  genCodeForFunctionNode (tree, scope);
  //TODO: Check which builtins are used, and append them here
  fclose (output);
  
  
}

/**
 * Node must be of type PROGRAM, PROC, or FUNC
 */

//TODO: We need to store the number of words allocated on teh stack for us, so we can adjust and
//get rid of the,
void genCodeForFunctionNode(GNode *node, int scope) {
    //printf ("In gen code for function %d\n", getNiceType(node));
    
    //we'll consider program as a special case of a procedure declaration    
    if (getNiceType (node) == NT_PROGRAM) {
    
        //All we should have here is a goto to the main...becuase of order we generate code in
        //generateLabel("start");
        DEBUG_PRINT (("Inside program node generation"));
        GNode *declarations = node->children;
        GNode *statements = node->children->next;
        
        procInfo *procedureInfo = calloc (1, sizeof (procInfo));
        procedureInfo->procLabel = "main";
        procedureInfo->indexingRegister = PROGRAM_REGISTER;
        mainProcInfo = procedureInfo;
        //store this globally. If all else fails we jump to this symbol
        //g_hash_table_insert (procedureInfoTable, procedureSymbol, procedureInfo);
        GNode *varDeclarationsList = declarations->children;
        GNode *procDeclarations = declarations->children->next;
        if (getNiceType (declarations) == NT_DECLS) {
            
           varDeclarationsList = declarations->children;
           procDeclarations = declarations->children->next;
            addVariables (varDeclarationsList, PROGRAM_REGISTER, PROGRAM_VAR_OFFSET, mainProcInfo); //pass in the var_decl_list
            DEBUG_PRINT(("Type of procDeclarations: %d", getNiceType(procDeclarations)));
        }
        DEBUG_PRINT (("TYpe of statements: %d", getNiceType(statements)));
        //do the declarations stuff here
        //showVariableAddressTable();

        //TODO: We need to add a goto to the main, 
        addLabel ("main");
        genGOTO ("main");


        //GOTO the actual instructions in program
        //genGOTO("main");
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
        generateComment ("###STATEMENTS######");
        generateLabel ("main");
        genCodeForStatementList (statements);
        
        //we need this for exit
        generateLabel ("mainend");
        generateStackDump();
        genVarAdjust (mainProcInfo->numVarWords);
        generateStackDump();
        generateFormattedInstruction ("STOP");


//         
    }
    else if (getNiceType(node) == NT_PROC_DECL) {


        //symbol *procedureSymbol = (symbol *)getSymbol (node->children);
        //printf ("Procedure name: %s\n", procedureSymbol->name);

//        symbol *procedureSymbol = getSymbol (node->children);
      
        symbol *procedureSymbol = (symbol *)getSymbol (node->children);
//         printf ("Procedure name: %s\n", procedureSymbol->name);
        const char *procName = ((symbol *) procedureSymbol)->name;
//         printf ("Procedure name 2: %s\n", procName);

        GNode *parentProc = getFirstParent (node, NT_PROC_DECL, NT_PROC_DECL);
//         printf ("Successfully returned from getFirstParnet\n");
        //if null, is within program scope
        char *procLabel;
        if (parentProc == NULL) {
//             printf ("Parent proc is null\n");
            procLabel = calloc ((strlen (procName) + 2), sizeof (char));
            sprintf (procLabel, "%s%d", procName, scope);
        }
        else {
            symbol *parentSymbol = (symbol *)getSymbol (parentProc->children);
//             printf ("Got a symbol succesffully\n");
            procInfo *parentInfo = g_hash_table_lookup (procedureInfoTable, parentSymbol);
            char *parentLabel = parentInfo->procLabel;
            procLabel = calloc ((strlen (procName) + strlen (parentLabel) + 2), sizeof (char));
            sprintf (procLabel, "%s%s%d", parentLabel, procName, scope);
        }
        //append scope so we don't get duplicate labels

        generateLabel (procLabel);
        //printf ("Returned from generate label\n");
        //check which registers are free
        
        procInfo *procedureInfo = calloc (1, sizeof (procInfo));
        procedureInfo->procLabel = procLabel;
        int callingRegister = scope;
        procedureInfo->indexingRegister = callingRegister;
        g_hash_table_insert (procedureInfoTable, procedureSymbol, procedureInfo);
        GNode *declarations = node->children->next;
        GNode *statements = declarations->next;
        //1 for the symbol, 1 for return value
        int adjustAmount;
        GNode *varDeclarationsList = declarations->children;
        GNode *procDeclarations = declarations->children->next;
        GPtrArray *params;
        if (procedureSymbol->oc == OC_FUNC) {
//             printf ("Is a function\n");
            params = procedureSymbol->desc.func_attr->params;
//             printf ("Number of children of functinon:  %d\n", g_node_n_children( node));
            int offset = 0 -(params->len) - NUM_RETURN_VALUES - 1;
//             printf ("offset of return value %d\n", offset);
            varAddressStruct *functionReturnAddress = calloc (1, sizeof (varAddressStruct));
            functionReturnAddress->indexingRegister = callingRegister;
            functionReturnAddress->offset = offset;
            g_hash_table_insert (variableAddressTable, procedureSymbol, functionReturnAddress);
            varAddressStruct *returned = g_hash_table_lookup (variableAddressTable, procedureSymbol);
//             printf ("Function: Address of %s: %p\n", procedureSymbol->name, procedureSymbol);
            // printf ("Address of function symbol: %p\n", procedureSymbol);
            // printf ("address o returned thing: %p\n", returned);
            // adjustAmount = g_node_n_children (node) -2;

        }
        else {
            // adjustAmount = g_node_n_children (node) - 1;
            params = procedureSymbol->desc.proc_attr->params;
        }
        
        int numParams = params->len;
        //adjustAmount += numParams;
        procedureInfo->numVarWords += numParams;
        int i = 0;
        int paramOffset = 0 - numParams- NUM_RETURN_VALUES;
        printf ("PARAM offset: %d\n", paramOffset);
        while (i < numParams) {
            symbol *paramSymbol = (symbol *) g_ptr_array_index (params, i);
            if (paramSymbol->desc.parm_attr->varParam) {
            //TODO: I can actually just pass in the address..
            //Don't need to do anything, except when i'm actually accessing it
              char instruction [strlen ("PUSH -[00]") + 12];
              sprintf (instruction, "PUSH %d[%d]", paramOffset + i, callingRegister);
              generateFormattedInstruction (instruction);
              varAddressStruct *addressStruct = calloc (1, sizeof (varAddressStruct));
              addressStruct->indexingRegister = callingRegister;
              addressStruct->offset = i;
              g_hash_table_insert (variableAddressTable, paramSymbol, addressStruct);
              printf ("Address of param symbol: %p\n", paramSymbol);
            }
            else {
            
              char instruction [strlen ("PUSH -[00]") + 12];
              sprintf (instruction, "PUSH %d[%d]", paramOffset + i, callingRegister);
              generateFormattedInstruction (instruction);
              varAddressStruct *addressStruct = calloc (1, sizeof (varAddressStruct));
              addressStruct->indexingRegister = callingRegister;
              addressStruct->offset = i;
              g_hash_table_insert (variableAddressTable, paramSymbol, addressStruct);
            }
            i++;
        }
        //TODO: Actually, we need to add the parameters first
        //go through the procedureSymbol, and get the parameters and
        //allocate space for them
        //TODO: Get the count of params
        if (getNiceType (declarations) == NT_DECLS) {
            varDeclarationsList = declarations->children;
            procDeclarations = declarations->children->next;
            addVariables (varDeclarationsList, callingRegister, numParams,  procedureInfo); //pass in the var_decl_list
            //DEBUG_PRINT(("Type of procDeclarations: %d", getNiceType(procDeclarations)));
        }
        DEBUG_PRINT (("TYpe of statements: %d", getNiceType(statements)));
        //do the declarations stuff here
        //showVariableAddressTable();

        generateComment("####STATEMENTS#####");
        genCodeForStatementList (statements);
        
        adjustAmount += procedureInfo->numVarWords;
       // genVarAdjust (adjustAmount);
        
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
void addVariables(GNode *varDeclNode, int indexingRegister, int offset, procInfo *procedureInfo) {
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
      offset += variableIterator (varNode, indexingRegister, offset);
      varNode = varNode->next;
    }
    procedureInfo->numVarWords = offset;
    generateStackDump();
    //while s
    

}

int pushScalar(symbol *symb){
  int size = symb->desc.type_attr->desc.scalar->len;
  //printf ("size: %d \n", size);

  char instruction[strlen("ADJUST") + sizeof(size) + 1];
  sprintf(instruction, "ADJUST %d", size);
  generateFormattedInstruction(instruction);

  return(size);
}


int pushArray(symbol *symb){
  // set return type to amount allocated
  //int size = symb->symbol_type->desc.type_attr->desc.array->size;
  int size = symb->desc.type_attr->desc.array->size;
  int finalsize = 0;
  type_class varType = getTypeClass(symb->desc.type_attr->desc.array->obj_type);
  
  //printf("array tc: %d \n", getTypeClass(symb->symbol_type->desc.type_attr->desc.array->obj_type));

  // Handle 'special' cases of arrays (eg. arrays of arrays or arrays of records)
  //printf("ARRAY SIZE: %d , TYPE: %d \n", size, varType);
  if(varType == TC_RECORD){
    //printf("pushing rec\n");

    int i;
    for(i=0; i<size; i++)
      finalsize += pushRecord(symb->desc.type_attr->desc.array->obj_type);
    
  }
  else if(varType == TC_ARRAY){
    //printf("pushing array\n");
    int i;
    for(i=0; i<size; i++){
      finalsize += pushArray(symb->desc.type_attr->desc.array->obj_type);
    }
    
  }
  // All other types of arrays
  else{
    char instruction[strlen("ADJUST") + sizeof(size) + 1];
    sprintf (instruction, "ADJUST %d", size); 
    generateFormattedInstruction (instruction);
    finalsize = size;
  }
  return(finalsize);
}

int pushRecord(symbol *symb){
  int size = 0;
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
    size += variableHandler(recSymbol, getTypeClass(recSymbol), lala);
    
  }

  return(size);
}
//TODO: Split variableIterator into an iterator fcn and a 'handler' fcn
//Function that is called for each var declaration
int variableIterator (GNode *varNode, int indexingRegister, int offset) {
    
    //int indexingRegister = *scope;
    //int indexingRegister = 0;
    symbol *sym = getSymbol (varNode);
    printf ("Adding this symbol: %s\n", sym->name);
    printf ("\tThe address: %p\n", sym);
    printf ("\tRegister: %d Offset: %d\n", indexingRegister, offset);
    type_class varType = getTypeClass (sym);

    varAddressStruct *addressDescription = calloc (1, sizeof (varAddressStruct));
    addressDescription->indexingRegister = indexingRegister;
    addressDescription->offset = offset;

    g_hash_table_insert(variableAddressTable, sym, addressDescription);
    if(g_hash_table_lookup (variableAddressTable, sym) == NULL){
      printf("WHAT THE HECK!? Empty when we just inserted??\n");
    }
    
    int value =variableHandler(sym, getTypeClass(sym), addressDescription);
    printf("\tTotal size of variable: %d \n", value);
    return value;
    
    //TODO: each variable needs to have its address added to the variableAddressTable
}

int variableHandler(symbol *symb, type_class varType, varAddressStruct *addDescription){

  int size = -1;
  //printf ("Inside variable handler\n");
  //printf ("Address inside variable handler: %p\n", symb);
  char *varComment = calloc ((strlen (symb->name) + strlen ("Var ")), sizeof (char));
  sprintf (varComment, "Var %s", symb->name);
  generateComment(varComment);
/* Probably better to do this outside the handler, since handler deals with record vars
  g_hash_table_insert(variableAddressTable, symb, addDescription);
  if (g_hash_table_lookup (variableAddressTable, symb) == NULL) {
    printf ("WHAT HTE HECK. empty when we just inserted...?\n");
  }
*/
  //varAddressStruct *check_it = g_hash_table_lookup(variableAddressTable, symb);
  //printf("idxReg: %d, Offset: %d \n", check_it->indexingRegister, check_it->offset);
  
   if (varType == TC_INTEGER) {
        pushConstantInt (0);
        size = 1;
    }
    else if (varType == TC_REAL) {
        pushConstantReal (0);
        size = 1;
    }
    else if (varType == TC_BOOLEAN) {
        pushConstantInt (0);
        size = 1;
    }
    else if (varType == TC_STRING){
      printf("string: %s\n", symb->name);
        //TODO: Need to do for arrays
        printf ("error, this kind of variable not yet implemented\n");
        DEBUG_PRINT (("Not implemented yet"));
    }
    else if(varType == TC_ARRAY){
      //printf("size array: %d \n", symb->symbol_type->desc.type_attr->desc.array->size);
      size = pushArray(symb->symbol_type);
    }
    else if(varType == TC_RECORD){
      size = pushRecord(symb->symbol_type);
    }
    else if(varType == TC_SCALAR){
      // Handles like an array
      //printf("scalar here \n");
      size = pushScalar(symb->symbol_type);
      
    }
    else{
      // For debugging: Spit out the typeclass we don't yet handle here
      printf("Type: %d \n", varType);

    }
    globalAddressCounter++; //do we need global address counter anymore?

    // Return the size of the variable so we know where it exists on the stack
    return(size);
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
int getExpressionValue (GNode *expressionNode) {
  // Should Return expr value for array accesses (maybe useful for other things)
  // This whole function might just have to go...not using it right now
  printf("type of expr node: %d \n", getNiceType(expressionNode));

  node_type exprType = getNiceType(expressionNode);
  switch (exprType) {
     case NT_CONST:
     {
       symbol *varSymbol = getSymbol(expressionNode->children);
       
       type_class constType = getTypeClass(varSymbol);
       switch (constType) {
         case TC_BOOLEAN:
         {
           return(varSymbol->desc.const_attr->value.boolean);
           break;
         }
         case TC_CHAR:
         {
           return(varSymbol->desc.const_attr->value.character);
           break;
         }
         case TC_INTEGER:
         {
           return(varSymbol->desc.const_attr->value.integer);
           break;
         }
         default:
         {
           printf("ERROR: Invalid array access type.\n");
           return(-1);
           break;
         }
       }
       break;
     }

     // NT_VAR can be: NT_SYMBOL or NT_ARRAY_ACCESS or NT_RECORD_ACCESS
     case NT_VAR:
     {
       node_type varType = getNiceType(expressionNode->children);
       if (varType == NT_SYMBOL) {
         // Works
         genVarAccess(g_hash_table_lookup(variableAddressTable, getSymbol(expressionNode->children)));
         generateFormattedInstruction("PUSHI");
       }

       else if (varType == NT_ARRAY_ACCESS) {
         //TODO: implement
         printf("accessing array for assignment...\n");
         //find the array...get the element
         GNode *varNode = expressionNode->children->children->children;
         GNode *exprNode = expressionNode->children->children->next;
         // push address of var onto stack
         genVarAccess(g_hash_table_lookup(variableAddressTable, getSymbol(varNode)));;
         // add expr to it
         genCodeForExpression(exprNode);
         generateFormattedInstruction("ADDI");
         generateFormattedInstruction("PUSHI");
         
       }
       else if (varType == NT_RECORD_ACCESS) {
         //TODO: implement
       }
       else { printf("What are you doing here!?\n"); }
       break;
    }
     
    case NT_FUNC_INVOK:
    {
       symbol *funcSymbol = getSymbol (expressionNode->children);
       GNode *currentParamNode = expressionNode->children->next;
       //-1 for symbol that is a child, -1 for the return value
       //allocate space for return value 
       generateFormattedInstruction ("ADJUST 1");
       int numParams = g_node_n_children (expressionNode) - 1;
       printf ("Number of params for function invocation: %d\n", numParams);
       while (currentParamNode != NULL) {
         genCodeForExpression(currentParamNode);
         currentParamNode = currentParamNode->next;
       }
            procInfo *functionInfo = g_hash_table_lookup (procedureInfoTable, funcSymbol);
            if (functionInfo == NULL) {
                genBuiltinCall (funcSymbol);
            }
            else {
            genProcCall (functionInfo);
            }
            genVarAdjust (numParams);
            break;
        }
       
    default:
    {
      // Operations
    }
  }
  
}

void genCodeForStatement(GNode *statement) {
    node_type statementType = getNiceType(statement);
    switch (statementType) {
        case NT_ASSIGNMENT:
        {
            //evaluate the expression
            printf ("--GENERATING CODE FOR ASSIGNMENT--\n");
            //remember there is a level of indrirection here
            GNode *varNode = statement->children;
            node_type varType = getNiceType (varNode->children);
            if (varType == NT_SYMBOL) {
              GNode *symbolNode = (statement->children)->children;
              // printf ("type of var's first child: %d\n", getNiceType (symbolNode));
              symbol *varSymbol = getSymbol (symbolNode);
              printf ("symbol name: %s\n", varSymbol->name);
              printf ("Symbol type: %d\n", varSymbol->oc);
              varAddressStruct *addressDescription = g_hash_table_lookup (variableAddressTable, varSymbol);
              if (varSymbol->oc == OC_PARAM) {
                  printf ("Is a parameter\n");
                if (varSymbol->desc.parm_attr->varParam) {
                   printf ("IM A PAASFSAFASFASFASFA\n");
                  GNode *expressionNode = statement->children->next;
                  genVarParamAssign (addressDescription);
                  genCodeForExpression (expressionNode);
                  generateFormattedInstruction("POPI");
                  return;
                }
              }
//               printf ("ASSIGNMENT\n");
//               printf ("symbol name: %s\n", varSymbol->name);
//               printf ("Symbol address: %p\n", varSymbol);

              GNode *expressionNode = statement->children->next;
              genCodeForExpression (expressionNode);
              printf ("Address of address description from hash: %p\n", addressDescription);
              genVarAssign (addressDescription,0);
            }
            else if (varType == NT_ARRAY_ACCESS) {
              GNode *varNode = statement->children->children; // lvalue
              GNode *exprNode = statement->children->next; // rvalue
              // Get address of symbol
              symbol *varSymbol = getSymbol(varNode->children->children);
              varAddressStruct *addrDesc = g_hash_table_lookup(variableAddressTable, varSymbol);
              // Suction out the 'expr' value to offset the array
              GNode *lval_exprNode = varNode->children->next;
              // Push lval_expr value on stack
              genCodeForExpression(lval_exprNode); // Relative array position
              // First subscript now on stack (ie. v1)

              // Subtract lower bound of v1 (ie. l1)
              int lower = varSymbol->symbol_type->desc.type_attr->desc.array->minIndex;
              //push lower onto stack
              // emit SUBI
              // push p1 on stack
              
              
              // TODO: Deal with strange starting positions (eg. -2)

              int u_vals[10];
              int l_vals[10];
              
              // First array size
              int size = varSymbol->symbol_type->desc.type_attr->desc.array->size;
              u_vals[0] = varSymbol->symbol_type->desc.type_attr->desc.array->maxIndex;
              l_vals[0] = varSymbol->symbol_type->desc.type_attr->desc.array->minIndex;
              
              
              symbol *objSymb = varSymbol->symbol_type->desc.type_attr->desc.array->obj_type;
              printf("\tArray is of type %d and size %d\n", getTypeClass(objSymb), size);
              printf("\tUpper is %d and lower is %d\n", u_vals[0], l_vals[0]);

              int i=1;
              while(getTypeClass(objSymb) == 6){
                u_vals[i] = objSymb->desc.type_attr->desc.array->maxIndex;
                l_vals[i] = objSymb->desc.type_attr->desc.array->minIndex;
                printf("\tobject number %d\n", i);
                i++;
                objSymb = objSymb->desc.type_attr->desc.array->obj_type;
                
              }

              printf("type we are arraying: %d \n", getTypeClass(objSymb));
                            
              int j;
              for(j=0; j<i; j++){
                printf("\tu: %d, \tl: %d nests: %d \n", u_vals[j], l_vals[j], i);
                
              }
              // need size of final arrayval
              
              int p_vals[10];
              for(j=1; j<i; j++){
                p_vals[j] = u_vals[j-1] - l_vals[j-1] +1;
                printf("p_val: %d\n", p_vals[j]);
              }
              
              // For multi-dimensional business - not 100% working yet
              // MIKE IS WORKING ON THIS
              while(0){
              //while(lval_exprNode->next != NULL){
                printf("xtra dim array\n");
                genCodeForExpression(lval_exprNode->next);
                // get size of arrays for calculating address

                int nextsize = objSymb->desc.type_attr->desc.array->size;
                printf("\tNext Array Size: %d\n", nextsize);
                
                char instruction [strlen ("CONSTI") + 256];
                sprintf(instruction, "CONSTI %d", size);
                generateFormattedInstruction(instruction);
                // multiply two values
                generateFormattedInstruction("MULI");
                
                generateFormattedInstruction("ADDI");
                lval_exprNode = lval_exprNode->next;

                objSymb = objSymb->desc.type_attr->desc.array->obj_type;
                printf("\tobjSymb type: %d \n", getTypeClass(objSymb));
              }
              
              char instruction [strlen ("CONSTI") + 256];
              sprintf (instruction, "CONSTI %d", addrDesc->offset);
              generateFormattedInstruction (instruction);
              generateFormattedInstruction("ADDI");
              genCodeForExpression(exprNode);
              generateFormattedInstruction("POPI");
            }
            else if (varType == NT_RECORD_ACCESS) {
              //TODO: Implement
            }
            else {
            }

         
            break;
        }
        case NT_PROC_INVOK:
        {

//             GNode *currentParamNode = statement->children->next;
//             int numParams = g_node_n_children (statement) - 1;
//             while (currentParamNode != NULL) {
//                 genCodeForExpression(currentParamNode);
//                 currentParamNode = currentParamNode->next;
//             }
            GNode *symbolNode = statement->children;
//             printf ("We have a procedure invocation here\n");
//             if (symbolNode == NULL) {
//                 printf ("Symbol node is null\n");
//             }
            //printf ("type of symbolNode: %d\n", getNiceType (symbolNode));
            
            symbol *procSymbol = getSymbol (symbolNode);
          //  symbol *writelnSymbol = globalLookup ("writeln");
//             printf ("proc symbol address %p\n", procSymbol);
//             printf ("writeln address %p\n", writelnSymbol);
//             GPtrArray *params = procSymbol->desc.proc_attr->params;
//             int numParams = params->len;
//             
            //Even though looks hackish, best wya to do it because what if we have
            //varyign numbers of parameters?
            GNode *currentParamNode = statement->children->next;
            int numParams = g_node_n_children (statement) -1;
            procInfo *procedureInfo = (procInfo *) g_hash_table_lookup (procedureInfoTable, procSymbol);
            //printf ("Returned proc label: %s\n", procLabel);
//             printf ("procedure info returned\n");
            //generate CALL
            if (procedureInfo == NULL) {
                printf ("Returned proc info is null, must be an IO procedure\n");
                const char *procName = procSymbol->name;
                if (strcmp (procName, "writeln") == 0) {
                    genCodeForWrite (currentParamNode, 1);
                }
                else if (strcmp (procName, "write") == 0) {
                    genCodeForWrite (currentParamNode, 0);
                }
                else if (strcmp (procName, "read") == 0) {
                    genCodeForRead (currentParamNode, 0);
                }
                else if (strcmp (procName, "readln") == 0) {
                    genCodeForRead (currentParamNode, 1);
                }
                else {
                    //Just in case I'm forgetting a procedure
                    //or theres some procedure that doesn't need special handling

                    genBuiltinCall(procSymbol);
                }

            }
            else {
            
            //TODO: before generating proc call, we need to get params from here
            //and then 

            while (currentParamNode != NULL) {
                genCodeForExpression(currentParamNode);
                currentParamNode = currentParamNode->next;
            }
            genProcCall (procedureInfo);
            genVarAdjust (numParams);
            }
            //TODO: Handle var params as we may actually use them 

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
//           char instruction [strlen (label) + strlen ("IFZ")];
//           sprintf (instruction, "IFZ %s", label);
          genBranch (label);
//           generateFormattedInstruction (instruction);
          // sprintf (instruction, "IFZ %s", label);
          genCodeForStatementList (ifStatementList);
          generateLabel (label);

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
            addLabel (elseLabel);
            addLabel (endLabel);
            genCodeForExpression (ifExpression);
//             char branchInstruction [strlen (elseLabel) + strlen("IFZ")];
//             sprintf (branchInstruction, "IFZ %s", elseLabel);
            genBranch (elseLabel);
//             generateFormattedInstruction (branchInstruction);
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
            printf ("IN WHILE\n");
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
            
            structInfo *whileInfo = calloc (1, sizeof (structInfo));
            whileInfo->beginLabel = beginLabel;
            whileInfo->endLabel = endLabel;
            addLabel (beginLabel);
            addLabel (endLabel);
            
            g_hash_table_insert (labelTable, statement, whileInfo);
            generateLabel (beginLabel);
            
            printf ("------About to generate code for expression-----\n");
            genCodeForExpression (conditionalExpression);
            printf ("-------Done generating expression----------\n");
//             char branchinstruction [strlen ("IFZ") + strlen (endLabel)];
// 
//             sprintf (branchinstruction, "IFZ %s", endLabel);
//             generateFormattedInstruction (branchinstruction);
            genBranch (endLabel);
            genCodeForStatementList (whileStatementList);
            genGOTO (beginLabel);
            generateLabel (endLabel);
            

            printf ("While address: %s %p\n",beginLabel,  statement);

            
            //evaluate expressoin
            //ifZ GOTO blah blah
            //GOTO While beginnning
            break;
        }
        case NT_CONTINUE: {
            printf ("Continue called\n");
            GNode *currentNode = statement->parent;
            while (getNiceType(currentNode) != NT_WHILE) {
                currentNode = currentNode->parent;
            }
            //current node is now while (theoretically)
            structInfo *whileInfo = g_hash_table_lookup (labelTable, currentNode);
            if (whileInfo == NULL) {
                printf ("Node type: %d\n", getNiceType (currentNode));
                printf ("Address of this: %p\n", currentNode);
                printf ("While info is null\n");
            }
            char *gotoLabel = whileInfo->beginLabel;
            genGOTO (gotoLabel);
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


GNode *getFirstParent (GNode *currentNode, node_type parentTypeLowerBound, node_type parentTypeUpperBound) {
//     printf ("Broken in here\n");
    currentNode = currentNode->parent; //if we are checking for the same thing, this will break it;
    while (currentNode != NULL) {
        node_type currentNodeType = getNiceType (currentNode);
//         printf ("get first parent node type: %d\n", currentNodeType);
        if ((currentNodeType >= parentTypeLowerBound) && (currentNodeType <= parentTypeUpperBound)) {       
            return currentNode;
        }

        currentNode = currentNode->parent;
    }
    return NULL;
}

//Get the first parent of this node that is 
symbol *getFirstProcParent(GNode *node) {
//   printf ("inside proc parent\n");
//   printf ("Node address: %p\n", node);
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
void genBuiltinCall (symbol *builtinSymbol) {
    const char *procName = builtinSymbol->name;
    if (strcmp (procName, "writeln") == 0) {
        generateComment ("Writeln call here");
//         printf ("Writeln detected\n");
    }
    else if (strcmp (procName, "write") == 0) {
    
    }
    else if (strcmp (procName, "readln") == 0) {
    }
    else if (strcmp (procName, "read") == 0) {
    }
    else if (strcmp (procName, "abs") == 0) {
        //genProcCall (
    }
    else if (strcmp (procName, "chr") == 0) {
    }
    else if (strcmp (procName, "cos") == 0) {
    }
    else if (strcmp (procName, "ln") == 0) {
    }
    else if (strcmp (procName, "odd") == 0) {
    }
    else if (strcmp (procName, "ord") == 0) {
    }
    else if (strcmp (procName, "pred") == 0) {
    }
    else if (strcmp (procName, "round") == 0) {
    }
    else if (strcmp (procName, "sin") == 0) {
    }
    else if (strcmp (procName, "sqr") == 0) {
    }
    else if (strcmp (procName, "sqrt") == 0) {
    }
    else if (strcmp (procName, "succ") == 0) {
    }
    else if (strcmp (procName, "exp") == 0) {
        
    }
    else if (strcmp (procName, "trunc") == 0) {
//         printf ("Call to trunc\n");
        procInfo truncInfo = {.procLabel = TRUNC_LABEL, .indexingRegister = BUILTIN_REGISTER};
        genProcCall (&truncInfo);
    }
    else {
    }
    
}

void genCodeForExpression (GNode *expressionNode) {
    //SPECIAL CASE to handle var params
    int isVarParam = 0;
    node_type parentNodeType = getNiceType (expressionNode->parent);
    if (parentNodeType == NT_PROC_INVOK || parentNodeType == NT_FUNC_INVOK) {
//         printf ("Parent node was a procedure or function invocation\n");

        GNode *parentNode = expressionNode->parent;
        int position = g_node_child_position (parentNode, expressionNode);
        symbol *procSymbol = getSymbol (parentNode->children);
        GPtrArray *params;
        
        if (procSymbol->oc == OC_FUNC) {
            params = procSymbol->desc.func_attr->params;
        }
        else if (procSymbol->oc == OC_PROC) {
            params = procSymbol->desc.proc_attr->params;
        }
        else {
//             printf ("In the else %d\n", getTypeClass (procSymbol));
        }
//         printf ("Done getting the params list\n");
        char *procName = procSymbol->name;
        printf ("The name of the procedure is: %s\n", procName);
        if ((strcmp (procName, "writeln") ==0) || (strcmp (procName, "write") ==0) ||
            (strcmp (procName, "read") == 0) || (strcmp (procName, "readln") == 0)) {
            printf ("Is an io procedure\n");
            //TODO: If it is an int or real, we do not set it to be a var param
            if ((getExpressionType(expressionNode) != TC_REAL) && (getExpressionType (expressionNode) != TC_INTEGER)) {
                printf ("EXPRESSION TYPE OF THE THING: %d\n", getExpressionType (expressionNode));
                isVarParam = 1;
                
            }
            else {
                if ((strcmp (procName, "read") == 0) || (strcmp (procName, "readln") == 0))
                    isVarParam = 1;
                printf ("IO FUNCTION BUT IS INTEGER OR REAL\n");
            }
            
        }
        else {
            if (params == NULL) {
                //This should never happen...
                //I think.
            }
            else {
                printf ("Got the params list, am about to get the symbol\n");
                printf ("My position: %d\n", position);
                position = position - 1;
//                 gpointer *lala = g_ptr_array_index (params, position);
                symbol *paramSymbol = (symbol *) g_ptr_array_index (params, position);
//                 printf ("Address of lala: %p\n", lala);
//                 printf ("address of param symbol: %p\n", paramSymbol);
//                 printf ("Type: %d\n", getTypeClass (paramSymbol));
                //printf ("param symbol type: %d\n", paramSymbol->oc);
//                 if (paramSymbol == NULL) {
//                     printf ("returned param symbol is null\n");
//                 }
                if (paramSymbol->desc.parm_attr->varParam) {
                    isVarParam = 1;
                }
            }
            
        }
        

        //get the index of the params
        printf ("Done checking if it is a var param\n");
    }
    expressionNode = expressionNode->children;
//     printf ("In genCodeForExpression\n");
    node_type exprType = getNiceType(expressionNode);
//     printf ("Got node type in genCodeForExpression\n");
//     if (getFirstParent (expressionNode, NT_PROC_INVOK, NT_FUNC_INVOK) != NULL) 
    
    switch (exprType) {
        case NT_CONST:
        {
            symbol *varSymbol = getSymbol (expressionNode->children);
            //getTypeClass (varSymbol);
            type_class constType = getTypeClass (varSymbol);
            //TODO: figure out what to do for strings
            switch (constType) {
                case TC_REAL:
                {
                    double value = varSymbol->desc.const_attr->value.real;
                    char instruction [strlen ("CONSTR") + 12];
                    sprintf (instruction, "CONSTR %f", value);
                    generateFormattedInstruction (instruction);
                    break;
                }
                case TC_BOOLEAN:
                {
                    int value = varSymbol->desc.const_attr->value.boolean;
                    char instruction [strlen ("CONSTI") + 2];
                    sprintf (instruction, "CONSTI %d", value);
                    generateFormattedInstruction (instruction);
                    break;
                }
                case TC_CHAR:
                {
                    int value = varSymbol->desc.const_attr->value.character;
                    char instruction [strlen ("CONSTI") + 3];
                    sprintf (instruction, "CONSTI %d", value);
                    generateFormattedInstruction (instruction);
                    break;
                }
                default:
                {
                    int value = varSymbol->desc.const_attr->value.integer;
                    char instruction [strlen ("CONSTI") + 12];
                    sprintf (instruction, "CONSTI %d", value);
                    generateFormattedInstruction (instruction);
                    break;
                }
            }
            
            //Check if we have a real parent so we need to convert to real
            GNode *operationNode = getFirstOperationParent (expressionNode->parent);
            if (operationNode == NULL) {
                //                 printf ("Operation node is null\n");
            }
            else {
                node_type type = getNiceType (operationNode);
                //                 printf ("Inside int math: Type of node is: %d\n", type);
                if ((type >= NT_REAL_ISEQUAL) && (type <=NT_REAL_INVERSION) && (constType != TC_REAL)) {
                    // //                     printf ("We have a real operation\n");
                    generateFormattedInstruction ("ITOR");
                }
                
            }
            //We need to turn this guy into a real if one of its parent is a real operation
            //get the first operation we see
            //             GNode *operationNode = getFirstOperationParent (expressionNode);
            //             node_type type = getNiceType (operationNode);
            //             if ((type >= NT_REAL_ISEQUAL) && (type <=NT_REAL_INVERSION)) {
                //                 printf ("We have a real operation\n");
                //                 generateFormattedInstruction ("ITOR");
                //             }
                
                break;
        }
        case NT_VAR:
        {
            printf ("---VAR IN AN EXPRESSION\n");
            printf ("Address of the var: %p\n", expressionNode);

            node_type varType = getNiceType(expressionNode->children);
            if (varType == NT_SYMBOL) {
                printf ("Is a symbol\n");
            
              symbol *varSymbol = getSymbol(expressionNode->children);
              varAddressStruct *address = g_hash_table_lookup (variableAddressTable, varSymbol);
//               if (varSymbol->oc == OC_PARAM) {
//                   printf ("This is a param: %s\n",  varSymbol->name);
//                 if (varSymbol->desc.parm_attr->varParam) {
//                   genVarParam (address);
//                   return;
//                 }
//               }

//               if (address == NULL) {
//                   printf ("Failure. address is null\n");
//                   printf ("This is the address of the var symbol: %p\n", varSymbol);
//                   printf ("Symbol name: %s type: %d typeClass: %d\n", varSymbol->name, varSymbol->oc, getTypeClass (varSymbol));
//               }
              printf ("Symbol we're accessing: %s\n", varSymbol->name);
              printf ("Type of the symbol; %d\n", varSymbol->oc);
              printf ("varParam flag: %d\n", isVarParam);
              if (isVarParam) {
                  genVarParam(address);
              }
              else {
                  printf ("We are not passing in a parameter\n");
                  printf ("Not an address, just put it on normally\n");
                  genVarAccess (address);
                  if (varSymbol->oc == OC_PARAM) {
                      if (varSymbol->desc.parm_attr->varParam) {
                          generateFormattedInstruction("PUSHI");
                      }
                  }
              }

            }
            else if (varType == NT_ARRAY_ACCESS) {
                //TODO: implement
              printf("accessing array...\n");
              //find the array...get the element
              GNode *varNode = expressionNode->children;
              
              printf("varNode type: %d \n", getNiceType(varNode->children));
              printf("varnode name: %s \n", getSymbol(varNode->children->children)->name);
              printf("varNode child type: %d \n", getNiceType(varNode->children->children));

              // output the base value of array specified
              varAddressStruct *addrDesc = g_hash_table_lookup(variableAddressTable, getSymbol(varNode->children->children));
              
              char instruction [strlen ("CONSTI") + 256];
              sprintf (instruction, "CONSTI %d", addrDesc->offset);
              generateFormattedInstruction (instruction);
              
              // add offset in expr (from below)
              GNode *arrayExpressionNode = varNode->children->next;
              printf("expNode type: %d \n", getNiceType(arrayExpressionNode->children));
              genCodeForExpression(arrayExpressionNode);  

              generateFormattedInstruction("ADDI");
              generateFormattedInstruction("PUSHI");
                //TODO: How we going to do bounds checking?

            }
            else if (varType == NT_RECORD_ACCESS) {
                //TODO: implement
            }
            else {
              
            }
            break;
        }
        case NT_FUNC_INVOK:
        {
            symbol *funcSymbol = getSymbol (expressionNode->children);
            GNode *currentParamNode = expressionNode->children->next;
            //-1 for symbol that is a child, -1 for the return value
            //allocate space for return value 
            generateFormattedInstruction ("ADJUST 1");
            int numParams = g_node_n_children (expressionNode) - 1;
            printf ("Number of params for function invocation: %d\n", numParams);
            while (currentParamNode != NULL) {
                genCodeForExpression(currentParamNode);
                currentParamNode = currentParamNode->next;
            }
            procInfo *functionInfo = g_hash_table_lookup (procedureInfoTable, funcSymbol);
            if (functionInfo == NULL) {
                genBuiltinCall (funcSymbol);
            }
            else {
            genProcCall (functionInfo);
            }
            genVarAdjust (numParams);
            break;
        }

        default:
        {
//           printf ("I'm an operation!!!!!!\n");
          // if ((exprType >= NT_ISEQUAL) && (exprType <=NT_INVERSION)) 
            genCodeForOperation (expressionNode);
          // else
            // genCodeForExpression (expressionNode-
            //call gen for operations
        }

    }
}
GNode *getFirstOperationParent (GNode *expressionNode) {
//     printf ("In first operation parent\n");
    GNode *currentNode = getFirstParent (expressionNode, NT_ISEQUAL, NT_INVERSION);
    if (currentNode != NULL)
        return currentNode;
//     printf ("past first if\n");
    currentNode = getFirstParent (expressionNode, NT_INT_ISEQUAL, NT_INT_INVERSION);
    if (currentNode != NULL)
        return currentNode;
    
//     printf ("past second if\n");
    currentNode = getFirstParent (expressionNode, NT_REAL_ISEQUAL, NT_REAL_INVERSION);
//     printf ("Done the 3rd thing\n");
    return currentNode;
    
//     printf ("past third if\n");
}
void genCodeForOperation (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    if ((exprType >= NT_INT_ISEQUAL) && (exprType <=NT_INT_GREATERTHANEQUALS)) {
//         printf ("Is of type int comparison\n");
        genCodeForIntComparison (expressionNode);
    }
    else if ((exprType >=NT_REAL_ISEQUAL) && (exprType <=NT_REAL_GREATERTHANEQUALS)) {
        //TODO:
        genCodeForRealComparison (expressionNode);
    }
    else if ((exprType >=NT_AND) && (exprType <= NT_NOT)) {

        genCodeForLogical (expressionNode);
    }
    else if ((exprType >= NT_INT_PLUS) && (exprType <= NT_INT_MULTIPLY)) {
       // printf ("IS an int math thinger\n");
        genCodeForIntMath (expressionNode);
    }
    else if ((exprType >= NT_REAL_PLUS) && (exprType <= NT_REAL_MULTIPLY)) {
        genCodeForRealMath (expressionNode);
    }
    else if ((exprType == NT_INT_IDENTITY) || (exprType == NT_REAL_IDENTITY)) {
        //uh...do nothing.
        //we might need to convert between I and R
        genCodeForExpression (expressionNode->children);
    }
    else if ((exprType == NT_INT_INVERSION) || (exprType == NT_REAL_INVERSION)) {
    //NEED TO GET THE TYPE OF the thing we're inverting
      generateFormattedInstruction ("CONSTI 0");
      genCodeForExpression (expressionNode->children);
      if (exprType == NT_INT_INVERSION) {
          generateFormattedInstruction ("SUBI");
      }
      else {
          generateFormattedInstruction ("SUBR");
      }
    }
    //TODO: Now do this for reals
    else {
        printf ("Error. Tried to gernerate code for an operation when it was not an operation\n");
        printf ("is of type: %d\n", getNiceType (expressionNode));
    }
    
}

void genCodeForIntComparison (GNode *expressionNode) {
//     printf ("Int comparison called\n");
    node_type exprType = getNiceType (expressionNode);
    GNode *leftExpressionNode = expressionNode->children;
    GNode *rightExpressionNode = leftExpressionNode->next;
    genCodeForExpression (leftExpressionNode);
    genCodeForExpression (rightExpressionNode);
    switch (exprType) {
        case NT_INT_ISEQUAL:
        {

          generateFormattedInstruction ("EQI");
          break;

        }
        case NT_INT_NOTEQUAL:
        {          

          generateFormattedInstruction ("EQI");
          generateFormattedInstruction ("NOT");
          break;
          
        }
        case NT_INT_LESSTHAN:
        {
          generateFormattedInstruction ("LTI");
          break;
        }
        case NT_INT_GREATERTHAN:
        {

          generateFormattedInstruction ("GTI");
          break;

        }
        case NT_INT_LESSTHANEQUALS:
        { 
          generateFormattedInstruction ("LTI");
          genCodeForExpression (leftExpressionNode);
          genCodeForExpression (rightExpressionNode);
          generateFormattedInstruction ("EQI");
          generateFormattedInstruction ("OR");
          break;
        }
        case NT_INT_GREATERTHANEQUALS:
        {
          generateFormattedInstruction ("GTI");
          genCodeForExpression (leftExpressionNode);
          genCodeForExpression (rightExpressionNode);
          generateFormattedInstruction ("EQI");
          generateFormattedInstruction ("OR");
          break;
        }
    }
    //Check if we have a real parent so we need to convert to real
    GNode *operationNode = getFirstOperationParent (expressionNode->parent);
    if (operationNode == NULL) {
        printf ("Operation node is null\n");
    }
    else {
        node_type type = getNiceType (operationNode);
        printf ("Inside int math: Type of node is: %d\n", type);
        if ((type >= NT_REAL_ISEQUAL) && (type <=NT_REAL_INVERSION)) {
            printf ("We have a real operation\n");
            generateFormattedInstruction ("ITOR");
        }
        
    }
    
}


void genCodeForRealComparison (GNode *expressionNode) {
    node_type exprType = getNiceType (expressionNode);
    GNode *leftExpressionNode = expressionNode->children;
    GNode *rightExpressionNode = leftExpressionNode->next;
    genCodeForExpression (leftExpressionNode);
    genCodeForExpression (rightExpressionNode);
    switch (exprType) {
        case NT_REAL_ISEQUAL:
        {
            
            generateFormattedInstruction ("EQR");
            break;
            
        }
        case NT_REAL_NOTEQUAL:
        {          
            
            generateFormattedInstruction ("EQR");
            generateFormattedInstruction ("NOT");
            break;
            
        }
        case NT_REAL_LESSTHAN:
        {
            generateFormattedInstruction ("LTR");
            break;
        }
        case NT_REAL_GREATERTHAN:
        {
            
            generateFormattedInstruction ("GTR");
            break;
            
        }
        case NT_REAL_LESSTHANEQUALS:
        { 
            generateFormattedInstruction ("LTR");
            genCodeForExpression (leftExpressionNode);
            genCodeForExpression (rightExpressionNode);
            generateFormattedInstruction ("EQR");
            generateFormattedInstruction ("OR");
            break;
        }
        case NT_REAL_GREATERTHANEQUALS:
        {
            generateFormattedInstruction ("GTR");
            genCodeForExpression (leftExpressionNode);
            genCodeForExpression (rightExpressionNode);
            generateFormattedInstruction ("EQR");
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
          expressionNode = expressionNode->children;
          genCodeForExpression (expressionNode);
          generateFormattedInstruction("NOT");
          break;
        }
    }
}

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
    printf ("In int math\n");
    //Check if we have a real parent so we need to convert to real
    GNode *operationNode = getFirstOperationParent (expressionNode->parent);
    if (operationNode == NULL) {
        printf ("Operation node is null\n");
    }
    else {
        node_type type = getNiceType (operationNode);
        printf ("Inside int math: Type of node is: %d\n", type);
        if ((type >= NT_REAL_ISEQUAL) && (type <=NT_REAL_INVERSION)) {
            printf ("We have a real operation\n");
            generateFormattedInstruction ("ITOR");
        }
        
    }
    
}


void genCodeForRealMath (GNode *expressionNode) {
    printf ("In gen code for real math\n");
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

void genVarAdjust (int value) {
    if (value == 0)
        return;
    char instruction [strlen ("ADJUST -") + 11];
    sprintf (instruction, "ADJUST -%d", value);
    generateFormattedInstruction (instruction);
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

void genBranch (char const *label) {
//     printf ("Adding new label: %s to be label%d
    addLabel (label);
    char *outputLabel = g_hash_table_lookup (masterLabelTable, label);
    char instruction [strlen ("IFZ ") + 16];
    sprintf (instruction, "IFZ %s", outputLabel);
    generateFormattedInstruction (instruction);
    
    //           char instruction [strlen (label) + strlen ("IFZ")];
    //           sprintf (instruction, "IFZ %s", label);
}

void genGOTO (char const *label) {
    addLabel (label);
    printf ("In gen goto\n");
    printf ("label :%s\n", label);
    char instruction [strlen ("GOTO") + 256];
    char *outputtedLabel = g_hash_table_lookup (masterLabelTable, label);
    printf ("outputlabel: %s\n", outputtedLabel);
    sprintf (instruction, "GOTO %s", outputtedLabel);
    generateFormattedInstruction (instruction);
}

void genVarAssign (varAddressStruct *addressDescription, int subscript) {
    printf ("In gen var assign\n");
    //printf ("Address of addressDescription: %p\n", addressDescription);
    int indexingRegister = addressDescription->indexingRegister;
    //programs are absolute, no index.

    int offset = addressDescription->offset + subscript;
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

void genVarParamAssign (varAddressStruct *addressDescription) {
    printf ("In gen var param assign\n");
    int indexingRegister = addressDescription->indexingRegister;
    int offset = addressDescription->offset;
    char instruction [strlen ("PUSHA []") + 32];
    if (indexingRegister < 0) {
      sprintf (instruction, "PUSHA %d", offset);
      generateFormattedInstruction (instruction);
    }
    else {
      sprintf (instruction, "PUSHA %d[%d]", offset, indexingRegister);
      generateFormattedInstruction (instruction);
    }

}
void genVarAccess (varAddressStruct *addressDescription) {
    printf ("Calling gen var access\n");
    int indexingRegister = addressDescription->indexingRegister;
    int offset = addressDescription->offset;
    char instruction [strlen ("PUSH []") + 32];
    
    if (indexingRegister < 0) {
        sprintf (instruction, "PUSH %d", offset);
        generateFormattedInstruction (instruction);
    }
    else {
        sprintf (instruction, "PUSH %d[%d]", offset, indexingRegister);
        generateFormattedInstruction (instruction);
        
    }
}
void genVarParam (varAddressStruct *addressDescription) {
    int indexingRegister = addressDescription->indexingRegister;
    int offset = addressDescription->offset;
    char instruction [strlen ("PUSHA []") + 32];
    if (indexingRegister < 0) {
      sprintf (instruction, "PUSHA %d", offset);
      generateFormattedInstruction (instruction);
    }
    else {
      sprintf (instruction, "PUSHA %d[%d]", offset, indexingRegister);
      generateFormattedInstruction (instruction);
    }
}

void genVarParamAccess (varAddressStruct *addressDescription) {
    int indexingRegister = addressDescription->indexingRegister;
    int offset = addressDescription->offset;
    char instruction [strlen ("PUSHA []") + 32];
    if (indexingRegister < 0) {
        sprintf (instruction, "PUSHA %d", offset);
        generateFormattedInstruction (instruction);
    }
    else {
        sprintf (instruction, "PUSHA %d[%d]", offset, indexingRegister);
        generateFormattedInstruction (instruction);
    }
    generateFormattedInstruction ("PUSHI");
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
    char *outputLabel = g_hash_table_lookup (masterLabelTable, procedureInfo->procLabel);
    sprintf (instruction, "CALL %d %s", procedureInfo->indexingRegister, outputLabel);
    generateFormattedInstruction (instruction);
}

void genProcReturn (procInfo *procedureInfo) {
    
    char label [strlen (procedureInfo->procLabel) + strlen("end")];
    sprintf (label, "%send", procedureInfo->procLabel);
    generateLabel (label);
    int numWords = procedureInfo->numVarWords;
    //TODO: GEnerate adjust!
    genVarAdjust (numWords);
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


/** 
 * Add label if it doesn't exist in the table. If it does,
 * do nothing
 */
void addLabel (char const *labelName) {
   gpointer *key = (gpointer) labelName;
   char *returnedLabel = g_hash_table_lookup(masterLabelTable, labelName);
   if (returnedLabel == NULL) {
       char *outputtedLabel = calloc (MAX_LABEL_LEN, sizeof (char));
       sprintf (outputtedLabel, "lab%d", masterLabelCount);
       g_hash_table_insert (masterLabelTable, key, outputtedLabel);
       masterLabelCount++;
   }

}
void generateLabel (const char *labelName) {
    //TODO: Make it so the labelName is hashed, and if it is null, then we add
    //put a comment with the label name
    generateComment (labelName);

    char *outputtedLabel = g_hash_table_lookup (masterLabelTable, labelName);
    if (outputtedLabel == NULL) {
//         gpointer *key = (gpointer) labelName;
//         outputtedLabel = calloc (MAX_LABEL_LEN, sizeof (char));
//         sprintf (outputtedLabel, "lab%d",  masterLabelCount);
//         fprintf (output, "%s\n", outputtedLabel);
//         g_hash_table_insert (masterLabelTable, key, outputtedLabel);
//         masterLabelCount++;

        addLabel (labelName);
        outputtedLabel = g_hash_table_lookup (masterLabelTable, labelName);
        
    }

        fprintf (output, "%s\n", outputtedLabel);
    
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
 * Special functions to handle writeln and readln
 * ln is a flag saying if it is writeln or write, or readln and read
 * paramNode is the first parameter passed in, to get the rest, call paramNode->next
 */
void genCodeForWrite(GNode *paramNode, int ln) {
    int numWordsAllocated = 0;
    printf ("In gen code for write\n");
    while (paramNode != NULL) {
        printf ("Type of param: %d\n", getExpressionType (paramNode));
        type_class returnedType = getExpressionType (paramNode);
        switch (returnedType) {
            case TC_INTEGER:
            {
//                 printf ("Write I param is a what param: %d\n", paramNode
                genCodeForExpression (paramNode);
                generateFormattedInstruction ("WRITEI");
                //numWordsAllocated++;
                break;
            }
            case TC_REAL:
            {
                genCodeForExpression (paramNode);
                generateFormattedInstruction ("WRITER");
                //numWordsAllocated++;
                break;
            }
            case TC_STRING: 
            {
                break;
            }
            default:
            {
            }
        }
        paramNode = paramNode->next;
    }
    
    if (ln) {
        //10 is newline
        generateFormattedInstruction ("CONSTI 10");
        generateFormattedInstruction ("WRITEC");
    }
//     genVarAdjust (numWordsAllocated);
    
}

void genCodeForRead (GNode *paramNode, int ln) {
    printf ("In gen code for read\n");
    while (paramNode != NULL) {
        type_class returnedType = getExpressionType (paramNode);
        switch (returnedType) {
            case TC_INTEGER:
            {
                //genVarParamAssign ()
                genCodeForExpression (paramNode);
                generateFormattedInstruction("READI");
                generateFormattedInstruction("POPI");
//                 if (ln) {
//                     generateFormattedInstruction("READC");
//                     generateFormattedInstruction("CONSTI 10");
//                     generateFormattedInstruction ("SUBI");
//                     generateFormattedInstruction ("NOT");
//                     genBranch ("readlnerror");
//                     
//                 }

                break;
            }
            case TC_REAL:
            {
                genCodeForExpression (paramNode);
                generateFormattedInstruction ("READR");
                generateFormattedInstruction ("POPI");
                if (ln) {
                    generateFormattedInstruction("READC");
                    generateFormattedInstruction("CONSTI 10");
                    generateFormattedInstruction ("SUBI");
                    generateFormattedInstruction ("NOT");
                    genBranch ("readlnerror");
                    
                }
                break;
            }
            case TC_STRING:
            {
                generateFormattedInstruction ("READC");
                break;
            }
            default:
            {
            }
        }
        
        paramNode = paramNode->next;
    }
    
//     if (ln) {
//         //10 is newline
//         generateFormattedInstruction ("CONSTI 10");
//         generateFormattedInstruction ("WRITEC");
//     }
}

type_class getExpressionType (GNode *head) {
    //go through whole tree, and find a node with node type of NT_VAR or NT_CONST
//     GNode *
//     printf ("In get expression type\n");
    type_class returnType = TC_INTEGER;
    if (head->children == NULL) {
        //printf("type at leaf: %d\n", getNiceType(head));
        return getTypeClass (getSymbol (head));
    }
    GNode *sibling = head->children;

    while (sibling != NULL) {
        type_class returnedType = getExpressionType (sibling);
        if (returnedType == TC_REAL) {
            returnType = returnedType;
        }
        else if (returnedType == TC_ARRAY) {
            symbol *arraySymbol = getSymbol (sibling);
            
            returnType == returnedType;
        }
        else {
            if (returnedType != TC_INTEGER) {
                printf ("Got this return type: %d\n", returnedType);
            }
        }
        sibling = sibling->next;
    }
        
        return returnType;


}
