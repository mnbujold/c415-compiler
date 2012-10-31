/**
 * made by Paul Vandermeer
 * modified by Daniel Chui
 * implementation of symbol table for compiler
 * modified heavily from Paul's logic calculator symbol table
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "symbol.h"

symbol *addSymbol(char const *identifier, symbol *symbol) {
  /**
  PSEUDOCODE because library not approved yet
  Symbol table = stack.getTop()
  hashmap_add (identifier, symbol);
  
  
  
  */
}
symbol *getSymbol(char const *identifier) {

/**
stack.size
for i = stack.size i > 0
pointer  = stack (i)get (identifier)
if (pointer != NULL)
  return
  

*/
  
}



addNewLevel () {
  /**
  pseudo code
  stack.push (new symbol table);
  */
}

removeTopLevel () {
/**
stack.pop 
*/
}

/**
 For debugging purposes, dump the contents of the symbol table
 */
void showAllSymbols(symbol *in) {

}
