#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <glib.h>

#include "syntaxtree.h"

void
displayOldTree(GNode *head, int level) {
    if (head->children == NULL) {
        printf("type at leaf: %d\n", getNodeType(head->children));
        return;
    }
    printf("type: %d\n", getNodeType(head));
    GNode *sibling = head->children;
    
    while (sibling != NULL) {
        printf("at level: %d\n", level);
        displayOldTree(sibling, level + 1);
    }
}

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
createNewNiceNode(node_type type, symbol *symbol) {
    node_info *data = calloc(1, sizeof(node_info));
    data->type = type;
    data->symbol = symbol;
    
    return g_node_new(data);
}

GNode *
niceify(GNode *node) {
    rule_and_node *oldData = node->data;
    node->data = oldData->node;
    
    return node;
}

GNode *
changeType(GNode *node, node_type newType) {
    node_info *data = node->data;
    data->type = newType;
    
    return node;
}

int
symbolEnd(GNode *node) {
    return getNodeType(node) == NT_SYMBOL;
}

GNode *
getSyntaxTree() {
    displayOldTree(syntaxTree, 0);
    
    niceify(syntaxTree);
    createStatList(createDecls(syntaxTree->children)->next);
        
    return syntaxTree;
}

GNode *
createDecls(GNode *decls) {
    niceify(decls);
    createProcDeclsList(createDeclsList(decls->children)->next);
    
    printf("done creating decls\n");
    
    return decls;
}

GNode *
createDeclsList(GNode *declsPart) {
    if (declsPart->children == NULL) {
        niceify(declsPart);
        changeType(declsPart, NT_VAR_DECL_LIST);
                
        return declsPart;
    }
    
    GNode *declsList = declsPart->children;
//     int (*endTree)(GNode *) = ;
    
    collapseNode(declsPart);
    flattenTree(declsList, &symbolEnd);
//     flattenTree(declsList, endTree);
    niceify(declsList);
    GNode *sibling = declsList->children;
    
    while (sibling != NULL) {
        niceify(sibling);
        sibling = sibling->next;
    }
    
    printf("done creating decls list\n");
    
    return declsList;
}

GNode *
createProcDeclsList(GNode *procPart) {
    niceify(procPart);
    procPart->children = NULL;
    
    return procPart;
}

GNode *
createStatList(GNode *cmpStat) {
    niceify(cmpStat);
    cmpStat->children = NULL;
    
    return cmpStat;
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
    
    printf("done collapsing a node\n");
    
    return newParent;
}

/**
 * Assumes:
 *      head
 *  |           \
 * next_head    target
 * |         \   
 * |            \
 * next_head     target
 * ...
 */
GNode *
flattenTree(GNode *head, int (*treeEnd)(GNode *)) {
    GNode *currNode = head->children;
    GNode *nodeToRemove = head->children;
    
    // flatten the list until treeEnd() is true
    while ((*treeEnd)(currNode) == 0) {
        g_node_prepend(head, currNode->next);
        
        currNode = currNode->children;
    }

    GNode *sibling = currNode->next;
    
    // add siblings of last node first
    while (sibling != NULL) {
        g_node_prepend(head, sibling);
        
        sibling = sibling->next;
    }
    
    // add last node first
    g_node_prepend(head, currNode);
    
    // remove the head link tree
    g_node_unlink(nodeToRemove);   
    
    printf("done flattening a tree\n");
    
    return head;
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
