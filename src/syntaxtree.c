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

GNode *
arrayHack(GNode *child, GNode *node) {
    g_node_unlink(child);
    child->parent = NULL;
    
    GNode *var = createNode(NT_VAR, child, NULL);
    
    g_node_prepend(node, var);
    changeType(child, NT_VAR);
    
//     child = var;
    return var;
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

int
arrayEnd(GNode *node) {
    return getNodeType(node) == NT_VAR;
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
        sibling = niceify(sibling)->next;
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
        sibling = createProcDecl(sibling)->next;
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

    // the last child may be an empty stat->simple stat->NULL chain
    GNode *lastChild = g_node_last_child(statList);
    if (lastChild->children->children == NULL) {
         g_node_destroy(lastChild);
    }   
    
    GNode *sibling = statList->children;
    
    while (sibling != NULL) {
        sibling = createStat(sibling)->next;
    }
    
    return statList;
}

GNode *
createStat(GNode *stat) {
    niceify(stat);
    GNode *child = stat->children;
    
    if (getNodeType(child) == NT_SIMPLE_STAT) {
        collapseNode(child);
        child = stat->children;
    }
    
    node_type statType = getNodeType(child);
    
    if (statType == NT_COMPOUND_STAT) {
        return mergeCompoundStat(stat, child);
    } else if (statType == NT_ASSIGNMENT) {
        createAssignment(child);
    } else if (statType == NT_PROC_INVOK) {
        createProcInvok(child);
    } else if (statType == NT_IF) {
        createIf(child);
    } else if (statType == NT_IF_ELSE) {
        createIfElse(child);
    } else if (statType == NT_WHILE) {
        createWhile(child);
    } else { // an "exit" or "continue" statment
        niceify(child);
    }
    
    return stat;
}

GNode *
mergeCompoundStat(GNode *stat, GNode *cmpStat) {
    createStatList(cmpStat);
    GNode *lastChild = g_node_last_child(stat);
    
    collapseNode(stat);
    
    return lastChild; 
}

GNode *
createAssignment(GNode *assign) {
    niceify(assign);
    GNode *child = assign->children;
    
    if (getNodeType(child) == NT_ARRAY_ACCESS) { // hack ...
        child = arrayHack(child, assign);
    }
    createExpr(createVar(child)->next);
    
    return assign;
}

GNode *
createVar(GNode *var) {
    niceify(var);
    GNode *child = var->children;
    node_type varType = getNodeType(child);
    
    if (varType == NT_SYMBOL) {
        niceify(child);
    } else if (varType == NT_ARRAY_ACCESS) {
        createArrayAccess(child);
    } else {    // NT_VAR, implying a record access
        createRecordAccess(child);
    }
    return var;
}

GNode *
createArrayAccess(GNode *array) {
    flattenTree(array, &arrayEnd);
    niceify(array);
    GNode *child = array->children;
    
    if (getNodeType(child) == NT_ARRAY_ACCESS) { // hack ...
        child = arrayHack(child, array);
    }    
    
    GNode *exprSibling = createVar(child)->next;

    while (exprSibling != NULL) {
        exprSibling = createExpr(exprSibling)->next;
    }

    return array;
}

GNode *
createRecordAccess(GNode *var) {
    niceify(var);
    changeType(var, NT_RECORD_ACCESS);    
    GNode *child = var->children;
    
    if (getNodeType(child) == NT_ARRAY_ACCESS) { // hack ...
        child = arrayHack(child, var);
    }
        
    niceify(createVar(child)->next);

    return var;
}

GNode *
createExpr(GNode *expr) {
    if (isExprList(expr)) {
        expr = createExprLeaf(expr);
    } else if (isOp0List(expr)) {
        expr = createExpr0(expr);
    } else if (isOp1List(expr)) {
        expr = createExpr1(expr);        
    } else if (isOp2List(expr)) {
        expr = createExpr2(expr);
    } else if (getNodeType(expr) == NT_EXPR) {
        expr = createBracketExpr(expr);
    } else {   // there should be no else ...
        printf("Do Not Want!\n");
        niceify(expr);
        expr->children = NULL;
    }

    return expr;
}

GNode *
createExprLeaf(GNode *expr) {
    collapseExprList(expr);
    niceify(expr);
    GNode *child = expr->children;
    node_type childType = getNodeType(child);
    
    if (childType == NT_ARRAY_ACCESS) { // hack ...
        child = arrayHack(child, expr);
        childType = getNodeType(child);
    }
    
    if (childType == NT_CONST) {
        niceify(niceify(child)->children);
    } else if (childType == NT_VAR) {
        createVar(child);
    } else {    // NT_FUNC_INVOK
        createFuncInvok(child);
    }
    
    return expr;
}

GNode *
createExpr0(GNode *expr) {
    GNode *parent = expr->parent;
    int position = g_node_child_position(parent, expr);
    
    g_node_unlink(expr);
    expr->parent = NULL;
    
    GNode *newExpr = createNode(NT_EXPR, expr, NULL);
    
    g_node_insert(parent, position, newExpr);
    niceify(newExpr);
    
    return resolveExprChildren(newExpr);
}

GNode *
createExpr1(GNode *expr) {
    niceify(expr);

    return resolveExprChildren(expr);
}

GNode *
createExpr2(GNode *expr) {
    collapseExprList(expr);
    niceify(expr);

    return resolveExprChildren(expr);
}

GNode *
createBracketExpr(GNode *expr) {
    collapseExprList(expr);
    niceify(expr);
    
    return resolveExprChildren(expr);
}

GNode *
createFuncInvok(GNode *funcInvok) {
    return createProcInvok(funcInvok); // basically the same things
}

GNode *
createProcInvok(GNode *procInvok) {
    GNode *child = procInvok->children;

    if (getNodeType(child) != NT_SYMBOL) { // > 0 arguments
        collapseNode(child);
        flattenTree(procInvok, &symbolEnd);
    }
    
    niceify(procInvok);
    GNode *exprSibling = niceify(procInvok->children)->next;
    
    while (exprSibling != NULL) {
        exprSibling = createExpr(exprSibling)->next;
    }
    
    return procInvok;
}

GNode *
createIf(GNode *ifStat) {
    niceify(ifStat);
    createExpr(ifStat->children);
    createCondStatList(ifStat->children->next);
    
    return ifStat;
}

GNode *
createIfElse(GNode *ifElseStat) {
    niceify(ifElseStat);
    createExpr(ifElseStat->children);
    createCondStatList(ifElseStat->children->next);
    createCondStatList(ifElseStat->children->next->next);
    
    return ifElseStat;
}

GNode *
createWhile(GNode *whileStat) {
    niceify(whileStat);
    createExpr(whileStat->children);
    createCondStatList(whileStat->children->next);
    
    return whileStat;
}

GNode *
createCondStatList(GNode *stat) {
    niceify(stat);
    GNode *child = stat->children;
    node_type childType = getNodeType(child);
    
    if (childType == NT_STAT) {
        child = createStat(child);

        if (getNiceType(child) == NT_STAT_LIST) {
            collapseNode(child);
        }
    } else if (childType == NT_IF_ELSE) {
        createIfElse(child);
    } else if (childType == NT_WHILE) {
        createWhile(child);
    } else { // an "exit" or "continue" statment
        niceify(child);
    }
    
    return stat;
}

int
isExprList(GNode *expr) {
    GNode *child = expr->children;
    node_type childType = getNodeType(child);
    
    while (childType == NT_EXPR) {
        if (child->next != NULL) {
            return 0;
        }
        child = child->children;
        childType = getNodeType(child);
    }
    
    return childType == NT_CONST || childType == NT_VAR
        || childType == NT_ARRAY_ACCESS || childType == NT_FUNC_INVOK;
}

int
isOp0List(GNode *expr) {
    node_type type = getNodeType(expr);
    
    return type != NT_EXPR;
}

int
isOp1List(GNode *expr) {
    node_type type = getNodeType(expr);
    node_type childType = getNodeType(expr->children);
    
    return type == NT_EXPR && childType != NT_EXPR;
}

int
isOp2List(GNode *expr) {
    node_type type = getNodeType(expr);
    node_type childType = getNodeType(expr->children);
    node_type grandType = getNodeType(expr->children->children);
        
    return type == NT_EXPR && childType == NT_EXPR && grandType != NT_EXPR;
}

GNode *
collapseExprList(GNode *expr) {
    GNode *child = expr->children;
    node_type childType = getNodeType(child);
    
    while (childType == NT_EXPR) {
        child = collapseNode(child)->children;
        childType = getNodeType(child);
    }
        
    return expr;
}

GNode *
resolveExprChildren(GNode *expr) {
    GNode *child = expr->children;

    niceify(child);
    createExpr(child->children);
    
    if (child->children->next != NULL) {
        createExpr(child->children->next);
    }
    
    return expr;
}

GNode *
collapseNode(GNode *node) {
    GNode *newParent = node->parent;
    GNode *sibling = node->children;
    int position = g_node_child_position(newParent, node);
    int numChildren = g_node_n_children(node);
    
    g_node_unlink(node);
    node->parent = NULL;
    node->children = NULL;

    GNode *nextSibling = sibling;
    int i;
    
    for (i = 0; i < numChildren; i += 1) {
        nextSibling = sibling->next;
        
        sibling->parent = NULL;
        sibling->prev = NULL;
        sibling->next = NULL;

        g_node_insert(newParent, position + i, sibling);
        
        sibling = nextSibling;
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
    
    if ((*treeEnd)(currNode) == 1) {    // already flat, so don't do anything
        return head;
    }
    
    // flatten the list until treeEnd() is true
    while ((*treeEnd)(currNode) == 0) {
        prependNode(head, currNode->next);
        
        currNode = currNode->children;
    }
    GNode *sibling = currNode->next;
    int numChildren = g_node_n_children(currNode->parent);
    int i;
    
    // add siblings of last node first
    for (i = 1; i < numChildren; i += 1) {
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
        
    if (data->hasNode == 0) {   // probably will never be true ...
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

struct proc_head_pair *
createProcHead(GNode *procHeading, GNode *decls) {
    struct proc_head_pair *pair = calloc(1, sizeof(struct proc_head_pair));
    pair->proc_heading = procHeading;
    pair->decls = decls;

    symbol *proc = extractSymbol(procHeading);
    
    if (proc != NULL && getTypeClass(proc) != TC_ERROR && proc->oc == OC_FUNC) {
        proc->desc.func_attr->defnState = 0;
    }
    
    return pair;
}
    