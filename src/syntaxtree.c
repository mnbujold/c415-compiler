#include <stdlib.h>
#include <stdarg.h>
#include <glib.h>

#include "syntaxtree.h"

GNode *
createNewNode(node_type type, symbol *symbol, struct pf_invok *pf_invok, GPtrArray *garray) {
    node_info *data = calloc(1, sizeof(node_info));
    data->type = type;
    data->symbol = symbol;
    
    rule_and_node *rule_node = calloc(1, sizeof(rule_and_node));
    rule_node->hasNode = 1;
    rule_node->node = data;
    
    if (symbol != NULL) {
        rule_node->rule.symbol = symbol;
    } else if (pf_invok != NULL) {
        rule_node->rule.pf_invok = pf_invok;
    } else if (garray != NULL) {
        rule_node->rule.garray = garray;
    }
    
    return g_node_new(rule_node);
}

node_type
getNodeType(GNode *node) {
    if (node == NULL || node->hasNode == 0) {
        return NT_NONE;
    }
    return node->data->node->type;
}

GNode *
createNode(node_type type, GNode *n args, ...) {
    GNode *newNode = createNewNode(type, NULL, NULL, NULL);
    int i;
    va_list argList;
    
    va_start(argList, n args);
    
    for (i = 1; i <= n_args; i += 1) {
        g_node_append(newNode, va_arg(argList, GNode *));
    }
    
    return newNode;
}

GNode *
createSymbolNode(symbol *symbol) {
    return createNewNode(NT_SYMBOL, symbol, NULL, NULL);
}

GNode *
createPF_InvokNode(struct pf_invok *pf_invok) {
    return createNewNode(NT_SYMBOL, NULL, pf_invok, NULL);
}

GNode *
createGArrayNode(GPtrArray *garray) {
    return createNewNode(NT_SYMBOL, NULL, NULL, garray);
}

GNode *
createArrayNode(symbol *result, GNode *n args, ...) {
    GNode *newNode = createNewNode(NT_ARRAY_ACCESS, result, NULL, NULL);
    int i;
    va_list argList;
    
    va_start(argList, n args);
    
    for (i = 1; i <= n_args; i += 1) {
        g_node_append(newNode, va_arg(argList, GNode *));
    }
    
    return newNode;
}

GNode *
createExprNode(node_type type, symbol *result, GNode *n args, ...) {
    GNode *newNode = createNewNode(type, result, NULL, NULL);
    int i;
    va_list argList;
    
    va_start(argList, n args);
    
    for (i = 1; i <= n_args; i += 1) {
        g_node_append(newNode, va_arg(argList, GNode *));
    }
    
    return newNode;
}

symbol *
extractSymbol(GNode *node) {
    return node->data->rule.symbol;
}

symbol *
extractType(GNode *node) {
    return node->data->rule.symbol->type;
}

const char *
extractID(GNode *node) {
    return node->data->rule.pf_invok->id;
}

GPtrArray *
extractParamList(GNode *node) {
    return node->data->rule.pf_invok->paramList;
}

GNode *
getProcNode(const char *procname) {
    return createSymbolNode(globalLookup(procname));
}



// 
// void
// initSyntaxTree() {
//     syntaxTree = g_node_new(createNode(NT_PROGRAM, NULL));
//     currentNode = syntaxTree;
// }
// 
// void
// addNode(node_type type) {
//     GNode *child = createNode(type, NULL);
//     g_node_append(currentNode, child);
//     
//     currentNode = child;
// }
// 
// void
// addSymbolNode(symbol *symbol) {
//     GNode *child = createNode(NT_SYMBOL, symbol);
//     g_node_append(currentNode, child);
//     
//     currentNode = child;
// }
// 
// void
// moveUp(){
//     GNode *parent = currentNode->parent;
//     if (parent != NULL) {
//         currentNode = parent;
//     }
// }
