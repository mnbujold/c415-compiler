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
 * Adds a "symbol 'id' not a variable, parameter, or constant" error.
 */
void symNotAVarParmConstError(const char *id);

/**
 * Adds a "symbol 'id' not a valid enumeration type" error.
 */
void symNotValidEnumError(const char *id);

/**
 * Adds an "illegal array assignment (object types not compatible)" error.
 */
void illArrayAssignObjError();

/**
 * Adds an "array index missing lower bound" error.
 */
void arrayMissLowerError();

/**
 * Adds a "array index lower bound not of constant type" error.
 */
void arrayLowerNotConstError();

/**
 * Adds an "array index lower bound not the same type as upper bound" error.
 */
void arrayBoundTypeError();

/**
 * Adds an "array index bounds are not valid array index types" error.
 */
void arrayBoundInvalidError();

/**
 * Adds a "lower bound is greater than upper bound" error.
 */
void lowerGreaterThanUpperError();

/**
 * Adds a "array index type not a valid array index type" error.
 */
void arrayIndexTypeError();

/**
 * Adds a "duplicate field 'id'" error.
 */
void duplicateFieldError(const char *id);

/**
 * Adds a "operator not of type boolean" error.
 */
void opNotBooleanError();

/**
 * Adds a "missing function return value assignment" error.
 */
void missFuncRetError();

/**
 * Adds a "target of assignment not a variable or parameter" error.
 */
void assignNotVarParamError();

/**
 * Adds a "cannot assign to a objectClass" error.
 */
void assignToError(const char *objectClass);

/**
 * Adds a "target and source of assignment do not have matching types" error.
 */
void assignBadTypesError();

/**
 * Adds a "cannot call 'procname' here since it is not a callable" error.
 */
void notCallableError(char *proc_name, const char *callable);

/**
 * Adds a "not enough arguments in callable call" error.
 */
void notEnoughParamsError(const char *callable);

/**
 * Adds a "too many arguments in callable call" error.
 */
void tooManyParamsError(const char *callable);

/**
 * Adds a "var argument arg_num of 'proc_name' call is not a variable" error.
 */
void missingVarParamError(int arg_num, char *proc_name);

/**
 * Adds an "argument arg_num of 'proc_name' call has incompatible type" error.
 */
void badProcArgError(int arg_num, char *proc_name);

/**
 * Adds a "symbol 'id' not a record" error.
 */
void symNotRecordError(const char *id);

/**
 * Adds a "symbol 'id' not an array" error.
 */
void symNotArrayError(const char *id);

/**
 * Adds a "'controlType' statement not in a loop" error.
 */
void ctrlTypeNotInLoopError(const char *controlType);

#endif
