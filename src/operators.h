/**
 * Author: James Osgood, Daniel Chui
 * Arithmetic and boolean operators.
 */

#ifndef OPERATORS_H
#define OPERATORS_H

#include "symbol.h"

/**
 * Returns 1 if the operand symbol is a constant with a known value. Returns 0
 * otherwise.
 */
int canEvaluate(symbol *operand);


/******************
 * Operators to negate and identity for a number
 * ****************/

symbol *identity (symbol *op);
symbol *inversion (symbol *op);

/*********
 * BOOLEAN operators
 * ****************/
/**
 * Returns the anonymous symbol resulting from applying the NOT operator to the
 * operand symbol.
 */
symbol *notOp(symbol *operand);

symbol *andOp (symbol *o1, symbol *o2);

symbol *orOp (symbol *o1, symbol *o2);



/*******************
 * Arithmetic operators
 * ******************/

int validArithOperator (symbol *operand);

symbol *addOp (symbol *o1, symbol *o2);

symbol *subtractOp (symbol *o1, symbol *o2);

symbol *multOp (symbol *o1, symbol *o2);

symbol *intDivOp (symbol *o1, symbol *divisor);

symbol *realDivOp (symbol *o1, symbol *divisor);

symbol *modOp (symbol *o1, symbol *o2);

/************************
 * comparison operators
 * *********************/

symbol *equalOp (symbol *o1, symbol *o2);
symbol *notEqualOp (symbol *o1, symbol *o2);
symbol *lessThanOp (symbol *o1, symbol *o2);
symbol *greaterThanOp (symbol *o1, symbol *o2);
symbol *greaterThanEqualOp (symbol *o1, symbol *o2);
symbol *lessThanEqualOp (symbol *o1, symbol *o2);
#endif
