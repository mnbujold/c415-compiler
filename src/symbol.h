/**
 * Author: Daniel Chui
 * Modified from original file from Paul Vandermeer
 * Implementation of Symbol table header
 */
 
/* Include guard, prevents redefinition of functions. */
#ifndef SYMBOL_H
#define SYMBOL_H

#include <glib.h>


#define MAX_LEVEL 14

/* Defines which field of union is being used for readability */
/* Type Class defs */
/**
#define TC_INTEGER 0
#define TC_REAL 1
#define TC_BOOLEAN 2
#define TC_CHAR 3
#define TC_STRING 4
#define TC_SCALAR 5
#define TC_ARRAY 6
#define TC_RECORD 7
#define TC_SUBRANGE 8
*/
enum type_class {
  TC_INTEGER,
  TC_REAL,
  TC_BOOLEAN,
  TC_CHAR,
  TC_CONST,
  TC_STRING,
  TC_SCALAR,
  TC_ARRAY,
  TC_RECORD,
  TC_SUBRANGE,
  TC_ERROR
};

typedef enum type_class type_class;

/* Object Class definintions */
/**
#define OC_CONST 0
#define OC_VAR 1
#define OC_FUNC 2
#define OC_PROC 3
#define OC_PARM 4
#define OC_TYPE 5
#define OC_PROGRAM 6
#define OC_RETURN 7
*/
enum object_class {
  OC_CONST,
  OC_VAR,
  OC_FUNC,
  OC_PROC,
  OC_PARAM,
  OC_TYPE,
  OC_PROGRAM
};

typedef enum object_class object_class;

/* Structs for defining type classes of variables */
struct tc_integer{
  int len; //32 bits?
  
};

struct tc_real{
  int len;
};

struct tc_char{
  int len;
};

struct tc_boolean{
  int len;
};

struct tc_const{
  /* individual enumeration constant */
  int len;
};

struct tc_string{
  int high;
  int len;
};

struct tc_scalar{
  /* const_list */
  int len;
  GArray *const_list;
};
struct tc_array{
  int size;
  struct symbol_rec *index_type;
  struct symbol_rec *obj_type;
  int maxIndex;
  int minIndex;
  int len;
};

struct tc_record{
  int len;
  GArray *field_list;
};

struct tc_subrange{
  int len;
  int low;
  int high;
  struct symbol_rec *mother_type;
};

struct tc_file{
  int len; // Doesn't actually do anything - just for type comparisons.
};

/* Description fields */
struct location_t{
  /* See ASC... */
  int display; 
  int offset;
};

struct const_desc{
  //struct symbol_rec *type;         /* */
  union{                           /* Value of const */ 
    int integer;
    int boolean;
    double real;
    char *string;
    char character;
  }value;
};

struct var_desc{
  //symbol *type;
};

struct procedure_desc{
  GPtrArray *params;
};

struct function_desc{
  GPtrArray *params;
  struct symbol_rec *return_type;
  // Need something for the return 'value' to check if the function actually returns. Maybe a void *.
};


struct param_desc{
  //symbol *type;
};

struct type_desc{
  type_class type;
  union{
    struct tc_integer *integer;
    struct tc_real *real;
    struct tc_boolean *boolean;
    struct tc_char *character;
    struct tc_const *enumeration;
    struct tc_string *string;
    struct tc_scalar *scalar;
    struct tc_array *array;
    struct tc_record *record;
    struct tc_subrange *subrange;
    struct tc_file *file;
  }desc;
};



struct symbol_rec {
  char const *name;         /* Name of symbol */
  object_class oc;     /* Class of object (eg. OC_CONST) */
  struct symbol_rec *symbol_type; // Will refactor this ...
  //symbol *typeSymbol; //A pointer to the type that this symbol is
  union {             /* Class-specific attributes */
    struct const_desc *const_attr;
    struct var_desc *var_attr;
    struct function_desc *func_attr;
    struct procedure_desc *proc_attr;
    struct param_desc *parm_attr;
    struct type_desc *type_attr;
  }desc;
};



typedef struct symbol_rec symbol;

extern GQueue* symbol_table;

/* Function defs */
int getCurrentLevel();
void printLevel();
void printSymbol();

void *addSymbol (char const *, symbol *);

symbol *localLookup (char const *);
symbol *globalLookup (char const *);

void showAllSymbols();

/**
Create a symbol with parameters: identifier, type symbol, int object class, 
pointer * value
*/
symbol *createSymbol (char const *, symbol *, int, void *);
/**
 * Create a symbol with parameters: identifier, anonymous type type_desc, enum
 * object class, pointer value.
 */
symbol *createSymbolAnonType (char const *, struct type_desc *, object_class, void *);

symbol *createSymbolFunction (char const *, struct function_desc*);
symbol *createSymbolType (char const *, type_class);

void removeSymbol (char const *);

/**
 Our symbol table will need multiple levels, so we will
 need to add and remove levels as a new scope is defined
*/

void pushLevel ();
void popLevel ();


GHashTable *createNewTable();


void init_table ();
void free_symbol_table();
#endif
