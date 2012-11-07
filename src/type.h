/**
 * Author: James Osgood
 * Type creation, assignment, and checking for the grammar.
 */

#ifndef TYPE_H
#define TYPE_H

#include <glib.h>
#include "symbol.h"

/**
 * Adds a new symbol with object class obj_class, of type, and with identifier
 * id to the symbol table; returns type. Adds an error if a symbol with id
 * already exists at the current level.
 */
symbol *addNewSymbol(const char *id, symbol *type, int objClass);

/**
 * Adds a new symbol with object class obj_class, of anonymous type, and with
 * identifier id to the symbol table; returns the anonymous type. Adds an error
 * if a symbol with id already exists at the current level.
 */
struct type_desc *addNewSymbolAnonType(const char *id, struct type_desc *type,
                                       int objClass);

/**
 * Returns the type_desc of the symbol with name id. Adds an error if no such
 * symbol can be found.
 */
struct type_desc *getType(const char *id);

/**
 * Returns a pointer to the scalar list type_desc created by nameList.
 */
struct type_desc *createScalarList(GArray *nameList);

/**
 * Adds scalar as a symbol to the current level and to scalarList and returns
 * the resulting scalarList. Adds an error if there exists a symbol in the
 * current level with the same name as scalar.
 */
GArray *addScalar(GArray *scalarList, const char *scalar);

/**
 * Returns a pointer to the array type_desc created by indexType and objType.
 */
struct type_desc *createArray(struct type_desc *indexType,
                              struct type_desc *objType);

/**
 * Returns a pointer to the record type_desc created by fieldList.
 */
struct type_desc *createRecord(GArray *fieldList);

/**
 * Adds newField to fieldList and returns the resulting fieldList. Adds an
 * error if there exists a symbol in fieldList with the same name as newField.
 */
GArray *addField(GArray *fieldList, symbol *newField);

#endif
