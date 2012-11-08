/**
 * Author: James Osgood
 * Type creation, assignment, and checking for the grammar.
 */

#ifndef TYPE_H
#define TYPE_H

#include <glib.h>
#include "symbol.h"

/**
 * Given two symbols (or type_descs) of type T1 and T2, respectively, returns 1
 * if any of the following are true:
 *  1. T1 and T2 are the exact same type.
 *  2. T1 and T2 are string types with the same number of components.
 *  3. T1 and T2 are integers and/or reals.
 */
int compatibleSym(symbol *sym1, symbol *sym2);
int compatible(struct type_desc *type1, struct type_desc *type2);

/**
 * Given two symbols (or type_descs) of type T1 and T2, respectively, returns 1
 * if any of the following are true:
 *  1. T1 and T2 are the same type.
 *  2. T1 is of type real and T2 is of type integer.
 *  3. T1 and T2 are compatible scalar types and the value of type T2 is in the
 *     interval specified by type T1.
 *  4. T1 and T2 are assignment compatible arrays.
 */
int assignmentCompatibleSym(symbol *sym1, symbol *sym2);
int assignmentCompatible(struct type_desc *type1, struct type_desc *type2);

/**
 * Returns 1 if and only if array1 and array2 have both identical indexing and
 * mutually-assignment-compatible object types.
 */
int arrayAssignmentCompatible(struct tc_array *array1, struct tc_array *array2);

/**
 * Returns a pointer to a type_desc of type TC_INTEGER, TC_REAL, or TC_CHAR.
 */
struct type_desc *createBaseType(type_class type);

/**
 * Returns a pointer to a type_desc of type TC_STRING.
 */
struct type_desc *createStringType(type_class type, const char *string);

/**
 * Adds new symbols for the program and input and output file parameters.
 */
void addProgramSymbols(const char *program, const char *input,
                       const char *output);

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
 * Returns a pointer to the array index type type_desc created by lowType and
 * highType. Adds an error if lowType and highType are not a compatible array
 * index type.
 */
struct type_desc *createArrayIndex(struct type_desc *lowType,
                                   struct type_desc *highType);

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
