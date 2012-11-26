#include <glib.h>

#include "symbol.h"

#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

GNode *syntaxTree = NULL;

// It's like a mini-grammar!

enum node_type {            // Children:
    NT_PROGRAM,             // NT_DECLS NT_STAT_LIST
    
    NT_DECLS,               // NT_VAR_DECL_LIST NT_PROC_DECL_LIST
    
    NT_VAR_DECL_LIST,       // NT_SYMBOL ... NT_SYMBOL
    
    NT_PROC_DECL_LIST,      // NT_PROC_DECL ... NT_PROC_DECL
    NT_PROC_DECL,           // NT_SYMBOL NT_DECLS NT_STAT_LIST
    
    NT_STAT_LIST,           // NT_STAT ... NT_STAT
    NT_STAT,                // NT_ASSIGNMENT or NT_PROC_INVOK or NT_IF or NT_IF_ELSE or NT_WHILE or NT_CONTINUE or NT_EXIT
    
    NT_ASSIGNMENT,          // NT_VAR NT_EXPR
    NT_VAR,                 // NT_SYMBOL or NT_ARRAY_ACCESS or NT_RECORD_ACCESS
    NT_ARRAY_ACCESS,        // NT_VAR (base array variable) NT_EXPR ... NT_EXPR (index ... index)
    
    NT_EXPR,                // NT_VAR or NT_FUNC_INVOK or (operation)
    
    NT_ISEQUAL,             // NT_EXPR NT_EXPR
    NT_NOTEQUAL,            // NT_EXPR NT_EXPR
    NT_LESSTHAN,            // NT_EXPR NT_EXPR
    NT_GREATERTHAN,         // NT_EXPR NT_EXPR
    NT_LESSTHANEQUALS,      // NT_EXPR NT_EXPR
    NT_GREATERTHANEQUALS,   // NT_EXPR NT_EXPR
    NT_AND,                 // NT_EXPR NT_EXPR
    NT_OR,                  // NT_EXPR NT_EXPR
    NT_NOT,                 // NT_EXPR
    NT_PLUS,                // NT_EXPR NT_EXPR
    NT_MINUS,               // NT_EXPR NT_EXPR
    NT_MULTIPLY,            // NT_EXPR NT_EXPR
    NT_DIVIDE,              // NT_EXPR NT_EXPR
    NT_DIV,                 // NT_EXPR NT_EXPR
    NT_MOD,                 // NT_EXPR NT_EXPR
    NT_IDENTITY,            // NT_EXPR
    NT_INVERSION,           // NT_EXPR
    
    NT_PROC_INVOK,          // NT_SYMBOL (procedure) NT_EXPR ... NT_EXPR (argument ... argument)
    NT_FUNC_INVOK,          // NT_SYMBOL (function) NT_EXPR ... NT_EXPR (argument ... argument)
    
    NT_RECORD_ACCESS,       // NT_SYMBOL (record) NT_SYMBOL (field)
    
    NT_SYMBOL,              // none
    
    NT_IF,                  // NT_EXPR NT_STAT_LIST
    NT_IF_ELSE,             // NT_EXPR NT_STAT_LIST (if statements) NT_STAT_LIST (else statements)
    
    NT_WHILE,               // NT_EXPR NT_STAT_LIST
    
    NT_CONTINUE,            // none
    NT_EXIT,                // none
    NT_NONE,                // none (should never occur)
    
    // Now, some more just for me:
    NT_VAR_DECL_PART, NT_PROC_DECL_PART, NT_PROC_HEADING, NT_COMPOUND_STAT,
    NT_SIMPLE_STAT, NT_PLIST_FINVOK    
};

typedef enum node_type node_type;

/**
 * The node in the node of the GLib N-ary Tree.
 * symbol is always NULL unless type == NT_SYMBOL.
 */
struct node_info {
    node_type type;
    symbol *symbol;
};

typedef struct node_info node_info;

struct rule_and_node {
    int hasNode;
    node_info *node;
    union rule_of_node {
        symbol *symbol;
        struct pf_invok *pf_invok;
    } rule;
};

typedef struct rule_and_node rule_and_node;

GNode *getSyntaxTree();

node_type getNodeType(GNode *node);

GNode *createNode(node_type type, GNode *n_args, ...);
GNode *createSingleNode(node_type type);

GNode *createSymbolNode(symbol *symbol);
GNode *createPF_InvokNode(struct pf_invok *pf_invok, GNode *n_args, ...);
GNode *createArrayNode(symbol *result, GNode *n_args, ...);
GNode *createExprNode(node_type type, symbol *result, GNode *n_args, ...);
GNode *createSingleExprNode(node_type type, symbol *result);

GNode *getProcNode(const char *procname);

symbol *extractSymbol(GNode *node);
symbol *extractType(GNode *node);
struct pf_invok *extractPF_Invok(GNode *node);
const char *extractID(GNode *node);
GPtrArray *extractParamList(GNode *node);

/**
 * Initializes the head of the syntax tree to a NT_PROGRAM node and sets it to
 * the current working node.
 */
// void initSyntaxTree();

/**
 * Adds a node of node_type type as a child of the current working node of the
 * tree and changes the current working node to the added child.
 */
// void addNode(node_type type);

/**
 * Adds a node of node_type NT_SYMBOL with symbol as a child of the current
 * working node of the tree and changes the current working node to the added
 * child.
 */
// void addSymbolNode(symbol *symbol);

/**
 * Changes the current working node of the tree to the parent of the current
 * node.
 */
// void moveUp();

/**
 * Returns the node_type of the current working node of the tree.
 */
// node_type getCurrentType();

#endif
