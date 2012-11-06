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
  GHashTable *table = g_queue_peek_head (symbol_table);
  g_hash_table_insert (table, identifier, symbol);

}
symbol *localLookup (char const *identifier) {
  GHashTable *table = g_queue_peek_head (symbol_table);
  symbol *returnedSymbol = g_hash_table_lookup (table, identifier);
  return returnedSymbol;
}
symbol *globalLookup (char const *identifier) {
  
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
int obj_class, void *value) {
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
    printf("In init table\n");
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
