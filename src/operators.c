/**
 * Author: James Osgood, Daniel Chui
 * Arithmetic and boolean operators.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symbol.h"
#include "type.h"
#include "typeerrors.h"
#include "operators.h"


/******************
 * Operators to negate and identity for a number
 * ****************/
int canEvaluate(symbol *operand) {
    if (operand == NULL) {
        return 0;
    }
    
    if (getTypeClass(operand) == TC_ERROR) {
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
  if (getTypeClass (op) == TC_REAL || getTypeClass (op) == TC_INTEGER) {
    return op;
  }
  addTypeError ("not a real or integer");
  return createErrorSym(OC_CONST);
}

/**
 * Reverse sign of number
 */
symbol *inversion (symbol *op)  {
  if (getTypeClass (op) == TC_REAL || getTypeClass (op) == TC_INTEGER) {
    if (canEvaluate (op)) {
      if (getTypeClass (op) == TC_REAL) {
	//grab int value and mult by -1
	double result = -1 * op->desc.const_attr->value.real;
	union constant_values value = {.real = result};
	return createConstant (TC_REAL, value);
      }
      else {
	int result = -1 * op->desc.const_attr->value.integer;
	union constant_values value = {.integer = result};
	return createConstant (TC_INTEGER, value);
      }
    }
    else {
      return op;
    }    
    return createAnonymousConst(op, NULL);
  }
  addTypeError ("not a real or integer");
  return createErrorSym(OC_CONST);
}

/*********
 * BOOLEAN operators
 * ****************/
symbol *notOp(symbol *operand) {
    if (operand == NULL) {
        //printf("Operand is NULL!\n");
        // error... will this ever happen?
        return createErrorSym(OC_CONST);
    }

    if (operand->symbol_type == NULL
     || getTypeClass (operand) != TC_BOOLEAN) {
        opNotBooleanError();
        return createErrorSym(OC_CONST);
    }
    
    if (canEvaluate(operand)) {
        int value = !(operand->desc.const_attr->value.boolean);
        union constant_values valueU = {.boolean = value}; 
        return createConstant(TC_BOOLEAN, valueU);
    }
    return createBooleanConst();
}

symbol *andOp (symbol *o1, symbol *o2) {
  if (o1 == NULL || o2 == NULL) {
    opNotBooleanError();
    return createErrorSym(OC_CONST);
  }
  if (getTypeClass (o1) != TC_BOOLEAN || getTypeClass (o2) != TC_BOOLEAN) {
    opNotBooleanError();
    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    //evaluate the expression
    int value1 =o1->desc.const_attr->value.boolean;
    int value2 = o2->desc.const_attr->value.boolean;
    int result = value1 && value2;
    union constant_values resultValue = {.boolean = result};
    return createConstant (TC_BOOLEAN, resultValue);
    
  }
  return createBooleanConst();
}

symbol *orOp (symbol *o1, symbol *o2){

  if (getTypeClass (o1) != TC_BOOLEAN || getTypeClass (o2) != TC_BOOLEAN) {
    opNotBooleanError();
    return createErrorSym(OC_CONST);
  }
  
  if (canEvaluate (o1) && canEvaluate (o2)) {
    int value1 =o1->desc.const_attr->value.boolean;
    int value2 = o2->desc.const_attr->value.boolean;
    int result = value1 || value2;
    union constant_values resultValue = {.boolean = result};
    return createConstant (TC_BOOLEAN, resultValue);
    
  }
  return createBooleanConst();
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

double getConstRealValue (symbol *operand) {
  return operand->desc.const_attr->value.real;
}
symbol *addOp (symbol *o1, symbol *o2) {
  if (!validArithOperator (o1) || !validArithOperator (o2)) {
    addTypeError ("operand not of type integer or real");
    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    
    //If one of them is real, then return a real, otherwise
    //returning an integer is sufficient
    int hasReal = 0;    
    int intSum = 0;
    double realSum = 0;

    if (getTypeClass (o1) == TC_INTEGER && getTypeClass (o2) == TC_INTEGER) {
      int value1 = o1->desc.const_attr->value.integer;
      int value2 = o2->desc.const_attr->value.integer;
      intSum = value1 + value2;
    }

    else {
      if (getTypeClass (o1) == TC_INTEGER) {
	int value1 = o1->desc.const_attr->value.integer;
	realSum += value1;
      }
      else {
	double value1 = o1->desc.const_attr->value.real;
	realSum += value1;
      }
      
      if (getTypeClass (o2) == TC_INTEGER) {
	int value2 = o2->desc.const_attr->value.integer;
	realSum += value2;
      }
      else {
	double value2 = o2->desc.const_attr->value.real;
	realSum += value2;
      }
      
      hasReal = 1;
    }

    //REturn based on what it is
    if (hasReal) {

      union constant_values sumValue = {.real = realSum};
      return createConstant(TC_REAL, sumValue);
    }
    else {
      union constant_values sumValue = {.integer = intSum};
      return createConstant (TC_INTEGER, sumValue);
    }
    
  }

  return createAnonymousConst(o1, o2);
}

symbol *subtractOp (symbol *o1, symbol *o2) {
  if (!validArithOperator (o1) || !validArithOperator (o2)) {
    addTypeError ("operand not of type integer or real");
    return createErrorSym(OC_CONST);
  }
    int hasReal = 0;    
    int intDiff = 0;
    double realDiff = 0;
    if (canEvaluate (o1) && canEvaluate (o2)) {
      
      //If one of them is real, then return a real, otherwise
      //returning an integer is sufficient
      int hasReal = 0;    
      int intSum = 0;
      double realSum = 0;
      
      if (getTypeClass (o1) == TC_INTEGER && getTypeClass (o2) == TC_INTEGER) {
	int value1 = o1->desc.const_attr->value.integer;
	int value2 = o2->desc.const_attr->value.integer;
	intDiff = value1 - value2;
      }
      
      else {
	if (getTypeClass (o1) == TC_INTEGER && getTypeClass (o2) == TC_REAL) {
	  int value1 = o1->desc.const_attr->value.integer;
	  double value2 = o2->desc.const_attr->value.real;
	  realDiff = value1 - value2;
	}
	else if (getTypeClass (o1) == TC_REAL && getTypeClass (o2) == TC_INTEGER) {
	  double value1 = o1->desc.const_attr->value.real;
	  int value2 = o2->desc.const_attr->value.integer;
	  realDiff = value1 - value2;
	}
	else {
	  double value1 = o1->desc.const_attr->value.real;
	  double value2 = o2->desc.const_attr->value.real;
	  realDiff = value1 - value2;
	}

	
	hasReal = 1;
      }
      
      //REturn based on what it is
      if (hasReal) {
	
	union constant_values sumValue = {.real = realDiff};
	return createConstant(TC_REAL, sumValue);
      }
      else {
	union constant_values sumValue = {.integer = intDiff};
	return createConstant (TC_INTEGER, sumValue);
      }
      
    }

    return createAnonymousConst(o1, o2);
}

symbol *multOp (symbol *o1, symbol *o2) {
  if (!validArithOperator (o1) || !validArithOperator (o2)) {
    addTypeError ("operand not of type integer or real");
    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    
    //If one of them is real, then return a real, otherwise
    //returning an integer is sufficient
    int hasReal = 0;    
    int intProd = 0;
    double realProd = 0;
    
    if (getTypeClass (o1) == TC_INTEGER && getTypeClass (o2) == TC_INTEGER) {
      int value1 = o1->desc.const_attr->value.integer;
      int value2 = o2->desc.const_attr->value.integer;
      intProd = value1 * value2;
    }
    
    else {
      if (getTypeClass (o1) == TC_INTEGER && getTypeClass (o2) ==TC_REAL) {
	int value1 = o1->desc.const_attr->value.integer;
	double value2 = o2->desc.const_attr->value.real;
	realProd = value1 * value2;
      }
      else if (getTypeClass (o1) == TC_REAL && getTypeClass (o2) == TC_INTEGER) {
	double value1 = o1->desc.const_attr->value.real;
	int value2 = o2->desc.const_attr->value.integer;
	realProd = value1 * value2;
      }
      else {
	double value1 = o1->desc.const_attr->value.real;
	double value2 = o2->desc.const_attr->value.real;
	realProd = value1 * value2;
      }
           
      hasReal = 1;
    }
    
    //REturn based on what it is
    if (hasReal) {
      
      union constant_values sumValue = {.real = realProd};
      return createConstant(TC_REAL, sumValue);
    }
    else {
      union constant_values sumValue = {.integer = intProd};
      return createConstant (TC_INTEGER, sumValue);
    }
    
  }

  return createAnonymousConst(o1, o2);
}

symbol *intDivOp (symbol *o1, symbol *o2) {
  if (getTypeClass (o1) != TC_INTEGER || getTypeClass (o2) != TC_INTEGER) {
    addTypeError ("operand not of type integer");
    return createErrorSym(OC_CONST);
  }
    
    if (canEvaluate (o1) && canEvaluate (o2)) {
      //Check if divisor is 0
      int dividend = o1->desc.const_attr->value.integer;
      int divisor = o2->desc.const_attr->value.integer;
      if (divisor == 0) {
	addTypeError ("cannot divide by 0");
	return createErrorSym(OC_CONST);
      }
      int quotient = dividend / divisor;
      union constant_values quotValue = {.integer = quotient};
      return createConstant (TC_INTEGER, quotValue);
      
    }
    
    return createAnonymousConst(o1, o2);
}

symbol *realDivOp (symbol *o1, symbol *o2) {
    type_class o1Type = getTypeClass (o1);
    type_class o2Type = getTypeClass (o2);
    printf ("%d %d\n",o1Type, o2Type);
    if ((o1Type != TC_REAL && o1Type != TC_INTEGER)|| (o2Type != TC_REAL && o2Type != TC_INTEGER) ) {
    addTypeError ("operand(s) not of type real or integer ");
    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    //Check if divisor is 0
    double dividend = o1->desc.const_attr->value.real;
    double divisor = o2->desc.const_attr->value.real;
    if (divisor == 0.0) {
      addTypeError ("cannot divide by 0");
      return createErrorSym(OC_CONST);
    }
    double quotient = dividend / divisor;
    union constant_values quotValue = {.real = quotient};
    return createConstant (TC_REAL, quotValue);
    
  }
  
  return createAnonymousConst(o1, o2);

}

symbol *modOp (symbol *o1, symbol *o2) {
  if (getTypeClass (o1) != TC_INTEGER || getTypeClass (o2) != TC_INTEGER) {
    addTypeError ("operand not of type integer");

    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    //Check if divisor is 0
    int dividend = o1->desc.const_attr->value.integer;
    int divisor = o2->desc.const_attr->value.integer;
    if (divisor == 0) {
      addTypeError ("cannot divide by 0");
      return createErrorSym(OC_CONST);
    }
    int result = dividend % divisor;
    union constant_values resultValue = {.integer = result};
    return createConstant (TC_INTEGER, resultValue);
    
  }
  
  return createAnonymousConst(o1, o2);
  
}


/************************
 * comparison operators
 * *********************/
int validComparisonOperator (symbol *operand) {
    if (getTypeClass (operand) == TC_ERROR) {
        return 1;
    }
  if (getTypeClass (operand) == TC_REAL) {
    return 1;
  }
  if (getTypeClass (operand) == TC_SCALAR) {
    return 1;
  }
  if (getTypeClass (operand) == TC_INTEGER) {
    return 1;
  }
  if (getTypeClass (operand) == TC_CHAR) {
    return 1;
  }
  if (getTypeClass (operand) == TC_BOOLEAN) {
    return 1;
  }
  if (getTypeClass (operand) == TC_ARRAY) {
    //TODO
    //showAllSymbols();
    //printf ("Have determined type class is array\n");
    //check to make sure array is of type char
    //type_class tc = operand->desc.type_attr->
    if (getArrayType (operand) == TC_CHAR) {
      return 1;
    }
  }
  return 0;
}

int checkComparisonCompatibility (symbol *o1, symbol *o2) {
  //exact same type
  
  type_class tc1 = getTypeClass (o1);
  type_class tc2 = getTypeClass (o2);
  
  if (tc1 == TC_ERROR || tc2 == TC_ERROR) {
      return 1;
  }
  
  if (tc1 == tc2) {
    if (o1->symbol_type == o2->symbol_type
     || o1->symbol_type->desc.type_attr == o2->symbol_type->desc.type_attr) {
      return 1;
    }
    return 0;
  }
  //real and integer
  if ((tc1 == TC_REAL || tc1 == TC_INTEGER) && (tc2 == TC_REAL || tc2 == TC_INTEGER)) {
    return 1;
  }
  //strings, hopefully chars should be here as well
  if (isString (o1) && isString (o2)) {
    //check string length
    char *string1 = getString (o1);
    char *string2 = getString (o2);
    if (strlen (string1) == strlen (string2)) {
      return 1;
    }
  }
  
  return 0;
}

/*NOTE: Cheating; am casting to doubles, so it is possible that these could return incorrect results
 * .....but screw it
 * Also, there is probably a better way to do this rather than just copying pasting...but blargh
 * And, right now it will still return if enums are not the exact same enum...but implement that
 * in check comparison compatibilty instead
 */
symbol *equalOp (symbol *o1, symbol *o2) {
    //printf ("Inside equal op\n");
    //printf ("op1: %p, op2: %p\n", o1, o2);
  if (!validComparisonOperator (o1) || !validComparisonOperator (o2)) {
    addTypeError ("operands cannot be compared");
    return createErrorSym(OC_CONST);
  }
  //printf ("Done checking valid\n");
  if (!checkComparisonCompatibility (o1, o2)) {
    addTypeError ("operands are not compatible, cannot be compared to each other");
    return createErrorSym(OC_CONST);
  }
  //printf ("Done checking compatiblity\n");
  
  if (canEvaluate (o1) && canEvaluate (o2)) {
    
    if (getTypeClass (o1) == getTypeClass (o2)) {
      int value1  = o1->desc.const_attr->value.integer;
      int value2 = o2->desc.const_attr->value.integer;
      int result = value1 == value2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);
      //straight comparison?
      //
      
    }
    else if (isString (o1)) {
      //call strcmp
      char *string1 = getString (o1);
      char *string2 = getString (o2);
      int cmpResult = strcmp (string1, string2);
      if (cmpResult ==0) {
	
	union constant_values resultValue = {.boolean = 1};
	return createConstant (TC_BOOLEAN, resultValue);
      }
      else {
	
	union constant_values resultValue = {.boolean = 0};
	return createConstant (TC_BOOLEAN, resultValue);
      }
      
    }
    
    else {
      double val1;
      double val2;
      if (getTypeClass (o1) == TC_REAL) {
	val1 = o1->desc.const_attr->value.real;
      }
      else {
	val1= o1->desc.const_attr->value.integer;
      }
      if (getTypeClass (o2) == TC_REAL) {
	val2 = o2->desc.const_attr->value.real;
      }
      else {
	val2 = o2->desc.const_attr->value.integer;
      }
      int result = val1 == val2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);

    }

  }
  return createBooleanConst();
}


symbol *notEqualOp (symbol *o1, symbol *o2) {
  if (!validComparisonOperator (o1) || !validComparisonOperator (o2)) {
    addTypeError ("operands cannot be compared");
    return createErrorSym(OC_CONST);
  }
  if (!checkComparisonCompatibility (o1, o2)) {
    addTypeError ("operands are not compatible, cannot be compared to each other");
    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    
    if (getTypeClass (o1) == getTypeClass (o2)) {
      int value1  = o1->desc.const_attr->value.integer;
      int value2 = o2->desc.const_attr->value.integer;
      int result = value1 != value2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);
      //straight comparison?
      
    }
    else if (isString (o1)) {
      //call strcmp
      //call strcmp
      char *string1 = getString (o1);
      char *string2 = getString (o2);
      int cmpResult = strcmp (string1, string2);
      if (cmpResult !=0) {
	
	union constant_values resultValue = {.boolean = 1};
	return createConstant (TC_BOOLEAN, resultValue);
      }
      else {
	
	union constant_values resultValue = {.boolean = 0};
	return createConstant (TC_BOOLEAN, resultValue);
      }
    }
    else {
      double val1;
      double val2;
      if (getTypeClass (o1) == TC_REAL) {
	val1 = o1->desc.const_attr->value.real;
      }
      else {
	val1= o1->desc.const_attr->value.integer;
      }
      if (getTypeClass (o2) == TC_REAL) {
	val2 = o2->desc.const_attr->value.real;
      }
      else {
	val2 = o2->desc.const_attr->value.integer;
      }
      int result = val1 != val2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);

    }
    
    
  }
  return createBooleanConst();
}

symbol *lessThanOp (symbol *o1, symbol *o2) {
  if (!validComparisonOperator (o1) || !validComparisonOperator (o2)) {
    addTypeError ("operands cannot be compared");
    return createErrorSym(OC_CONST);
  }
  if (!checkComparisonCompatibility (o1, o2)) {
    addTypeError ("operands are not compatible, cannot be compared to each other");
    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    
    if (getTypeClass (o1) == getTypeClass (o2)) {
      int value1  = o1->desc.const_attr->value.integer;
      int value2 = o2->desc.const_attr->value.integer;
      int result = value1  < value2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);
      
    }
    else if (isString (o1)) {
      //call strcmp
      char *string1 = getString (o1);
      char *string2 = getString (o2);
      int cmpResult = strcmp (string1, string2);
      if (cmpResult <0) {
	
	union constant_values resultValue = {.boolean = 1};
	return createConstant (TC_BOOLEAN, resultValue);
	//true
      }
      else {
	
	union constant_values resultValue = {.boolean = 0};
	return createConstant (TC_BOOLEAN, resultValue);
	//return false
      }
      //call strcmp
    }
    else {
      double val1;
      double val2;
      if (getTypeClass (o1) == TC_REAL) {
	val1 = o1->desc.const_attr->value.real;
      }
      else {
	val1= o1->desc.const_attr->value.integer;
      }
      if (getTypeClass (o2) == TC_REAL) {
	val2 = o2->desc.const_attr->value.real;
      }
      else {
	val2 = o2->desc.const_attr->value.integer;
      }
      int result = val1 < val2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);

    }
    
    
  }
  return createBooleanConst();
}

symbol *greaterThanOp (symbol *o1, symbol *o2) {
  if (!validComparisonOperator (o1) || !validComparisonOperator (o2)) {
    addTypeError ("operands cannot be compared");
    return createErrorSym(OC_CONST);
  }
  if (!checkComparisonCompatibility (o1, o2)) {
    addTypeError ("operands are not compatible, cannot be compared to each other");
    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    
    if (getTypeClass (o1) == getTypeClass (o2)) {
      int value1  = o1->desc.const_attr->value.integer;
      int value2 = o2->desc.const_attr->value.integer;
      int result = value1 > value2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);
      
    }
    else if (isString (o1)) {
      //call strcmp
      char *string1 = getString (o1);
      char *string2 = getString (o2);
      int cmpResult = strcmp (string1, string2);
      if (cmpResult >0) {
	
	union constant_values resultValue = {.boolean = 1};
	return createConstant (TC_BOOLEAN, resultValue);
      }
      else {
	
	union constant_values resultValue = {.boolean = 0};
	return createConstant (TC_BOOLEAN, resultValue);
      }
    }
    else {
      double val1;
      double val2;
      if (getTypeClass (o1) == TC_REAL) {
	val1 = o1->desc.const_attr->value.real;
      }
      else {
	val1= o1->desc.const_attr->value.integer;
      }
      if (getTypeClass (o2) == TC_REAL) {
	val2 = o2->desc.const_attr->value.real;
      }
      else {
	val2 = o2->desc.const_attr->value.integer;
      }
      int result = val1 > val2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);

    }
    
    
  }
  return createBooleanConst();
}

symbol *greaterThanEqualOp (symbol *o1, symbol *o2) {
  if (!validComparisonOperator (o1) || !validComparisonOperator (o2)) {
    addTypeError ("operands cannot be compared");
    return createErrorSym(OC_CONST);
  }
  if (!checkComparisonCompatibility (o1, o2)) {
    addTypeError ("operands are not compatible, cannot be compared to each other");
    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    
    if (getTypeClass (o1) == getTypeClass (o2)) {
      int value1  = o1->desc.const_attr->value.integer;
      int value2 = o2->desc.const_attr->value.integer;
      int result = value1 >= value2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);
      
    }
    else if (isString (o1)) {
      //call strcmp
      char *string1 = getString (o1);
      char *string2 = getString (o2);
      int cmpResult = strcmp (string1, string2);
      if (cmpResult >=0) {
	
	union constant_values resultValue = {.boolean = 1};
	return createConstant (TC_BOOLEAN, resultValue);
	//return false
      }
      else {
	
	union constant_values resultValue = {.boolean = 0};
	return createConstant (TC_BOOLEAN, resultValue);
	//return true
      }

    }
    else {
      double val1;
      double val2;
      if (getTypeClass (o1) == TC_REAL) {
	val1 = o1->desc.const_attr->value.real;
      }
      else {
	val1= o1->desc.const_attr->value.integer;
      }
      if (getTypeClass (o2) == TC_REAL) {
	val2 = o2->desc.const_attr->value.real;
      }
      else {
	val2 = o2->desc.const_attr->value.integer;
      }
      int result = val1 >= val2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);

    }
    
    
  }
  return createBooleanConst();
}

symbol *lessThanEqualOp (symbol *o1, symbol *o2) {
  if (!validComparisonOperator (o1) || !validComparisonOperator (o2)) {
    addTypeError ("operands cannot be compared");
    return createErrorSym(OC_CONST);
  }
  if (!checkComparisonCompatibility (o1, o2)) {
    addTypeError ("operands are not compatible, cannot be compared to each other");
    return createErrorSym(OC_CONST);
  }
  if (canEvaluate (o1) && canEvaluate (o2)) {
    
    if (getTypeClass (o1) == getTypeClass (o2)) {
      int value1  = o1->desc.const_attr->value.integer;
      int value2 = o2->desc.const_attr->value.integer;
      int result = value1 <= value2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);
      
    }
    else if (isString (o1)) {
      //call strcmp
      char *string1 = getString (o1);
      char *string2 = getString (o2);
      int cmpResult = strcmp (string1, string2);
      if (cmpResult <=0) {
	//return true
	union constant_values resultValue = {.boolean = 1};
	return createConstant (TC_BOOLEAN, resultValue);
      }
      else {
	//return false
	union constant_values resultValue = {.boolean = 0};
	return createConstant (TC_BOOLEAN, resultValue);
      }
    }
    else {
      double val1;
      double val2;
      if (getTypeClass (o1) == TC_REAL) {
	val1 = o1->desc.const_attr->value.real;
      }
      else {
	val1= o1->desc.const_attr->value.integer;
      }
      if (getTypeClass (o2) == TC_REAL) {
	val2 = o2->desc.const_attr->value.real;
      }
      else {
	val2 = o2->desc.const_attr->value.integer;
      }
      int result = val1 <= val2;
      union constant_values resultValue = {.boolean = result};
      return createConstant (TC_BOOLEAN, resultValue);

    }

  }
  return createBooleanConst();
}
