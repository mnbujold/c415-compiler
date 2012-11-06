/**
 * Author: Daniel Chui
 * Modified from original file from Paul Vandermeer
 * Implementation of Symbol table header
 */
 
/* Include guard, prevents redefinition of functions. */
#ifndef SYMBOL_H
#define SYMBOL_H

#include <glib.h>

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
  TC_STRING,
  TC_SCALAR,
  TC_ARRAY,
  TC_RECORD,
  TC_SUBRANGE
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
#define OC_ERROR 6
#define OC_RETURN 7
*/
enum object_class {
  OC_CONST,
  OC_VAR,
  OC_FUNC,
  OC_PROC,
  OC_PARAM,
  OC_TYPE,
  OC_ERROR,
  OC_RETURN
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
  struct type_desc *index_type;
  struct type_desc *obj_type;
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
  struct type_desc *mother_type;
};

/* Description fields */
struct location_t{
  /* See ASC... */
  int display; 
  int offset;
};

struct const_desc{
  struct symbol_rec *type;         /* */
  union{                           /* Value of const */ 
    int integer;
    int boolean;
    double real;
    char *string;
    char character;
  }value;
};

struct var_desc{
  struct symbol_rec *type;
};

struct procedure_desc{
  GArray *params;
};

struct function_desc{
  GArray *params;
  struct type_desc *return_type;
};

  
struct param_desc{
  struct type_desc *type;
};

struct type_desc{
  type_class type;
  union{
    struct tc_integer *integer;
    struct tc_real *real;
    struct tc_boolean *boolean;
    struct tc_char *character;
    struct tc_string *string;
    struct tc_scalar *scalar;
    struct tc_array *array;
    struct tc_record *record;
    struct tc_subrange *subrange;
  }desc;
};



struct symbol_rec {
  char *name;         /* Name of symbol */
  object_class oc;     /* Class of object (eg. OC_CONST) */
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

symbol *addSymbol (char const *, symbol *);
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
