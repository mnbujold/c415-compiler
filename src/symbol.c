/**
 * Author: Daniel Chui
 * Implementation of Symbol table
 * Uses Glib library
 */
 
 
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "builtin.h"  //separate builtins initalization from rest of this

#include "symbol.h"

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
  GHashTable *table = g_queue_peek_head (symbol_table);
  if (table == NULL) {
    DEBUG_PRINT(("Error: Symbol table is null for some reason\n"));
  }
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
 *  Debug function: When called, this will print out the entire symbol table
 */

void iterator (gpointer key, gpointer value, gpointer user_data) {
  char *identifier = (char *) key;
  symbol *recordPointer = (symbol *) value;
  //NOTE: THIS IS BAD, oc is an ENUM. But whatever
  int oc = recordPointer->oc;
  printf ("\n");
  //printf ("Address: %p", recordPointer);
  printf ("KEY: '%s' ", identifier);
  printf ("Name: %s, %p ", recordPointer->name, recordPointer->name);
  printf ("Object class: %d\n", oc); 
  if (recordPointer->symbol_type != NULL) {
    printf ("Symbol type: %p ", recordPointer->symbol_type);
    if (recordPointer->symbol_type->name != NULL) {
      printf ("Symbol name: %s\n", recordPointer->symbol_type->name); 
    }
  }
    
  if (oc==OC_TYPE) {
    struct type_desc *typeDescription = recordPointer->desc.type_attr;
    int tc = typeDescription->type;
    printf ("TYPE: %d\n", tc);
  }
  printf ("\n");
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
    printf ("\n");
  }
  printf ("=========================================================\n");
  
}


/**
Create a symbol with parameters: identifier, type symbol, object class oc,
pointer * value
*/
symbol *createSymbol (char const *identifier, symbol *type, 
object_class oc, void *value) {
    DEBUG_PRINT(("Inside create symbol type\n"));
    
    symbol *newSymbol = calloc(1, sizeof(symbol));
    newSymbol->name = identifier;
    newSymbol->oc = oc;
    newSymbol->symbol_type = type;
    
    return newSymbol;
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
  functionSymbol->symbol_type = functionDescription->return_type;
  
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
  return constDec;
 }
 struct var_desc *createVarDesc () {
  struct var_desc *varDesc = calloc (1, sizeof (struct var_desc));
  return varDesc;
  
 }
 struct function_desc *createFunctionDesc (GPtrArray *params, 
                                              symbol *return_type) {
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
 struct type_desc *createTypeDesc (type_class type, 
    union type_descriptions tc_desc) {
  struct type_desc *typeDesc = calloc (1, sizeof (struct type_desc));
  typeDesc->type = type;
  typeDesc->desc = tc_desc;
  return typeDesc;
 }
 
 
 
  type_class getTypeClass (symbol *theSymbol) {
    symbol *tempSymbol = theSymbol;
    while (tempSymbol->symbol_type != NULL) {
      if (tempSymbol->oc == OC_TYPE) {
        return tempSymbol->desc.type_attr->type;
      }
      tempSymbol = tempSymbol->symbol_type;
    }
  }
 
 
 
 
 
 
/**
 Our symbol table will need multiple levels, so we will
 need to add and remove levels as a new scope is defined
 level is incremented by 1
*/
void pushLevel () {
  if (level >=MAX_LEVEL) {
    iserror = 1;
    char *str = "Exceeded the number of levels allowed in ASC, will continue adding to top level\n";
    eList = addError(eList, str, last_column, lineno);
    printf ("Too many levels\n");
    return;
  }
  GHashTable *table = createNewTable (level);
  g_queue_push_head (symbol_table, table);
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
  
  GQueue* table_stack = g_queue_new();
  GHashTable *builtin_table = createNewTable(level);
  //q_queue_push_tail (table_stack, builtin_table);
  
  //Add all the builtins here: call builtins function
  symbol_table = table_stack;
  
  
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
  GHashTable *table = g_hash_table_new (g_str_hash, g_str_equal);
  return table;
}
