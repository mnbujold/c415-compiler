/**
 * author: Daniel Chui
 * file that will initialize compiler symbol table with built ins
 */


#include <stdlib.h>

#include "builtin.h"

#include "symbol.h"
#include "type.h"
#include "builtinfunctions.h"


void add_builtins () {
  pushLevel();
   add_builtin_types();
   add_builtin_functions();
  /**
   * What to do with these? Consts
   * true
   * false
   * maxint
   * minint
   * pi
   */
  printf ("Done adding builtins\n");
   showAllSymbols();
//   exit;
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
   
   //Get all the type descriptions here, assign them as needed
   symbol *intTypeDescription = getType ("integer");
   symbol *booleanTypeDescription = getType ("boolean");
   symbol *realTypeDescription = getType ("real");
   GPtrArray *oddParam = g_ptr_array_new ();

   struct param_desc *parameter = calloc (1, sizeof (struct param_desc));
   parameter->type = intTypeDescription;
   g_ptr_array_add (oddParam, parameter);
   functionDescription->params = oddParam;
   functionDescription->return_type = booleanTypeDescription;
   functionSymbol = createSymbolFunction ("odd", functionDescription);
   addSymbol ("odd", functionSymbol);
   
   //TODO: They are all the same as odd right now, bad, change them
   /*Math functions */
   
   //TODO: How do we let functions accept both reals and ints?
   //For now, I am just giving them reals, since reals can be ints, but not vice versa
   //For large ints, this will probably not work
   GPtrArray *absParam = g_ptr_array_new ();
   parameter->type = realTypeDescription;
   g_ptr_array_add (absParam, parameter);
   functionDescription->params = absParam;
   functionDescription->return_type = realTypeDescription;
   functionSymbol = createSymbolFunction ("abs", functionDescription);
   addSymbol ("abs", functionSymbol);
   
   
   
   GPtrArray *sqrParam = g_ptr_array_new();
   parameter->type = realTypeDescription;
   g_ptr_array_add(sqrParam, parameter);
   functionDescription->params = sqrParam;
   functionDescription->return_type = realTypeDescription;
   functionSymbol = createSymbolFunction ("sqr", functionDescription);
   addSymbol ("sqr", functionSymbol);
   
   GPtrArray *sqrtParam = g_ptr_array_new();
   parameter->type = realTypeDescription;
   g_ptr_array_add (sqrtParam, parameter);
   functionDescription->params = sqrtParam;
   functionDescription->return_type = realTypeDescription;
   functionSymbol = createSymbolFunction ("sqrt", functionDescription);
   addSymbol ("sqrt", functionSymbol);
   
   GPtrArray *sinParam = g_ptr_array_new();
   parameter->type = realTypeDescription;
   g_ptr_array_add (sinParam, parameter);
   functionDescription->params = sinParam;
   functionDescription->return_type = realTypeDescription;
   functionSymbol = createSymbolFunction ("sin", functionDescription);
   addSymbol ("sin", functionSymbol);

   GPtrArray *expParam = g_ptr_array_new ();
   parameter->type = realTypeDescription;
   g_ptr_array_add (expParam, parameter);
   g_ptr_array_add (expParam, parameter); //2 arguments, dunno if this works
   functionDescription->params = expParam;
   functionDescription->return_type = realTypeDescription;
   functionSymbol = createSymbolFunction ("exp", functionDescription);
   addSymbol ("exp", functionSymbol);
   
   
   
   GPtrArray *lnParam = g_ptr_array_new ();
   parameter->type = realTypeDescription;
   g_ptr_array_add (lnParam, parameter);
   functionDescription->params = oddParam;
   functionDescription->return_type = realTypeDescription;
   functionSymbol = createSymbolFunction ("ln", functionDescription);
   addSymbol ("ln", functionSymbol);
   
   /* Numbery functions */
   
   
   GPtrArray *truncParam = g_ptr_array_new ();
   parameter->type = realTypeDescription;
   g_ptr_array_add (truncParam, parameter);
   functionDescription->params = truncParam;
   functionDescription->return_type = realTypeDescription;
   functionSymbol = createSymbolFunction ("trunc", functionDescription);
   addSymbol ("trunc", functionSymbol);
   
   GPtrArray *roundParam = g_ptr_array_new();
   parameter->type = realTypeDescription;
   g_ptr_array_add(roundParam, parameter);
   functionDescription->return_type = realTypeDescription;
   functionSymbol = createSymbolFunction ("round", functionDescription);
   addSymbol ("round", functionSymbol);
   /* enumy functions */
   
   
   GPtrArray *ordParam = g_ptr_array_new();
   //parameter->type = 
   functionSymbol = createSymbolFunction ("ord", functionDescription);
   functionSymbol = createSymbolFunction ("chr", functionDescription);
   functionSymbol = createSymbolFunction ("succ", functionDescription);
   functionSymbol = createSymbolFunction ("pred", functionDescription);
   
   
   
   //createSymbolFunction ("high", functionDescription);
   
   
   /*Write/Read functions */
   
   functionSymbol = createSymbolFunction ("read", functionDescription);
   functionSymbol = createSymbolFunction ("readln", functionDescription);
   
   
   
   struct procedure_desc *procedureDescription = calloc (1, sizeof (struct procedure_desc));
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