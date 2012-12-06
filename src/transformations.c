/**
 * Author: James Osgood
 * Transformations to the clean and nice version of the syntax tree.
 */

#include <stdio.h>
#include <glib.h>

#include "transformations.h"
#include "symbol.h"

GNode *
setOperatorTypes(GNode *syntaxTree) {
    setOps(syntaxTree);
    
    return syntaxTree;
}

op_flag
setOps(GNode *node) {
    node_type nodeType = getNiceType(node);
    
    if (nodeType == NT_SYMBOL) {
        node_info *data = node->data;

        return (getTypeClass(data->symbol) == TC_REAL) ? REAL_OP : INT_OP;
    } else if (node->children == NULL) {
        return NO_TYPE;
    }
    
    GNode *sibling = node->children;
    op_flag firstType = setOps(sibling);
    op_flag opType;
    
    if (nodeType == NT_RECORD_ACCESS) {
        return setOps(sibling->next);
    }
    
    if (isUnaryOperator(nodeType)) {
        opType = firstType;

        convertOpType(node, opType);
    } else if (isBinaryOperator(nodeType)) {
        op_flag secondType = setOps(sibling->next);
        opType = (firstType == REAL_OP || secondType == REAL_OP) ? REAL_OP : INT_OP;

        convertOpType(node, opType);
    } else {
        opType = firstType;
        sibling = sibling->next;
        
        while (sibling != NULL) {
            setOps(sibling);
            sibling = sibling->next;
        }
    }

    if (nodeType == NT_DIV || nodeType == NT_MOD) {
        return INT_OP;
    } else if (nodeType == NT_DIVIDE) {
        return REAL_OP;
    } 
    
    return opType;
}

int
isUnaryOperator(node_type type) {    
    return type == NT_IDENTITY || type == NT_INVERSION;
}

int
isBinaryOperator(node_type type) {    
    return (NT_ISEQUAL <= type && type <= NT_GREATERTHANEQUALS)
        || (NT_PLUS <= type && type <= NT_MULTIPLY);
}

void
convertOpType(GNode *node, op_flag newType) {
    if (newType == INT_OP) {
        changeType(node, getNiceType(node) + 34);
    } else {
        changeType(node, getNiceType(node) + 51);
    }
}
