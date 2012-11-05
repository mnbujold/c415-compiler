/**
 * Author: James Osgood
 * Type creation, assignment, and checking for the grammar.
 */

#ifndef TYPE_H
#define TYPE_H

#include "myerror.h"
#include "symbol.h"

struct symbol *addNewSymbol(const char *id, struct symbol *type, int kind);

#endif
