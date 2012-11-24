#include <glib.h>

#include "symbol.h"

#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

GNode *syntaxTree;
GNode *currentNode;

// It's like a mini-grammar!

enum node_type {            // Children:
    NT_PROGRAM,             // NT_DECL NT_STAT_LIST
    
    NT_DECL,                // NT_VAR_DECL_LIST NT_PROC_DECL_LIST
    
    NT_VAR_DECL_LIST,       // NT_SYMBOL ... NT_SYMBOL
    
    NT_PROC_DECL_LIST,      // NT_PROC_DECL ... NT_PROC_DECL
    NT_PROC_DECL,           // NT_SYMBOL DECL NT_STAT_LIST
    
    NT_STAT_LIST,           // NT_STATEMENT ... NT_STATEMENT
    NT_STATEMENT,           // NT_ASSIGNMENT or NT_PROC_INVOK or NT_IF or NT_WHILE or NT_CONTINUE or NT_EXIT
    
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
    
    NT_WHILE,               // NT_EXPR NT_STAT_LIST
    
    NT_CONTINUE,            // none
    NT_EXIT,                // none
    NT_NONE                 // none (may never occur)
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

/**
 * Initializes the head of the syntax tree to a NT_PROGRAM node and sets it to
 * the current working node.
 */
void initSyntaxTree();

/**
 * Adds a node of node_type type as a child of the current working node of the
 * tree and changes the current working node to the added child.
 */
void addNode(node_type type);

/**
 * Adds a node of node_type NT_SYMBOL with symbol as a child of the current
 * working node of the tree and changes the current working node to the added
 * child.
 */
void addSymbolNode(symbol *symbol);

/**
 * Changes the current working node of the tree to the parent of the current
 * node.
 */
void moveUp();

#endif
