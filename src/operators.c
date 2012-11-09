/**
 * Author: James Osgood
 * Arithmetic and boolean operators.
 */

#include <stdlib.h>
#include <stdio.h>
#include "symbol.h"
#include "type.h"
#include "typeerrors.h"

int
canEvaluate(symbol *operand) {
    if (operand == NULL) {
        return 0;
    }
    
    if (operand->oc != OC_CONST) {
        return 0;
    }
    
    return operand->desc.const_attr->hasValue;
}

symbol *
notOp(symbol *operand) {
    if (operand == NULL) {
        printf("Operand is NULL!\n");
        // error... will this ever happen?
        return createErrorType();
    }
    
    if (operand->symbol_type == NULL
     || operand->symbol_type->desc.type_attr->type != TC_BOOLEAN) {
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
