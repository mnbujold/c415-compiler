/**
 * Author: James Osgood
 * Type creation, assignment, and checking for the grammar.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "symbol.h"

symbol *
addNewSymbol(const char *id, symbol *type, int objClass) {
    if (localLookup(id) == NULL) {
        symbol *newSym = createSymbol(id, type, objClass, NULL);
        addSymbol(id, newSym);
    } else {
        printf("type error\n");
        // type error ...
    }
    return type;
}

struct type_desc *
addNewSymbolAnonType(const char *id, struct type_desc *type, int objClass) {
    if (localLookup(id) == NULL) {
        symbol *newSym = createSymbolAnonType(id, type, objClass, NULL);
#if DEBUG
  printf ("DEBUG: Inside add new anonymous type symbol\n");
#endif
        addSymbol(id, newSym);
    } else {
        printf("type error\n");
        // type error ...
    }
    return type;
}

struct type_desc *
getType(const char *id) {
#if DEBUG
  printf ("DEBUG: inside get type\n");
#endif
    symbol *typeSymbol = globalLookup (id);
    if (typeSymbol == NULL) {
      printf ("tpe symbole fasilgsad");
      //TODO: error no symbol for this
    }
    if (OC_TYPE == typeSymbol->oc) {
      return typeSymbol->desc.type_attr;
    }
    else {
      printf ("Type sagsdagdsa");
      //TODO: error abot it not being a type class
      //O no. what to do here?
      return NULL;
    }
#if DEBUG
  printf ("DEBUG: inside get type\n");
#endif
}

struct type_desc *
createScalarList(GArray *scalarList) {
    return NULL;
}

GArray *
addScalar(GArray *scalarList, const char *scalar) {
    return NULL;
}

struct type_desc *
createArray(struct type_desc *indexType, struct type_desc *objType) {
    int indexClass = indexType->class;
    int size = 0;
    
    if (indexClass == TC_SUBRANGE) {
        struct tc_subrange *subrange = indexType->desc.subrange;
        size = subrange->high - subrange->low;
    } else if (indexClass == TC_INTEGER) {
        // ...
    } else if (indexClass == TC_CHAR) {
        // ...
    } 
    
    struct tc_array *newArray = calloc(1, sizeof(struct tc_array));
    newArray->size = size;
    newArray->index_type = indexType;
    newArray->obj_type = objType;
    
    struct type_desc *newType = calloc(1, sizeof(struct type_desc));
    newType->class = TC_ARRAY;
    newType->desc.array = newArray;
    
    return newType;
}

struct type_desc *
createRecord(GArray *fieldList) {
    struct tc_record *newRecord = calloc(1, sizeof(struct tc_record));
    newRecord->field_list = fieldList;
    
    struct type_desc *newType = calloc(1, sizeof(struct type_desc));
    newType->class = TC_RECORD;
    newType->desc.record = newRecord;
    
    return newType;
}

GArray *
addField(GArray *fieldList, symbol *newField) {
    if (fieldList == NULL) {
        fieldList = g_array_new(1, 1, sizeof(symbol *));
    }
    int listSize = fieldList->len;
    char *newName = newField->name;
    int i;
    
    for (i = 0; i < listSize; i++) {
        if (strcmp(g_array_index(fieldList, symbol *, i)->name, newName) == 0) {
            printf("Duplicate name!\n");
            // duplicate record error ...
            return fieldList;
        }
    }
    g_array_prepend_val(fieldList, newField); // Added in 'correct' order.
    
    return fieldList;
}
