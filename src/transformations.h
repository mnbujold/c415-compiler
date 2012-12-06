/**
 * Author: James Osgood
 * Transformations to the clean and nice version of the syntax tree.
 */

#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include <glib.h>
#include "syntaxtree.h"

enum op_flag {
    NO_TYPE,
    INT_OP,
    REAL_OP
};

typedef enum op_flag op_flag;

GNode *setOperatorTypes(GNode *syntaxTree);

op_flag setOps(GNode *node);
int isUnaryOperator(node_type type);
int isBinaryOperator(node_type type);
void convertOpType(GNode *node, op_flag newType);

#endif
