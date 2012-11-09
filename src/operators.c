/**
 * Author: James Osgood, Daniel Chui
 * Arithmetic and boolean operators.
 */

#include <stdlib.h>
#include <stdio.h>
#include "symbol.h"
#include "type.h"
#include "typeerrors.h"


/******************
 * Operators to negate and identity for a number
 * ****************/
int canEvaluate(symbol *operand) {
    if (operand == NULL) {
        return 0;
    }
    int oc = operand->oc;
    if (oc == OC_CONST) {
      return operand->desc.const_attr->hasValue;
    }
    if (oc == OC_VAR) {
      //check if it points to a constant eventually
    }
    //evaluate functions taking constants
    return 0;
    
}

symbol *identity (symbol *op) {
}

symbol *inversion (symbol *op)  {
}

/*********
 * BOOLEAN operators
 * ****************/
symbol *notOp(symbol *operand) {
    if (operand == NULL) {
        printf("Operand is NULL!\n");
        // error... will this ever happen?
        return createErrorType();
    }
    
    if (operand->symbol_type == NULL
     || getTypeClass (operand) != TC_BOOLEAN) {
        opNotBooleanError();
        return createErrorType();
    }
    
    if (canEvaluate(operand)) {
        int value = !(operand->desc.const_attr->value.boolean);
        return createConstant(TC_BOOLEAN, value, 0.0, 0);
    }
    symbol *resultSym = createConstant(TC_BOOLEAN, 0, 0.0, 0);
    resultSym->desc.const_attr->hasValue = 0;
    
    return resultSym;
}

symbol *andOp (symbol *o1, symbol *o2) {
  if (o1 == NULL || o2 == NULL) {
    return createErrorType();
  }
  if (getTypeClass (o1) != TC_BOOLEAN || getTypeClass (o2) != TC_BOOLEAN) {
    return createErrorType();
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    //evaluate the expression
  }
  
}

symbol *orOp (symbol *o1, symbol *o2){

  if (getTypeClass (o1) != TC_BOOLEAN || getTypeClass (o2) != TC_BOOLEAN) {
    return createErrorType ();
  }
  
  if (canEvaluate (o1) && canEvaluate (o2)) {
  }
}

/*******************
 * Arithmetic operators
 * ******************/

int validArithOperator (symbol *operand) {
  if (getTypeClass (operand) == TC_INTEGER || getTypeClass (operand) == TC_REAL) {
    return 1;
  }
  return 0;
}
symbol *addOp (symbol *o1, symbol *o2) {
  if (!validArithOperator (o1) || !validArithOperator (o2)) {
  }
}

symbol *subtractOp (symbol *o1, symbol *o2) {
  if (!validArithOperator (o1) || !validArithOperator (o2)) {
  }
}

symbol *multOp (symbol *o1, symbol *o2) {
  if (!validArithOperator (o1) || !validArithOperator (o2)) {
  }
}

symbol *intDivOp (symbol *o1, symbol *divisor) {
  if (getTypeClass (o1) != TC_INTEGER || getTypeClass (divisor) != TC_INTEGER) {
    return createErrorType();
  }
  //if divisor == 0 return error
}

symbol *realDivOp (symbol *o1, symbol *divisor) {
  if (getTypeClass (o1) != TC_REAL || getTypeClass (divisor) != TC_REAL) {
    return createErrorType();
  }
  //if divisor == 0 error
}

symbol *modOp (symbol *o1, symbol *o2) {
  if (getTypeClass (o1) != TC_INTEGER || getTypeClass (o2) != TC_INTEGER) {
    return createErrorType();
  }
  //if divisor == 0 error
  
}


/************************
 * comparison operators
 * *********************/
symbol *equalOp (symbol *o1, symbol *o2) {
}
symbol *notEqualOp (symbol *o1, symbol *o2) {
}
symbol *lessThanOp (symbol *o1, symbol *o2) {
}
symbol *greaterThanOp (symbol *o1, symbol *o2) {
}
symbol *greaterThanEqualOp (symbol *o1, symbol *o2) {
}
symbol *lessThanEqualOp (symbol *o1, symbol *o2) {
}