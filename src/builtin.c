/**
 * made by Daniel Chui
 * file that will initialize compiler symbol table with built ins
 */
 
void addbuiltins () {

    addSymbol ("bool");
    addSymbol ("integer")
    addSymbol ("real")
    addSymbol ("char")
    addSymbol ("false")
    addSymbol ("true")
    addSymbol ("maxint")
    
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
  }