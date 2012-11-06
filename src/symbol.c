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

GQueue* symbol_table = NULL;
int level = -1;

int func_counter = 1;

/**
addSymbol always adds the symbol to the topmost level
*/

symbol *addSymbol (char const *identifier, symbol *symbol) {
#if DEBUG
    printf("DEBUG: add symbol called\n");
#endif
  GHashTable *table = g_queue_peek_head (symbol_table);
  g_hash_table_insert (table, identifier, symbol);

}
symbol *localLookup (char const *identifier) {
  GHashTable *table = g_queue_peek_head (symbol_table);
  symbol *returnedSymbol = g_hash_table_lookup (table, identifier);
  return returnedSymbol;
}
symbol *globalLookup (char const *identifier) {
#if DEBUG
  printf("DEBUG: In global lookup\n");
#endif
  int numLevels = g_queue_get_length(symbol_table);
  symbol *returnedSymbol = NULL;
  while (numLevels > 0) {
    GHashTable *table = g_queue_peek_head (symbol_table);
    returnedSymbol = g_hash_table_lookup (table, identifier);
    if (returnedSymbol != NULL) {
      return returnedSymbol;
    }
    numLevels--;

  }
  return returnedSymbol;
}

void showAllSymbols() {
}

/**
Create a symbol with parameters: identifier, type symbol, int object class,
pointer * value
*/
symbol *createSymbol (char const *identifier, symbol *type, 
int obj_class, void *value) {
}

/**
 * Create a symbol with parameters: identifier, anonymous type type_desc, int
 * object class, pointer value.
 */
symbol *createSymbolAnonType (char const *identifier, struct type_desc *type,
object_class oc, void *value) {
}


symbol *createSymbolType (char const *identifier, type_class type) {

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
      typeDescription->desc.real = boolDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_CHAR:
      typeDescription->type = type;
      struct tc_char *charDescription = calloc (1, sizeof (struct tc_char));
      charDescription->len = 8;
      typeDescription->desc.real = charDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;      
    case TC_STRING:
      typeDescription->type = type;
      struct tc_string *stringDescription = calloc (1, sizeof (struct tc_string));
      stringDescription->len = 32;
      typeDescription->desc.real = stringDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_SCALAR:
      typeDescription->type = type;
      struct tc_scalar *scalarDescription = calloc (1, sizeof (struct tc_scalar));
      scalarDescription->len = 32;
      typeDescription->desc.real = scalarDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_ARRAY:
      typeDescription->type = type;
      struct tc_array *arrayDescription = calloc (1, sizeof (struct tc_array));
      arrayDescription->len = 32;
      typeDescription->desc.real = arrayDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;    
    case TC_RECORD:
      typeDescription->type = type;
      struct tc_record *recordDescription = calloc (1, sizeof (struct tc_record));
      recordDescription->len = 32;
      typeDescription->desc.real = recordDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;
    case TC_SUBRANGE:
      typeDescription->type = type;
      struct tc_subrange *subrangeDescription = calloc (1, sizeof (struct tc_subrange));
      subrangeDescription->len = 32;
      typeDescription->desc.real = subrangeDescription;
      typeSymbol->desc.type_attr = typeDescription;
      break;    
  }
  
  struct symbol_rec {
  char *name;         /* Name of symbol */
  object_class oc;     /* Class of object (eg. OC_CONST) */
  union {             /* Class-specific attributes */
    struct const_desc *const_attr;
    struct var_desc *var_attr;
    struct function_desc *func_attr;
    struct procedure_desc *proc_attr;
    struct param_desc *parm_attr;
    struct type_desc *type_attr;
  }desc;
};

}

/**
 Our symbol table will need multiple levels, so we will
 need to add and remove levels as a new scope is defined
 level is incremented by 1
*/
void pushLevel () {
  level++;
  GHashTable *table = createNewTable (level);
  g_queue_push_head (symbol_table, table);

  

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
  
#if DEBUG
    printf("DEBUG: In init table\n");
#endif
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
