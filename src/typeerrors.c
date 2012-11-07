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

void
duplicateFieldError(const char *id) {
    char errMsg[25 + strlen(id)];
    sprintf(errMsg, "duplicate field '%s'", id);
    addTypeError(errMsg);
}
