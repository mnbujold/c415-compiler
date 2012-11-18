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
    eList = addError(eList, errMsg, last_column, lineno);
}

void
typeNotDefinedError(const char *id) {
    char errMsg[90 + strlen(id)];
    sprintf(errMsg,
            "type not defined for symbol '%s'; subsequent type errors with this symbol will be ignored",
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
    char errMsg[25 + strlen(id)];
    sprintf(errMsg, "no '%s' symbol defined", id);
    addTypeError(errMsg);
}

void
symNotATypeError(const char *id) {
    char errMsg[25 + strlen(id)];
    sprintf(errMsg, "symbol '%s' not a type", id);
    addTypeError(errMsg);
}

void symNotAVarParmConstError(const char *id) {
    char errMsg[50 + strlen(id)];
    sprintf(errMsg, "symbol '%s' not a variable, parameter, or constant", id);
    addTypeError(errMsg);
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
  addTypeError ("could not assign; types are incompatible");
}

void
duplicateFieldError(const char *id) {
    char errMsg[25 + strlen(id)];
    sprintf(errMsg, "duplicate field '%s'", id);
    addTypeError(errMsg);
}

void
opNotBooleanError() {
    addTypeError("operator not of type boolean");
}

void
missFuncRetError() {
    addTypeError("missing function return value assignment");
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
    char errMsg[75 + strlen(proc_name)];
    sprintf(errMsg,
            "var argument %d of '%s' call is not a variable",
            arg_num,
            proc_name);
    addTypeError(errMsg);
}


void
badProcArgError(int arg_num, char *proc_name) {
    char errMsg[75 + strlen(proc_name)];
    sprintf(errMsg,
            "argument %d of' %s' call has incompatible type",
            arg_num,
            proc_name);
    addTypeError(errMsg);
}

void
symNotRecordError(const char *id) {
    char errMsg[50 + strlen(id)];
    sprintf(errMsg, "symbol '%s' not a record", id);
    addTypeError(errMsg);
}
