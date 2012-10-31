#ifndef _symbolh_
#define _symbolh_

struct symbol {
  char* identifier;
  int type;
  int level;
  union {
    double real;
    int integer;
    char character;
  }value;
};

enum types {
  boolean,
  integer,
  real,
  char,
  string,
  procedure,
  function
  
};

typedef struct symbol symbol;

extern stack *symbol_table_stack;

symbol *addSymbol(char const *, symbol *);
symbol *getSymbol(char const *);
void showAllSymbols();

/**
Create a symbol with parameters: identifier, int type, int level, 
pointer * value
*/
symbol *createSymbol (char const *, int, int, void *);

/**
 Our symbol table will need multiple levels, so we will
 need to 
*/
addNewLevel ();
removeTopLevel ();

#endif
