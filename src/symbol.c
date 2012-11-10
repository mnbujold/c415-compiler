/**
 * Author: Daniel Chui, Mike Bujold
 * Implementation of Symbol table
 * Uses Glib library
 */
 
 
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

//#include "builtin.h"  //separate builtins initalization from rest of this

#include "symbol.h"
#include "type.h"
#include "typeerrors.h"
#include "myerror.h"
#include "debug.h"


extern myerror *eList;
extern int iserror;
extern int lineno;
extern int last_column;

GQueue* symbol_table = NULL;
int level = -1;

int func_counter = 1;

/**
addSymbol always adds the symbol to the topmost level
*/
void *addSymbol (char const *identifier, symbol *symbol) {

  gpointer *key = (gpointer) identifier;
  /* Table is equal to the hashmap at head of the queue */
  GHashTable *table = g_queue_peek_head(symbol_table);
  if (table == NULL) {
    DEBUG_PRINT(("Error: Symbol table is null for some reason\n"));
  }
  /* Insert symbol at key in hashmap at head of queue */
  g_hash_table_insert (table, key, symbol);

}

//Is returning null for a null identifier the right thing to do?
symbol *localLookup (char const *identifier) {
  if (identifier == NULL) {
    return NULL;
  }
  GHashTable *table = g_queue_peek_head (symbol_table);
  symbol *returnedSymbol = g_hash_table_lookup (table, identifier);
  return returnedSymbol;
}

symbol *globalLookup (char const *identifier) {
    if (identifier == NULL) {
        return NULL;
    }
    
  int numLevels = g_queue_get_length(symbol_table);

  symbol *returnedSymbol = NULL;
  int i = 0;
  while (i < numLevels) {
    GHashTable *table = g_queue_peek_nth (symbol_table, i);
    if (table == NULL) {
	//Would this ever happen?
    }

    returnedSymbol = g_hash_table_lookup (table, identifier);


    if (returnedSymbol != NULL) {
      return returnedSymbol;
    }

    i++;

  }

  return returnedSymbol;
}

symbol *topLevelLookup (char const *identifier) {
  GHashTable *table = g_queue_peek_tail (symbol_table);
  symbol *returnedSymbol = g_hash_table_lookup (table, identifier);
  return returnedSymbol;
}

/**
 * Returns found variable symbol, or returns an error type and emits an error.
 */
symbol *getVarSymbol(char const *id) {
    symbol *varSym = globalLookup(id);
    
    if (varSym == NULL) {
        symNotDefinedError(id);
        return createErrorSym(OC_VAR);
    }
    object_class objClass = varSym->oc;

    if (objClass == OC_FUNC) {
        return createSymbol(id, varSym->symbol_type, OC_VAR, createVarDesc());
    }
    
    if (objClass != OC_VAR && objClass != OC_PARAM && objClass != OC_CONST) {    
      //TODO: WHat is this?
        symNotAVarParmConstError(id);
        return createErrorSym(OC_VAR);
    }
    
    return varSym;
}

/**
 *  Debug function: When called, this will print out the entire symbol table
 */

void iterator (gpointer key, gpointer value, gpointer user_data) {
  char *identifier = (char *) key;
  symbol *recordPointer = (symbol *) value;
  //NOTE: THIS IS BAD, oc is an ENUM. But whatever
  int oc = recordPointer->oc;
  printf("Name: '%-10s', %p ", recordPointer->name, recordPointer->name);
  printf ("\tAddress: %p", recordPointer);
  printf ("\tKEY: '%s' ", identifier);
  printf ("\tObject class: %d", oc); 
  printf ("\tTYPE: %p; ", recordPointer->symbol_type);
  printf ("\tTYPE CLASS: %d", getTypeClass (recordPointer));
  
//printf ("Symbol attributes: %p\n", recordPointer->desc);
/*
  if (recordPointer->symbol_type != NULL) {
    //printf ("Symbol type: %p ", recordPointer->symbol_type);
    if (recordPointer->symbol_type->name != NULL) {
      printf ("Symbol type: '%s'\n", recordPointer->symbol_type->name); 
    }
  }
  */

  /*
  if (oc==OC_TYPE) {
    struct type_desc *typeDescription = recordPointer->desc.type_attr;
    int tc = typeDescription->type;
    printf ("TYPE: %d\n", tc);
  }*/
  printf ("\n");
}

void printLevel(int level){

  printf("LEVEL %d ============================\n", level-1);
  GHashTable *table = g_queue_peek_nth(symbol_table, level-1);
  g_hash_table_foreach(table, (GHFunc)iterator, NULL);
  printf("\nEND LEVEL %d ========================\n", level-1);
  
}

void printSymbol(){
  
}
void showAllSymbols() {

  int numLevels = g_queue_get_length (symbol_table);
  //printf ("Number of levels: %d\n", numLevels);
  //printf ("LEvl: %d\n", level);
  int tmp = level;
  printf ("=================SYMBOL TABLE ENTRIES==================\n");
  int i = 0;
  while (i < numLevels) {
    
    printf ("LEVEL %d\n", tmp-1);
    GHashTable *table = g_queue_peek_nth(symbol_table, i);
    g_hash_table_foreach(table, (GHFunc)iterator, NULL);
    
    i++;
    tmp--;
  }
  printf ("==================SYMBOL TABLE END=======================\n");
  
}



/**
 * Call to create an anonymous type symbol
 * Create a symbol with parameters: identifier, anonymous type type_desc, int
 * object class, pointer value.
 */
symbol *createSymbolAnonType (char const *identifier, struct type_desc *type,
object_class oc, void *value) {
  
  DEBUG_PRINT(("Inside create symbol anonymous type\n"));
  
  symbol *anonSymbol = calloc (1, sizeof (symbol));
  anonSymbol->name = identifier;
  anonSymbol->oc = oc;
  anonSymbol->desc.type_attr = type;
  return anonSymbol;
  
}

symbol *createSymbolFunction (char const *identifier, struct function_desc *functionDescription) {
  
  symbol *functionSymbol = calloc (1, sizeof (symbol));
  functionSymbol->name = identifier;
  functionSymbol->oc = OC_FUNC;
  //functionSymbol->type = type of return value of FP
  functionSymbol->desc.func_attr = functionDescription;
  if(functionDescription == NULL)
    functionSymbol->symbol_type = NULL;
  else
    functionSymbol->symbol_type = functionDescription->return_type;
  
  return(functionSymbol);
}

/**
 *  Call to create the PAL builtin types
 *  Perhaps we should actually move this to builtin.c
 */
symbol *createSymbolType (char const *identifier, type_class type) {

  DEBUG_PRINT (("in create symbol type type class: %d\n", type));
  symbol *typeSymbol = calloc (1, sizeof (symbol));
  typeSymbol->name = identifier;
  typeSymbol->oc = OC_TYPE; 
  struct type_desc *typeDescription = calloc (1, sizeof (struct type_desc));
  switch (type) {
    case TC_INTEGER:
      typeDescription->type = type;
      struct tc_integer *integerDescription = calloc (1, sizeof (struct tc_integer));
      integerDescription->len = 32;
      typeDescription->desc.integer = integerDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_REAL:
      typeDescription->type = type;
      struct tc_real *realDescription = calloc (1, sizeof (struct tc_real));
      realDescription->len = 32;
      typeDescription->desc.real = realDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_BOOLEAN:
      typeDescription->type = type;
      struct tc_boolean *boolDescription = calloc (1, sizeof (struct tc_boolean));
      boolDescription->len = 32;
      typeDescription->desc.boolean = boolDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_CHAR:
      typeDescription->type = type;
      struct tc_char *charDescription = calloc (1, sizeof (struct tc_char));
      charDescription->len = 8;
      typeDescription->desc.character = charDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;      
    case TC_STRING:
      typeDescription->type = type;
      struct tc_string *stringDescription = calloc (1, sizeof (struct tc_string));
      stringDescription->len = 32;
      typeDescription->desc.string = stringDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_SCALAR:
      typeDescription->type = type;
      struct tc_scalar *scalarDescription = calloc (1, sizeof (struct tc_scalar));
      scalarDescription->len = 32;
      typeDescription->desc.scalar = scalarDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_ARRAY:
      typeDescription->type = type;
      struct tc_array *arrayDescription = calloc (1, sizeof (struct tc_array));
      arrayDescription->len = 32;
      typeDescription->desc.array = arrayDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;    
    case TC_RECORD:
      typeDescription->type = type;
      struct tc_record *recordDescription = calloc (1, sizeof (struct tc_record));
      recordDescription->len = 32;
      typeDescription->desc.record = recordDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_SUBRANGE:
      typeDescription->type = type;
      struct tc_subrange *subrangeDescription = calloc (1, sizeof (struct tc_subrange));
      subrangeDescription->len = 32;
      typeDescription->desc.subrange = subrangeDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;    
  }
  

}

struct const_desc *createConstDesc (union constant_values value) {
   struct const_desc *constDec = calloc (1, sizeof (struct const_desc));
  //TODO: Does not actually assign value right now, need to change
   constDec->value = value;
   constDec->hasValue = 1;
   return constDec;
 }

 struct var_desc *createVarDesc () {
  struct var_desc *varDesc = calloc (1, sizeof (struct var_desc));
  return varDesc;
  
 }
 struct function_desc *createFunctionDesc (GPtrArray *params, symbol *return_type) {
  struct function_desc *funcDesc = calloc (1, sizeof (struct function_desc));
  funcDesc->params = params;
  funcDesc->return_type = return_type;
  return funcDesc;
 }
 struct procedure_desc *createProcedureDesc (GPtrArray *params) {
  struct procedure_desc *procDesc = calloc (1, sizeof (struct procedure_desc));
  procDesc->params = params;
  return procDesc;
 }
 struct param_desc *createParamDesc () {
  struct param_desc *paramDesc = calloc (1, sizeof (struct param_desc));
  return paramDesc;
 }
struct type_desc *createTypeDesc (type_class type) {
  struct type_desc *typeDesc = calloc (1, sizeof (struct type_desc));

  typeDesc->type = type;

  switch (type) {
    case TC_INTEGER:
    {
      struct tc_integer *integerDesc = calloc (1, sizeof (struct tc_integer));
      integerDesc->len = 32;
      typeDesc->desc.integer = integerDesc;
      break;
    }
    case TC_REAL:
    {
      struct tc_real *realDesc = calloc (1, sizeof (struct tc_real));
      realDesc->len = 32;
      typeDesc->desc.real = realDesc;
      break;
    }
    case TC_BOOLEAN:
    {
      struct tc_boolean *boolDesc = calloc(1, sizeof(struct tc_boolean));
      boolDesc->len = 32;
      typeDesc->desc.boolean = boolDesc;
      break;
    }
    case TC_CHAR:
    {
      struct tc_char *charDesc = calloc(1, sizeof(struct tc_char));
      charDesc->len = 32;
      typeDesc->desc.character = charDesc;
      break;
    }
    case TC_CONST:
    {

      break;
    }
    case TC_STRING:
    
      break;
    case TC_SCALAR:
    
      break;
    case TC_ARRAY:
    
      break;    
    case TC_RECORD:
    
      break;
    case TC_SUBRANGE:
    
      break;    
  }

  return typeDesc;
 }

  type_class getTypeClass (symbol *theSymbol) {

    //DEBUG_PRINT (("In get type class\n"));
    symbol *tempSymbol = theSymbol;

    while (tempSymbol != NULL){
      //DEBUG_PRINT (("temp SYmbol address: %p\n", tempSymbol));
      //DEBUG_PRINT (("temp symbol oc: %d\n", tempSymbol->oc));
    //printf ("Symbol name: %s\n", tempSymbol->name);
    //printf ("Object class: %d\n", tempSymbol->oc);
      if (tempSymbol->oc == OC_TYPE) {
	//DEBUG_PRINT(("type description: %p\n", tempSymbol->desc.type_attr));
        return tempSymbol->desc.type_attr->type;
      }
      tempSymbol = tempSymbol->symbol_type;
    } 
    //DEBUG_PRINT (("Hm...symbol did not point to any type\n"));
  }
 
 
symbol *createErrorSym(object_class objClass) {
    symbol *errSym = calloc(1, sizeof(symbol));
    errSym->oc = objClass;
    errSym->symbol_type = createErrorType();
    
    return errSym;
}
 
/**
Create a symbol with parameters: identifier, type symbol, object class oc,
pointer * value
*/
symbol *createSymbol (char const *identifier, symbol *type, object_class oc, void *value){
  
  /* Create a symbol with name, object class, type class*/
  
    //DEBUG_PRINT(("Inside create symbol type\n"));
    
    symbol *newSymbol = calloc(1, sizeof(symbol));
    newSymbol->name = identifier;
    newSymbol->oc = oc;
    newSymbol->symbol_type = type;

    if (value == NULL) {
      //do nothing
    }
    else {
      switch (oc) {
        case OC_CONST:
        {
          struct const_desc *description = (struct const_desc *) value;
          newSymbol->desc.const_attr = description;
          break;
        }
        case OC_VAR:
        {
          struct var_desc *description = (struct var_desc *) value;
          newSymbol->desc.var_attr = description;
          break;
        }  
        case OC_FUNC:
        {
          struct function_desc *description = (struct function_desc *) value;
          newSymbol->desc.func_attr = description;
          break;
        }
        case OC_PROC:
        {
          struct procedure_desc *description = (struct procedure_desc *) value;
          newSymbol->desc.proc_attr = description;
          break;
        }
        case OC_PARAM:
        {
          struct param_desc *description = (struct param_desc *) value;
          newSymbol->desc.parm_attr = description;
          break;
        }
        case OC_TYPE:
        {
          struct type_desc *description = (struct type_desc *) value;
          newSymbol->desc.type_attr = description;
          break;
        }
        case OC_PROGRAM:
          break;
        
      }
    }
    return newSymbol;
}
 
 
/**
 Our symbol table will need multiple levels, so we will
 need to add and remove levels as a new scope is defined
 level is incremented by 1
*/
void pushLevel () {
  if (level >=MAX_LEVEL) {
    iserror = 1;
    const char *str = "scope level is too big";
    eList = addError(eList, str, last_column, lineno);
   // return;
  }
//  GHashTable *table = createNewTable (level);
  g_queue_push_head (symbol_table, createNewTable(level));
  level++;
}


/**
 * Once a scope ends, pop the head of the symbol table
 * and decrement level by 1
 */
void popLevel () {
  GHashTable *table = g_queue_pop_head (symbol_table);
  g_hash_table_destroy (table); 
  level--;
}
/** 
Called to initialize the symbol table
*/
void init_table () {
  /* Create a new queue for symbol_table*/
  symbol_table = g_queue_new();
  /* Add a new level to the symbol table (queue) */
  pushLevel();
  
  //Add all the builtins here
  addSymbol("char", createSymbol("char", NULL, OC_TYPE, createTypeDesc(TC_CHAR)));
  addSymbol("boolean", createSymbol("boolean", NULL, OC_TYPE, createTypeDesc(TC_BOOLEAN)));
  addSymbol("integer", createSymbol("integer", NULL, OC_TYPE, createTypeDesc(TC_INTEGER)));
  addSymbol("real", createSymbol("real", NULL, OC_TYPE, createTypeDesc(TC_REAL)));
  /* Constants */
  union constant_values trueval = { .boolean = 1 };
  addSymbol("true", createSymbol("true", NULL, OC_CONST, createConstDesc(trueval)));
  union constant_values falseval = { .boolean = 0 };
  addSymbol("false", createSymbol("false", NULL, OC_CONST, createConstDesc(falseval)));
  union constant_values maxintval = { .integer = 2294967295 };
  addSymbol("maxint", createSymbol("maxint", NULL, OC_CONST, createConstDesc(maxintval)));
  union constant_values pival = { .real = 3.141592653 };
  addSymbol("pi", createSymbol("pi", NULL, OC_CONST, createConstDesc(pival)));

  /* Param lists for built-ins */
  addSymbol("abs_param", createSymbol("abs_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("chr_param", createSymbol("chr_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("cos_param", createSymbol("cos_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("exp_param", createSymbol("exp_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("ln_param", createSymbol("ln_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("odd_param", createSymbol("odd_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("ord_param", createSymbol("ord_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("pred_param", createSymbol("pred_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("round_param", createSymbol("round_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("sin_param", createSymbol("sin_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("sqr_param", createSymbol("sqr_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("sqrt_param", createSymbol("sqrt_param", NULL, OC_PARAM, createParamDesc()));
  addSymbol("succ_param", createSymbol("succ_param", NULL, OC_PARAM, createParamDesc()));
  
  /* Built-in functions */
  
  addSymbol("writeln", createSymbol("writeln", NULL, OC_FUNC, NULL)); 
  addSymbol("write", createSymbol("write", NULL, OC_FUNC, NULL));
  addSymbol("readln", createSymbol("readln", NULL, OC_FUNC, NULL));
  addSymbol("read", createSymbol("read", NULL, OC_FUNC, NULL));
  addSymbol("abs", createSymbol("abs", NULL, OC_FUNC, NULL));
  addSymbol("chr", createSymbol("chr", NULL, OC_FUNC, NULL));
  addSymbol("cos", createSymbol("cos", NULL, OC_FUNC, NULL));
  addSymbol("exp", createSymbol("exp", NULL, OC_FUNC, NULL));
  addSymbol("ln", createSymbol("ln", NULL, OC_FUNC, NULL));
  addSymbol("odd", createSymbol("odd", NULL, OC_FUNC, NULL));
  addSymbol("ord", createSymbol("ord", NULL, OC_FUNC, NULL));
  addSymbol("pred", createSymbol("pred", NULL, OC_FUNC, NULL));
  addSymbol("round", createSymbol("round", NULL, OC_FUNC, NULL));
  addSymbol("sin", createSymbol("sin", NULL, OC_FUNC, NULL));
  addSymbol("sqr", createSymbol("sqr", NULL, OC_FUNC, NULL ));
  addSymbol("sqrt", createSymbol("sqrt", NULL, OC_FUNC, NULL));
  addSymbol("succ", createSymbol("succ", NULL, OC_FUNC, NULL));

  
}
void free_symbol_table() {
  int numLevels = g_queue_get_length(symbol_table);
  
  while (numLevels > 0) {
    GHashTable *table = g_queue_pop_head (symbol_table);
    g_hash_table_destroy (table);
    numLevels--;
  }
  g_queue_free (symbol_table);
}
/**
 * Called to create a new symbol table
 * int level variable is currently unused
 */
GHashTable *createNewTable(int level) {
  return g_hash_table_new (g_str_hash, g_str_equal);
}
