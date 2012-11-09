/**
 * Author: James Osgood
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

/**
 * Returns the anonymous symbol resulting from applying the NOT operator to the
 * operand symbol.
 */
symbol *notOp(symbol *operand);

#endif
