/**
 * Author: James Osgood
 * Add various semantic type errors.
 */

#ifndef TYPE_H
#define TYPE_H

/**
 * Add errMsg as a type error to the current column and line.
 */
void addTypeError(const char *errMsg);

/**
 * Adds a "type not defined for symbol 'id'; subsequent type errors with this
 * symbol will be ignored" error.
 */
void typeNotDefinedError(const char *id);

/**
 * Adds a "symbol 'id' already defined in this scope" error.
 */
void symExistsError(const char *id);

/**
 * Adds a "no 'id' symbol defined" error.
 */
void symNotDefinedError(const char *id);

/**
 * Adds a "symbol 'id' not a type" error.
 */
void symNotATypeError(const char *id);

/**
 * Adds an "illegal array assignment (object types not compatible)" error.
 */
void illArrayAssignObjError();

/**
 * Adds a "duplicate field 'id'" error.
 */
void duplicateFieldError(const char *id);

#endif
