/**
 * Author: James Osgood
 * Transformations to the clean and nice version of the syntax tree.
 */

#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include <glib.h>
#include "syntaxtree.h"

GNode *setOperatorTypes(GNode *syntaxTree);

#endif
