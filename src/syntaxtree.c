#include <stdlib.h>
#include <glib.h>

#include "syntaxtree.h"

/**
 * Returns a pointer to a GNode containing a node_info with type and symbol as
 * members.
 */
GNode *
createNode(node_type type, symbol *symbol) {
    node_info *data = calloc(1, sizeof(node_info));
    data->type = type;
    data->symbol = symbol;
    
    return g_node_new(data);
}

void
initSyntaxTree() {
    syntaxTree = g_node_new(createNode(NT_PROGRAM, NULL));
    currentNode = syntaxTree;
}

void
addNode(node_type type) {
    GNode *child = createNode(type, NULL);
    g_node_append(currentNode, child);
    
    currentNode = child;
}

void
addSymbolNode(symbol *symbol) {
    GNode *child = createNode(NT_SYMBOL, symbol);
    g_node_append(currentNode, child);
    
    currentNode = child;
}

void
moveUp(){
    GNode *parent = currentNode->parent;
    if (parent != NULL) {
        currentNode = parent;
    }
}
