#include <stdlib.h>
#include <stdarg.h>
#include <glib.h>

#include "syntaxtree.h"

GNode *
createNewNode(node_type type, symbol *symbol, struct pf_invok *pf_invok) {
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
    }
    
    return g_node_new(rule_node);
}

GNode *
getSyntaxTree() {
    return syntaxTree; // ... sooo not finished!
}

GNode *
collapseNode(GNode *node) {
    GNode *newParent = node->parent;
    int numChildren = g_node_n_children(node);
    int i;
    
    // children of node point to node's parent:
    for (i = 0; i < numChildren; i += 1) {
        g_node_nth_child(node, i)->parent = newParent;
    }
    
    // parent of node points to node's children
    newParent->children = node->children;
    
    // free only node
    node->next = NULL;
    node->prev = NULL;
    node->parent = NULL;
    node->children = NULL;
    g_node_destroy(node);
    
    return newParent;
}

node_type
getNodeType(GNode *node) {
    if (node == NULL) {
        return NT_NONE;
    }
    rule_and_node *data = node->data;
        
    if (data->hasNode == 0) {
        return NT_NONE;
    }
    
    return data->node->type;
}

int
noError(int checkSymbol, GNode *n_args, ...) {
    va_list argList;    
    va_start(argList, n_args);
    
    GNode *arg = n_args;
    
    while(arg != NULL) {
        if (getNodeType(arg) == NT_NONE
         || (checkSymbol == 1 && extractSymbol(arg) == NULL)) {
            return 0;
        }
        arg = va_arg(argList, GNode *);
    }

    return 1;
}

GNode *
createNode(node_type type, GNode *n_args, ...) {
    GNode *newNode = createNewNode(type, NULL, NULL);
    va_list argList;
    va_start(argList, n_args);
    
    GNode *arg = n_args;
    
    while(arg != NULL) {
        g_node_append(newNode, arg);        
        arg = va_arg(argList, GNode *);
    }

    return newNode;
}

GNode *
createSingleNode(node_type type) {
    return createNewNode(type, NULL, NULL);
}

GNode *
createSymbolNode(symbol *symbol) {
    return createNewNode(NT_SYMBOL, symbol, NULL);
}

GNode *
createPF_InvokNode(struct pf_invok *pf_invok, GNode *n_args, ...) {
    GNode *newNode = createNewNode(NT_PLIST_FINVOK, NULL, pf_invok);
    va_list argList;
    va_start(argList, n_args);
    
    GNode *arg = n_args;
    
    while(arg != NULL) {
        g_node_append(newNode, arg);        
        arg = va_arg(argList, GNode *);
    }
    
    return newNode;    
}

GNode *
createArrayNode(symbol *result, GNode *n_args, ...) {
    GNode *newNode = createNewNode(NT_ARRAY_ACCESS, result, NULL);
    va_list argList;
    va_start(argList, n_args);
    
    GNode *arg = n_args;
    
    while(arg != NULL) {
        g_node_append(newNode, arg);        
        arg = va_arg(argList, GNode *);
    }
    
    return newNode;
}

GNode *
createExprNode(node_type type, symbol *result, GNode *n_args, ...) {
    GNode *newNode = createNewNode(type, result, NULL);
    va_list argList;
    va_start(argList, n_args);
    
    GNode *arg = n_args;
    
    while(arg != NULL) {
        g_node_append(newNode, arg);        
        arg = va_arg(argList, GNode *);
    }
    
    return newNode;
}

GNode *
createSingleExprNode(node_type type, symbol *result) {
    return createNewNode(type, result, NULL);
}

symbol *
extractSymbol(GNode *node) {
    rule_and_node *data = node->data;
    
    return data->rule.symbol;
}

symbol *
extractType(GNode *node) {
    rule_and_node *data = node->data;
    
    return data->rule.symbol->symbol_type;
}

struct pf_invok *
extractPF_Invok(GNode *node) {
    rule_and_node *data = node->data;
    
    return data->rule.pf_invok;
}

const char *
extractID(GNode *node) {
    rule_and_node *data = node->data;
    
    return data->rule.pf_invok->id;
}

GPtrArray *
extractParamList(GNode *node) {
    rule_and_node *data = node->data;
    
    return data->rule.pf_invok->paramList;
}

GNode *
getProcNode(const char *procname) {
    return createSymbolNode(globalLookup(procname));
}
