#include <glib.h>

#include "symbol.h"
#include "type.h"

#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

GNode *syntaxTree;

// It's like a mini-grammar!

enum node_type {            // Children:
    NT_PROGRAM,             //  0 NT_DECLS NT_STAT_LIST
    
    NT_DECLS,               //  1 NT_VAR_DECL_LIST NT_PROC_DECL_LIST
    
    NT_VAR_DECL_LIST,       //  2 NT_SYMBOL ... NT_SYMBOL
    
    NT_PROC_DECL_LIST,      //  3 NT_PROC_DECL ... NT_PROC_DECL
    NT_PROC_DECL,           //  4 NT_SYMBOL NT_DECLS NT_STAT_LIST
    
    NT_STAT_LIST,           //  5 NT_STAT ... NT_STAT
    NT_STAT,                //  6 NT_ASSIGNMENT or NT_PROC_INVOK or NT_IF or NT_IF_ELSE or NT_WHILE or NT_CONTINUE or NT_EXIT
    
    NT_ASSIGNMENT,          //  7 NT_VAR NT_EXPR
    NT_VAR,                 //  8 NT_SYMBOL or NT_ARRAY_ACCESS or NT_RECORD_ACCESS
    NT_ARRAY_ACCESS,        //  9 NT_VAR (base array variable) NT_EXPR ... NT_EXPR (index ... index)
    
    NT_EXPR,                // 10 NT_VAR or NT_FUNC_INVOK or (operation)
    
    NT_ISEQUAL,             // 11 NT_EXPR NT_EXPR
    NT_NOTEQUAL,            // 12 NT_EXPR NT_EXPR
    NT_LESSTHAN,            // 13 NT_EXPR NT_EXPR
    NT_GREATERTHAN,         // 14 NT_EXPR NT_EXPR
    NT_LESSTHANEQUALS,      // 15 NT_EXPR NT_EXPR
    NT_GREATERTHANEQUALS,   // 16 NT_EXPR NT_EXPR
    NT_AND,                 // 17 NT_EXPR NT_EXPR
    NT_OR,                  // 18 NT_EXPR NT_EXPR
    NT_NOT,                 // 19 NT_EXPR
    NT_PLUS,                // 20 NT_EXPR NT_EXPR
    NT_MINUS,               // 21 NT_EXPR NT_EXPR
    NT_MULTIPLY,            // 22 NT_EXPR NT_EXPR
    NT_DIVIDE,              // 23 NT_EXPR NT_EXPR
    NT_DIV,                 // 24 NT_EXPR NT_EXPR
    NT_MOD,                 // 25 NT_EXPR NT_EXPR
    NT_IDENTITY,            // 26 NT_EXPR
    NT_INVERSION,           // 27 NT_EXPR
    
    NT_PROC_INVOK,          // 28 NT_SYMBOL (procedure) NT_EXPR ... NT_EXPR (argument ... argument)
    NT_FUNC_INVOK,          // 29 NT_SYMBOL (function) NT_EXPR ... NT_EXPR (argument ... argument)
    
    NT_RECORD_ACCESS,       // 30 NT_SYMBOL (record) NT_SYMBOL (field)
    
    NT_SYMBOL,              // 31 none
    
    NT_IF,                  // 32 NT_EXPR NT_STAT_LIST
    NT_IF_ELSE,             // 33 NT_EXPR NT_STAT_LIST (if statements) NT_STAT_LIST (else statements)
    
    NT_WHILE,               // 34 NT_EXPR NT_STAT_LIST
    
    NT_CONTINUE,            // 35 none
    NT_EXIT,                // 36 none
    NT_NONE,                // 37 none (should never occur)
    
    // Now, some more just for me:
    NT_VAR_DECL_PART, NT_PROC_DECL_PART, NT_PROC_HEADING, NT_COMPOUND_STAT,
    NT_SIMPLE_STAT, NT_PLIST_FINVOK    // 38 - 44
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

#endif
