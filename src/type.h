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

/**
 * Returns 1 if and only if array1 and array2 have both identical indexing and
 * mutually-assignment-compatible object types.
 */
int arrayAssignmentCompatible(symbol *sym1, symbol *sym2);

/**
 * Get the array description for a symbol
 */
struct tc_array *getArrayDescription (symbol *sym);

/**
 * Returns a pointer to a constant symbol of type TC_INTEGER, TC_REAL, TC_CHAR,
 * TC_BOOLEAN, or TC_STRING with value.
 */
symbol *createConstant(type_class type, union constant_values value);

/**
 * Returns a pointer to an anonymous array symbol.
 */
symbol *stringToArray(const char *string);

/**
 * Returns a pointer to a type symbol of type TC_ERROR.
 */
symbol *createErrorType();

/**
 * Returns a pointer to a symbol with name, object class OC_TYPE, and type
 * attribute type.
 */
symbol *createTypeSym(const char *name, struct type_desc *type);

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
symbol *addNewSymbol(const char *id, symbol *type, object_class objClass);

/**
 * Functions that add and return new symbols that are declared in the grammar.
 */
symbol *addNewType(const char *id, symbol *type);
symbol *addNewVar(const char *id, symbol *type);
symbol *addNewConst(const char *id, symbol *result);
symbol *addNewParam(const char *id, const char *typeId);
symbol *addNewProc(const char *id, GPtrArray *paramList);
symbol *addNewFunc(const char *id, const char *typeId, GPtrArray *paramList);

/**
 * Adds a new symbol with object class obj_class, of anonymous type, and with
 * identifier id to the symbol table; returns the anonymous type. Adds an error
 * if a symbol with id already exists at the current level.
 */
struct type_desc *addNewSymbolAnonType(const char *id, struct type_desc *type,
                                       object_class objClass);

/**
 * Returns the type symbol with name id. Adds an error if no such type symbol
 * can be found.
 */
symbol *getType(const char *id);

/**
 * Returns a pointer to the scalar list type symbol created by nameList.
 * 
 */
symbol *createScalarList(GPtrArray *nameList);

/**
 * Adds scalar as a symbol to the current level and to scalarList and returns
 * the resulting scalarList. Adds an error if there exists a symbol in the
 * current level with the same name as scalar.
 */
GPtrArray *addScalar(GPtrArray *scalarList, const char *scalar);

/**
 * Returns a pointer to the array type symbol created by indexType and objType.
 */
symbol *createArray(symbol *indexType, symbol *objType);

/**
 * Returns a pointer to the array index type symbol created by low and high.
 * Adds an error if low and high do not have a compatible array index type.
 */
symbol *createArrayIndex(symbol *low, symbol *high);

/**
 * Returns a pointer to the record type symbol created by fieldList.
 */
symbol *createRecord(GPtrArray *fieldList);

/**
 * Adds newField to fieldList and returns the resulting fieldList. Adds an
 * error if there exists a symbol in fieldList with the same name as newField.
 */
GPtrArray *addField(GPtrArray *fieldList, symbol *newField);

/**
 * Adds newParam to paramList and returns the resulting paramList.
 */
GPtrArray *addParam(GPtrArray *paramList, symbol *newParam) ;

/**
 * Assign an expression to a var
 */
void doVarAssignment (symbol *var, symbol *expr);

symbol *createAnonymousVar(symbol *o1, symbol *o2);

int isString (symbol *sym);
char *getString (symbol *sym);
type_class getArrayType (symbol *sym);

#endif
