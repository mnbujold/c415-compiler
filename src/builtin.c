/**
 * author: Daniel Chui
 * file that will initialize compiler symbol table with built ins
 */


#include "builtin.h"
#include "symbol.h"
 
void add_builtins () {
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
  pushLevel();
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