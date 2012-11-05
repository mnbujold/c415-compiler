/**
 * Author: James Osgood
 * Type creation, assignment, and checking for the grammar.
 */

#ifndef TYPE_H
#define TYPE_H

#include "myerror.h"
#include "symbol.h"

/**
 * Adds a new symbol with object class obj_class, of type, and with identifier id
 * to the symbol table. Adds an error if a symbol with id already exists at the
 * current level.
 */
struct symbol *addNewSymbol(const char *id, struct symbol *type, int obj_class);

#endif
