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
  /**
   * What to do with these?
   * true
   * false
   * maxint
   * minint
   */
}
int odd (int x) {
  return 1;
}
 void add_builtin_functions() {
   symbol *functionSymbol;
   
   /*Here is the stuff for odd function
    */
   struct function_desc *functionDescription = calloc (1, sizeof (struct function_desc));
   //functionDescription
   //Do stuff to set function description to 
   GPtrArray *oddParam = g_ptr_array_new ();
   struct type_desc *intTypeDescription = getType ("integer");
   g_ptr_array_add (oddParam, intTypeDescription);
   struct type_desc *booleanTypeDescription = getType ("boolean");
   functionDescription->return_type = booleanTypeDescription;
   functionSymbol = createSymbolFunction ("odd", functionDescription);
   addSymbol ("odd", functionSymbol);
   
   //TODO: They are all the same as odd right now, bad, change them
   /*Math functions */
   createSymbolFunction ("abs", functionDescription);
   createSymbolFunction ("sqr", functionDescription);
   createSymbolFunction ("sqrt", functionDescription);
   createSymbolFunction ("sin", functionDescription);
   createSymbolFunction ("exp", functionDescription);
   createSymbolFunction ("ln", functionDescription);
   
   /* Numbery functions */
   createSymbolFunction ("trunc", functionDescription);
   createSymbolFunction ("round", functionDescription);
   
   createSymbolFunction ("ord", functionDescription);
   createSymbolFunction ("chr", functionDescription);
   createSymbolFunction ("succ", functionDescription);
   createSymbolFunction ("pred", functionDescription);
   
   
   
   //createSymbolFunction ("high", functionDescription);
   
   
   /*Write/Read functions */
   createSymbolFunction ("read", functionDescription);
   createSymbolFunction ("readln", functionDescription);
   createSymbolFunction ("write", functionDescription);
   createSymbolFunction ("writeln", functionDescription);
   
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