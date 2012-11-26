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
 struct param_desc *createParamDesc (int varParam) {
  struct param_desc *paramDesc = calloc (1, sizeof (struct param_desc));
  paramDesc->varParam = varParam;
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
    //The other ones are not basic types, so don't need a type description
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

    DEBUG_PRINT (("In get type class\n"));
    symbol *tempSymbol = theSymbol;

    while (tempSymbol != NULL){
      DEBUG_PRINT (("temp SYmbol address: %p\n", tempSymbol));
      DEBUG_PRINT (("temp symbol oc: %d\n", tempSymbol->oc));
    //printf ("Symbol name: %s\n", tempSymbol->name);
    //printf ("Object class: %d\n", tempSymbol->oc);
      if (tempSymbol->oc == OC_TYPE) {
	DEBUG_PRINT(("type description: %p\n", tempSymbol->desc.type_attr));
        return tempSymbol->desc.type_attr->type;
      }
      tempSymbol = tempSymbol->symbol_type;
    } 
    DEBUG_PRINT (("Hm...symbol did not point to any type\n"));
  }
 
 
symbol *createErrorSym(object_class objClass) {
    symbol *errSym = calloc(1, sizeof(symbol));
    errSym->oc = objClass;
    errSym->symbol_type = createErrorType();
    
    return errSym;
}
 
/**
Create a symbol with parameters: identifier, 
symbol type (what type the symbol is), object class oc,
pointer * value
*/
symbol *createSymbol (char const *identifier, symbol *type, 
object_class oc, void *value){
  
  /* Create a symbol with name, object class, type class*/
  
    //DEBUG_PRINT(("Inside create symbol type\n"));
    
    symbol *newSymbol = calloc(1, sizeof(symbol));
    newSymbol->name = identifier;
    newSymbol->oc = oc;
    newSymbol->symbol_type = type;

    if (value == NULL) {
    //TODO: Should there ever be a time where
    // the description of a symbol is null?
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
  *
  * Actions to take on the symbol table itself
  *
  */
 
/**
 Our symbol table will need multiple levels, so we will
 need to add and remove levels as a new scope is defined
 level is incremented by 1
*/
void pushLevel () {
  //the level is equal to the level we will be at currently
  if (level >MAX_LEVEL) {
    iserror = 1;
    //printf ("Level: %d\n", level);
    //showAllSymbols ();
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
  
  /* Basic types */
  addSymbol("char", createSymbol("char", NULL, OC_TYPE, createTypeDesc(TC_CHAR)));
  symbol *boolean = createSymbol("boolean", NULL, OC_TYPE, createTypeDesc(TC_BOOLEAN));
  addSymbol("boolean", boolean);
  symbol *integer = createSymbol("integer", NULL, OC_TYPE, createTypeDesc(TC_INTEGER));
  addSymbol("integer", integer);
  symbol *real = createSymbol("real", NULL, OC_TYPE, createTypeDesc(TC_REAL));
  addSymbol("real", real);
  
  /* Constants */
  union constant_values trueval = { .boolean = TRUE_VALUE };
  addSymbol("true", createSymbol("true", boolean, OC_CONST, createConstDesc(trueval)));
  union constant_values falseval = { .boolean = FALSE_VALUE };
  addSymbol("false", createSymbol("false", boolean, OC_CONST, createConstDesc(falseval)));
  union constant_values maxintval = { .integer = MAX_INT_VALUE };
  addSymbol("maxint", createSymbol("maxint", integer, OC_CONST, createConstDesc(maxintval)));
  union constant_values pival = { .real = PI_VALUE };
  addSymbol("pi", createSymbol("pi", real, OC_CONST, createConstDesc(pival)));
  
  /* Procedures */
  addBuiltinProc("writeln", addParam(NULL, NULL));
  addBuiltinProc("write", addParam(NULL, NULL));
  addBuiltinProc("readln", addParam(NULL, NULL));
  addBuiltinProc("read", addParam(NULL, NULL));

  /* Functions */
  addBuiltinFunc("abs", globalLookup("integer"), addFuncParam(globalLookup("integer"))); // check later
  addBuiltinFunc("chr", globalLookup("char"), addFuncParam(globalLookup("integer"))); // implemented
  addBuiltinFunc("cos", globalLookup("real"), addFuncParam(globalLookup("real")));
  addBuiltinFunc("ln", globalLookup("real"), addFuncParam(globalLookup("real")));
  addBuiltinFunc("odd", globalLookup("boolean"), addFuncParam(globalLookup("integer"))); // implemented
  addBuiltinFunc("ord", globalLookup("integer"), addFuncParam(globalLookup("integer"))); // check later
  addBuiltinFunc("pred", globalLookup("integer"), addFuncParam(globalLookup("integer"))); // check later
  addBuiltinFunc("round", globalLookup("integer"), addFuncParam(globalLookup("real"))); // implemented
  addBuiltinFunc("sin", globalLookup("real"), addFuncParam(globalLookup("real")));
  addBuiltinFunc("sqr", globalLookup("integer"), addFuncParam(globalLookup("integer"))); // check later
  addBuiltinFunc("sqrt", globalLookup("real"), addFuncParam(globalLookup("real")));
  addBuiltinFunc("succ", globalLookup("integer"), addFuncParam(globalLookup("integer"))); // check later
  addBuiltinFunc("exp", globalLookup("real"), addFuncParam(globalLookup("real")));
  addBuiltinFunc("trunc", globalLookup("integer"), addFuncParam(globalLookup("real"))); // implemented

}


//return a symbol with the result of the evaluation
//we are assuming here that type checking has been done, so this will
//always return successfully

symbol *evaluateBuiltin(const char *name, symbol *arg) {

    struct const_desc *argumentDescription = arg->desc.const_attr;

    if (strcmp (name, "odd") == 0) {
        int argument = argumentDescription->value.integer;
        int result = argument % 2;
        union constant_values resultValue = {.boolean = result};
        return createConstant(TC_BOOLEAN, resultValue);
    }
    else if (strcmp (name, "chr")== 0) {
        int argument = argumentDescription->value.integer;
        if (argument > MAX_CHAR_VALUE || argument < 0) {
            addTypeError ("there is no valid character for this integer value");
            return createErrorSym(OC_CONST);
            //error, this is not a valid "byte"
            //we are faking bytes since there are no bytes in pascal
        }
        char result = (char) argument;
        //printf ("Here is the result: %c\n", result);
        union constant_values resultValue = {.character = result};
        return createConstant (TC_CHAR, resultValue);        
    }
    else if (strcmp (name, "round") == 0) {
        double argument = argumentDescription->value.real;
        int result = myround (argument);
        union constant_values resultValue = {.integer = result};
        return createConstant (TC_INTEGER, resultValue);
    }
    else if (strcmp (name, "trunc") ==0) {
        double argument = argumentDescription->value.real;
        int result = (int) argument;
        union constant_values resultValue = {.integer = result};
        return createConstant (TC_INTEGER, resultValue);
    }
    else if (strcmp (name, "ord") == 0) {
        int argument = argumentDescription->value.integer;

        if (argument < 0) {
            addTypeError ("ord cannot be called on a negative value");
            return createErrorSym(OC_CONST);
        }
        union constant_values resultValue = {.integer = argument};
        return createConstant (TC_INTEGER, resultValue);
    }
    else if (strcmp (name, "pred") == 0) {
        type_class tc = getTypeClass(arg);
        
        if (tc == TC_INTEGER) {
            int value = argumentDescription->value.integer;
            
            if (value <= (-1 * MAX_INT_VALUE)) {
                addTypeError("there is no valid predecessor of this integer");
                return createErrorSym(OC_CONST);
            }
            
            union constant_values resultValue = {.integer = (value - 1)};
            return createConstant(TC_INTEGER, resultValue);
        } else if (tc == TC_BOOLEAN) {
            int value = argumentDescription->value.boolean;
            
            if (value <= 0) {
                addTypeError("there is no valid predecessor of this boolean");
                return createErrorSym(OC_CONST);
            }
            
            return globalLookup("false");
            
        } else if (tc == TC_CHAR) {
            int value = argumentDescription->value.character;
            
            if (value <= 0) {
                addTypeError("there is no valid predecessor of this character");
                return createErrorSym(OC_CONST);
            }
            
            union constant_values resultValue = {.character = (char) (value - 1)};
            return createConstant(TC_CHAR, resultValue);
        }
        // tc == TC_SCALAR
        int value = argumentDescription->value.integer;
        
        if (value <= 0) {
            addTypeError("there is no valid predecessor of this scalar");
            return createErrorSym(OC_CONST);
        }
        
        union constant_values resultValue = {.integer = (value - 1)};
        return createSymbol(NULL, arg->symbol_type, OC_CONST, (void *) createConstDesc(resultValue));
    }
    else if (strcmp (name, "succ") == 0) {
        type_class tc = getTypeClass(arg);
        
        if (tc == TC_INTEGER) {
            int value = argumentDescription->value.integer;
            
            if (value >= MAX_INT_VALUE) {
                addTypeError("there is no valid successor of this integer");
                return createErrorSym(OC_CONST);
            }
            
            union constant_values resultValue = {.integer = (value + 1)};
            return createConstant(TC_INTEGER, resultValue);
        } else if (tc == TC_BOOLEAN) {
            int value = argumentDescription->value.boolean;
            
            if (value >= 1) {
                addTypeError("there is no valid successor of this boolean");
                return createErrorSym(OC_CONST);
            }
            
            return globalLookup("true");
            
        } else if (tc == TC_CHAR) {
            int value = argumentDescription->value.character;
            
            if (value >= MAX_CHAR_VALUE) {
                addTypeError("there is no valid successor of this character");
                return createErrorSym(OC_CONST);
            }
            
            union constant_values resultValue = {.character = (char) (value + 1)};
            return createConstant(TC_CHAR, resultValue);
        }
        // tc == TC_SCALAR
        int value = argumentDescription->value.integer;
        GPtrArray *constList = arg->symbol_type->desc.type_attr->desc.scalar->const_list;
        int listLen = (constList == NULL) ? 0 : (constList->len - 1);
        
        if (value >= listLen) {
            addTypeError("there is no valid successor of this scalar");
            return createErrorSym(OC_CONST);
        }
        
        union constant_values resultValue = {.integer = (value + 1)};
        return createSymbol(NULL, arg->symbol_type, OC_CONST, (void *) createConstDesc(resultValue));
    }
    else if (strcmp (name, "abs") == 0) {
        type_class tc = getTypeClass (arg);
        if (tc == TC_REAL) {
            double argument = argumentDescription->value.real;
            if (argument < 0) {
                argument = argument * -1;
            }
            union constant_values resultValue = {.real = argument};
            return createConstant (TC_REAL, resultValue);
        }
        else if (tc == TC_INTEGER) {
            int argument = argumentDescription->value.integer;
            if (argument < 0) {
                argument = argument * -1;
            }
            union constant_values resultValue = {.integer = argument};
            return createConstant (TC_INTEGER, resultValue);
        }
    }
    else if (strcmp (name, "sqr") == 0) {
        type_class tc = getTypeClass (arg);
        if (tc == TC_REAL) {
            double argument = argumentDescription->value.real;
            double result = argument * argument;
       
            union constant_values resultValue = {.real = result};
            return createConstant (TC_REAL, resultValue);
        }
        else if (tc == TC_INTEGER) {
            int argument = argumentDescription->value.integer;

            int result = argument * argument;

            union constant_values resultValue = {.integer = result};
            return createConstant (TC_INTEGER, resultValue);
        }        
    }
    else {
        //BAD, you should never call a builtin that does
        //not exist
    }
        
}

int myround(double number)
{
    return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}
symbol *addBuiltinProc(const char *id, GPtrArray *paramList) {    
  struct tc_none *noneType = calloc(1, (sizeof(struct tc_none)));
    struct type_desc *typeDesc = calloc(1, (sizeof(struct type_desc)));
    typeDesc->type = TC_NONE;
    typeDesc->desc.none = noneType;

    
    symbol *type = createTypeSym(NULL, typeDesc);
    symbol *newProc = createSymbol(id, type, OC_PROC, createProcedureDesc(paramList));

    addSymbol(id, newProc);

    return newProc;
}

GPtrArray *addFuncParam(symbol *paramType){
  /* Only for adding a single parameter */
  GPtrArray *param = g_ptr_array_new();
  g_ptr_array_add(param, createSymbol(NULL, paramType, OC_PARAM, createParamDesc(0)));
  return param;
}
symbol *addBuiltinFunc(const char *id, symbol *returnType, GPtrArray *paramList) {
  symbol *paramType;
  symbol *newFunc = createSymbol(id, returnType, OC_FUNC,
                                   createFunctionDesc(paramList, returnType));

    /* Tease out parameter - all built-ins have just one, so we go with that */
//    if(paramList)
  //    paramType = ((symbol *) g_ptr_array_index(paramList,0))->symbol_type;
      
    addSymbol(id, newFunc);
    
    return newFunc;
}


/**
 *Symbol table memory management functions
 */
 
 
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
