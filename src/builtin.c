/**
 * author: Daniel Chui
 * file that will initialize compiler symbol table with built ins
 */


#include "builtin.h"

#include "symbol.h"
#include "builtinfunctions.h"


void add_builtins () {
  pushLevel();
  add_builtin_types();
  add_builtin_functions();
}
int odd (int x) {
  return 1;
}
 void add_builtin_functions() {
   symbol *functionSymbol;
   
   
   functionSymbol = createSymbolFunction ("odd", &odd);
   
 }
void add_builtin_types () {
/**
    addSymbol ("bool");
    addSymbol ("integer")
    addSymbol ("real")
    addSymbol ("char");
    addSymbol ("false");
    addSymbol ("true");
    addSymbol ("maxint");
   */ 
   
   
  //There is probably a better way to do this programmatically...but screw it
  //push on initial level

  symbol *typeSymbol;
  typeSymbol = createSymbolType ("char", TC_CHAR);
  addSymbol ("char", typeSymbol);
  typeSymbol = createSymbolType ("boolean", TC_BOOLEAN);
  addSymbol ("boolean", typeSymbol);
  typeSymbol = createSymbolType ("integer", TC_INTEGER);
  addSymbol ("integer", typeSymbol);
  typeSymbol = createSymbolType ("real", TC_REAL);
  addSymbol ("real", typeSymbol);
  
  


}

struct init_types
{
  char *type_name;
  void *type;
};

struct init_types types []
  = {
      "boolean",
      "integer",
      "real",
      "char",
      "string",
      "enum"
  };
  
char * typesBuiltinNames [] = {"boolean", "integer", "real", "char"};