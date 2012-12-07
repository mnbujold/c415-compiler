/**
 * Author: James Osgood
 * Add various semantic type errors.
 */

#include <stdio.h>
#include <string.h>
#include "myerror.h"

extern myerror *eList;
extern int iserror;
extern int lineno;
extern int last_column;

void
addTypeError(const char *errMsg) {
    iserror = 1;
    eList = addError(eList, errMsg, last_column, lineno, ET_ERROR);
}

void
addTypeWarning(const char *warnMsg) {
    eList = addError(eList, warnMsg, last_column, lineno, ET_WARNING);
}

void
typeNotDefinedError(const char *id) {
    char errMsg[90 + strlen(id)];
    sprintf(errMsg,
            "type not defined for symbol '%s'; each undefined type will be reported once per scope",
            id);
    addTypeError(errMsg);
}

void
symExistsError(const char *id) {
    char errMsg[50 + strlen(id)];
    sprintf(errMsg, "symbol '%s' already defined in this scope", id);
    addTypeError(errMsg);
}

void
symNotDefinedError(const char *id) {
    char errMsg[80 + strlen(id)];
    sprintf(errMsg,
            "no '%s' symbol defined; each undefined symbol will be reported once per scope",
            id);
    addTypeError(errMsg);
}

void
symNotATypeError(const char *id) {
    char errMsg[25 + strlen(id)];
    sprintf(errMsg, "symbol '%s' not a type", id);
    addTypeError(errMsg);
}

void
symNotAVarParmConstError(const char *id) {
    if (id == NULL) {
        addTypeError("index is not a variable, parameter, or constant");
    } else {
        char errMsg[50 + strlen(id)];
        sprintf(errMsg, "symbol '%s' not a variable, parameter, or constant", id);
        addTypeError(errMsg);
    }
}

void
symNotValidEnumError(const char *id) {
    char errMsg[50 + strlen(id)];
    sprintf(errMsg, "symbol '%s' not a valid enumeration type", id);
    addTypeError(errMsg);
}

void
illArrayAssignObjError(){
    addTypeError("illegal array assignment (object types not compatible)");
}

void
illArrayAssignIndError() {
    addTypeError("illegal array assignment (index types not the same)");
}

void
illArrayAssignMinError(int min1, int min2) {
    char errMsg[100];
    sprintf(errMsg,
            "illegal array assignment (lower bound %d does not match lower bound %d)",
            min1,
            min2);
    addTypeError(errMsg);
}

void
illArrayAssignMaxError(int max1, int max2) {
    char errMsg[100];
    sprintf(errMsg,
            "illegal array assignment (upper bound %d does not match upper bound %d)",
            max1,
            max2);
    addTypeError(errMsg);
}

void arrayMissLowerError() {
    addTypeError("array index missing lower bound");
}

void arrayLowerNotConstError() {
    addTypeError("array index lower bound not of constant type");
}

void arrayBoundTypeError() {
    addTypeError("array index lower bound not the same type as upper bound");
}

void arrayBoundInvalidError() {
    addTypeError("array index bounds are not valid array index types");
}

void lowerGreaterThanUpperError() {
    addTypeError("lower bound is greater than upper bound");
}

void arrayIndexTypeError() {
    addTypeError("array index type not a valid array index type");
}

void arrayOutOfBoundsError () {
    addTypeError ("index is not within bounds of array");
}

void assignmentError () {
  addTypeError ("could not assign to var");
}

void assignmentCompatibilityError () {
  addTypeError ("could not assign (types are incompatible)");
}

void
duplicateFieldError(const char *id) {
    char errMsg[25 + strlen(id)];
    sprintf(errMsg, "duplicate field '%s'", id);
    addTypeError(errMsg);
}

void
fieldNotInRecordError(const char *record, const char *field) {
    char errMsg[30 + strlen(field) + strlen(record)];
    sprintf(errMsg, "record '%s' does not have field '%s'", record, field);
    addTypeError(errMsg);
}

void
opNotBooleanError() {
    addTypeError("operand not of type boolean");
}

void
missFuncRetError() {
    addTypeError("missing unconditional function return value assignment");
}

void
assignNotVarParamError() {
    addTypeError("target of assignment not a variable or parameter");
}

void
assignToError(const char *objectClass) {
    char errMsg[25 + strlen(objectClass)];
    sprintf(errMsg, "cannot assign to a %s", objectClass);
    addTypeError(errMsg);
}


void
assignBadTypesError() {
    addTypeError("target and source of assignment do not have matching types");
}

void
notCallableError(char *proc_name, const char *callable) {
    char errMsg[50 + strlen(proc_name) + strlen(callable)];
    sprintf(errMsg, "cannot call '%s' here since it is not a %s", proc_name, callable);
    addTypeError(errMsg);
}

void
notEnoughParamsError(const char *callable) {
    char errMsg[50 + strlen(callable)];
    sprintf(errMsg, "not enough arguments in %s call", callable);
    addTypeError(errMsg);
}

void
tooManyParamsError(const char *callable) {
    char errMsg[50 + strlen(callable)];
    sprintf(errMsg, "too many arguments in %s call", callable);
    addTypeError(errMsg);
}

void
missingVarParamError(int arg_num, char *proc_name) {
    char errMsg[80 + strlen(proc_name)];
    sprintf(errMsg,
            "var argument %d of '%s' call is not a variable",
            arg_num,
            proc_name);
    addTypeError(errMsg);
}


void
badProcArgError(int arg_num, char *proc_name) {
    char errMsg[80 + strlen(proc_name)];
    sprintf(errMsg,
            "argument %d of '%s' call has incompatible type",
            arg_num,
            proc_name);
    addTypeError(errMsg);
}

void
symNotRecordError(const char *id) {
    if (id == NULL) {
        addTypeError("symbol not a record");
    } else {
        char errMsg[25 + strlen(id)];
        sprintf(errMsg, "symbol '%s' not a record", id);
        addTypeError(errMsg);
    }
}

void
symNotArrayError(const char *id) {
    if (id == NULL) {
        addTypeError("symbol being accessed is not an array");
    } else {
        char errMsg[25 + strlen(id)];
        sprintf(errMsg, "symbol '%s' not an array", id);
        addTypeError(errMsg);
    }
}

void
illArrayAccessIndError() {
    addTypeError("array index type not compatible with type being used to access array");
}

void
illArrayAccessMinError(int index, int min) {
    char errMsg[75];
    sprintf(errMsg, "illegal array access (index %d below lower bound %d)", index, min);
    addTypeError(errMsg);
}

void
illArrayAccessMaxError(int index, int max) {
    char errMsg[75];
    sprintf(errMsg, "illegal array access (index %d above upper bound %d)", index, max);
    addTypeError(errMsg);
}

void
ctrlTypeNotInLoopError(const char *controlType) {
    char errMsg[30 + strlen(controlType)];
    sprintf(errMsg, "'%s' statement not in a loop", controlType);
    addTypeError(errMsg);
}
