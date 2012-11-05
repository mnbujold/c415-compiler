/**
 * Author: Daniel Chui
 * Modified from original file from Paul Vandermeer
 * Implementation of Symbol table header
 */
 
//I have no idea what Paul is doing here
#ifndef _symbolh_
#define _symbolh_

#include <glib.h>

struct symbol {
  char* identifier;
  int kind;
  int level;
  union {
    double real;
    int integer;
    char character;
  }value;
};

struct procedure_symbol {
};

struct function_symbol {
};

struct type_symbol {
};

struct var_symbol {
};

struct array_symbol {
};

struct const_symbol {
};

struct param_symbol {
};

enum types {
  boolean,
  integer,
  real,
  character,
  string,
  record,
  subrange,
  scalar
};

typedef struct symbol symbol;

extern GQueue* symbol_table;

symbol *addSymbol (char const *, symbol *);
symbol *localLookup (char const *);
symbol *globalLookup (char const *);

void showAllSymbols();

/**
Create a symbol with parameters: identifier, int type, int level, 
pointer * value
*/
symbol *createSymbol (char const *, int, int, void *);
void removeSymbol (char const *);

/**
 Our symbol table will need multiple levels, so we will
 need to add and remove levels as a new scope is defined
*/
void pushLevel ();
void popLevel ();

GHashTable *createNewTable();


void init_table ();
#endif
