#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <glib.h>

#include "syntaxtree.h"

node_type
getNiceType(GNode *node) {
    node_info *data = node->data;
    
    return data->type;
}

void
displayOldTree(GNode *head, int level) {
    if (head->children == NULL) {
        printf("type at leaf: %d\n", getNodeType(head));
        return;
    }
    printf("type: %d\n", getNodeType(head));
    GNode *sibling = head->children;
    
    while (sibling != NULL) {
        printf("at level: %d\n", level);
        displayOldTree(sibling, level + 1);
        sibling = sibling->next;
    }
}

void
displayNewTree(GNode *head, int level) {
    if (head->children == NULL) {
        printf("type at leaf: %d\n", getNiceType(head));
        return;
    }
    printf("type: %d\n", getNiceType(head));
    GNode *sibling = head->children;
    
    while (sibling != NULL) {
        printf("at level: %d\n", level);
        displayNewTree(sibling, level + 1);
        sibling = sibling->next;
    }
}

void
displayNodeInfo(GNode *node) {
    printf("myself:%p\n", node);
    printf("parent:%p\n", node->parent);
    printf("next:%p\n", node->next);
    printf("prev:%p\n", node->prev);
    printf("children:%p\n", node->children);
    printf("node_type:%d\n", getNodeType(node));
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

GNode *
prependNode(GNode *parent, GNode *firstChild) {
    GNode *secondChild = parent->children;
    
    if (firstChild->prev == NULL) {
        firstChild->parent->children = firstChild->next;
        
        if (firstChild->next != NULL) {
            firstChild->next->prev = NULL;
        }
    } else if (firstChild->next == NULL) {
        firstChild->prev->next = NULL;
    } else {
        firstChild->prev->next = firstChild->next;
        firstChild->next->prev = firstChild->prev;
    }

    firstChild->parent = parent;
    firstChild->prev = NULL;
    firstChild->next = secondChild;
    
    secondChild->prev = firstChild;
    
    parent->children = firstChild;
    
    return firstChild;
}

int
symbolEnd(GNode *node) {
    return getNodeType(node) == NT_SYMBOL;
}

int
procEnd(GNode *node) {
    return getNodeType(node) == NT_PROC_DECL;
}

int
statEnd(GNode *node) {
    return getNodeType(node) == NT_STAT;
}

GNode *
getSyntaxTree() {
    
//     displayOldTree(syntaxTree, 0);
    
    niceify(syntaxTree);
    createStatList(createDecls(syntaxTree->children)->next);

//     printf("\n-----------\n\n");    
//     displayNewTree(syntaxTree, 0);
    
    return syntaxTree;
}

GNode *
createDecls(GNode *decls) {    
    niceify(decls);
    createProcDeclsList(createDeclsList(decls->children)->next);

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

    collapseNode(declsPart);
    flattenTree(declsList, &symbolEnd);
    niceify(declsList);
    GNode *sibling = declsList->children;
    
    while (sibling != NULL) {
        niceify(sibling);
        sibling = sibling->next;
    }

    return declsList;
}

GNode *
createProcDeclsList(GNode *procPart) {
    if (procPart->children == NULL) {
        niceify(procPart);
        changeType(procPart, NT_PROC_DECL_LIST);
                
        return procPart;
    }
    
    GNode *procsList = procPart->children;

    collapseNode(procPart);
    flattenTree(procsList, &procEnd);
    niceify(procsList);
    GNode *sibling = procsList->children;
    
    while (sibling != NULL) {
        createProcDecl(sibling);
        sibling = sibling->next;
    }

    return procsList;
}

GNode *
createProcDecl(GNode *procDecl) {
    niceify(procDecl);
    niceify(procDecl->children);
    createStatList(createDecls(procDecl->children->next)->next);
    
    return procDecl;
}

GNode *
createStatList(GNode *cmpStat) {
    if (cmpStat->children->children->children->children == NULL) {
        niceify(cmpStat);
        changeType(cmpStat, NT_STAT_LIST);
        g_node_destroy(cmpStat->children);
        
        return cmpStat;
    }
    
    GNode *statList = cmpStat->children;

    collapseNode(cmpStat);
    flattenTree(statList, &statEnd);
    niceify(statList);    
    // the last child will be an empty stat
    g_node_destroy(g_node_last_child(statList));
    
    GNode *sibling = statList->children;
    
    while (sibling != NULL) {
        createStat(sibling);
        sibling = sibling->next;
    }
    
    return statList;
}

GNode *
createStat(GNode *stat) {
    niceify(stat);
    
    stat->children = NULL;
    
    return stat;
}

GNode *
collapseNode(GNode *node) {
    GNode *newParent = node->parent;
    GNode *sibling = node->children;
    int position = g_node_child_position(newParent, node);
    
    g_node_unlink(node);
    node->parent = NULL;
    
    int numChildren = g_node_n_children(node);
    int i;
    
    for (i = 0; i < numChildren; i += 1) {
        sibling->parent = NULL;
        g_node_insert(newParent, position + i, sibling);
        
        sibling = sibling->next;
    }
    
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
        prependNode(head, currNode->next);
        
        currNode = currNode->children;
    }
    GNode *sibling = currNode->next;
    
    // add siblings of last node first
    while (sibling != NULL) {
        prependNode(head, sibling);
        
        sibling = sibling->next;
    }
    
    // add last node first
    prependNode(head, currNode);

    // remove the head link tree
    g_node_unlink(nodeToRemove);   

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
    
    if (data->node == NULL) {
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
