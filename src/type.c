/**
 * Author: James Osgood
 * Type creation, assignment, and checking for the grammar.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "symbol.h"
#include "typeerrors.h"
#include "debug.h"

int
compatibleSym(symbol *sym1, symbol *sym2) {
    return compatible(sym1->desc.type_attr, sym1->desc.type_attr); // Incorrect, but I don't know if I'm actually going to use this ...
}

int
compatible(struct type_desc *type1, struct type_desc *type2) {
    // T1 and T2 are the exact same type:
    if (type1 == type2) {
        return 1;
    }
    type_class tc1 = type1->type;
    type_class tc2 = type2->type;
    
    // T1 and T2 are integers and/or reals:
    if ((tc1 == TC_INTEGER || tc1 == TC_REAL)
    && (tc2 == TC_INTEGER || tc2 == TC_REAL)) {
        return 1;
    }
    return 0;
}

int
assignmentCompatibleSym(symbol *sym1, symbol *sym2) {
    return compatible(sym1->desc.type_attr, sym1->desc.type_attr); // Incorrect, but I don't know if I'm actually going to use this ...
}

int
assignmentCompatible(struct type_desc *type1, struct type_desc *type2) {
    // T1 and T2 are the same type:
    if (type1 == type2) {
        return 1;
    }
    type_class tc1 = type1->type;
    type_class tc2 = type2->type;
    
    // check if booleans ...
    
    // T1 is of type real and T2 is of type integer:
    if (tc1 == TC_REAL && tc2 == TC_INTEGER) {
        return 1;
    }
    
    // T1 and T2 are assignment compatible arrays:
    if (tc1 == TC_ARRAY && tc2 == TC_ARRAY) {
    
        return arrayAssignmentCompatible(type1->desc.array,
                                         type2->desc.array);
    }
    
    return 0;
}

int
arrayAssignmentCompatible(struct tc_array *array1, struct tc_array *array2) {
    return 0;
//     int objCompat = 1;
//     int indEquiv = 1;
//     struct type_desc *ot1 = array1->obj_type;
//     struct type_desc *ot2 = array2->obj_type;
//     
//     if (!(assignmentCompatible(ot1, ot2) && assignmentCompatible(ot2, ot1))) {
//         objCompat = 0;
//         illArrayAssignObjError();
//     }
//     
//     // index checking ...
//     
//     return objCompat && indEquiv;
}

symbol *
createConstant(type_class type, int intValue, double realValue, char charValue) {
    struct const_desc *constant = calloc(1, sizeof(struct const_desc));    
    char *typeName;

    if (type == TC_INTEGER) {
        typeName = "integer";
        constant->value.integer = intValue;
        constant->hasValue = 1;
    } else if (type == TC_REAL) {
        typeName = "real";
        constant->value.real = realValue;
        constant->hasValue = 1;
    } else if (type == TC_CHAR) {
        typeName = "char";
        constant->value.character = charValue;
        constant->hasValue = 1;
    } else if (type == TC_BOOLEAN) {
        typeName = "boolean";
        constant->value.integer = intValue;  
        constant->hasValue = 1;      
    } else {
        typeName = NULL; // You asked for it. Well, not really, but I'm lazy.
        constant->value.integer = 0;
        constant->hasValue = 0;
    }
    symbol *constSym = calloc(1, sizeof(symbol));
    constSym->name = NULL;
    constSym->oc = OC_CONST;
    constSym->desc.const_attr = constant;
    constSym->symbol_type = topLevelLookup(typeName);
    
    return constSym;
}

symbol *
createStringConstant(const char *string) {
    int len = strlen(string) + 1;
    symbol *low = createConstant(TC_INTEGER, 1, 0.0, 0);
    symbol *high = createConstant(TC_INTEGER, len, 0.0, 0);
    
    symbol *indexSym = createArrayIndex(low, high);
    symbol *objectSym = topLevelLookup("char");
    
    return createArray(indexSym, objectSym);
}

symbol *
createErrorType() {
    struct type_desc *errorType = calloc(1, sizeof(struct type_desc));
    errorType->type = TC_ERROR;
    
    return createTypeSym(NULL, errorType);
}

symbol *
createTypeSym(const char *name, struct type_desc *type) {
    symbol *typeSym = calloc(1, sizeof(symbol));
    typeSym->name = name;
    typeSym->oc = OC_TYPE;
    typeSym->desc.type_attr = type;
    
    return typeSym;
}

void
addProgramSymbols(const char *program, const char *input, const char *output) {
    struct type_desc *fileType = calloc(1, sizeof(struct type_desc));
    symbol *fileSym = createTypeSym(NULL, fileType);

    addNewSymbol(program, fileSym, OC_PROGRAM); // Kinda cheating here, but no one should be touching these ...
    addNewSymbol(input, fileSym, OC_PARAM);
    addNewSymbol(output, fileSym, OC_PARAM);
}

symbol *
addNewSymbol(const char *id, symbol *type, object_class objClass) {
    if (localLookup(id) == NULL) {
        symbol *newSym;
        if (type == NULL) {
            typeNotDefinedError(id);
            type = createErrorType();
        } else if (globalLookup(type->name) != NULL) { // A named type.
            addSymbol(type->name, type);
        }
        newSym = createSymbol(id, type, objClass, NULL);
        
        addSymbol(id, newSym);
    } else {
        symExistsError(id);
    }
    return type;
}

symbol *
addNewType(const char *id, symbol *type) {
    if (localLookup(id) == NULL) {
        if (type == NULL) {
            typeNotDefinedError(id);
            type = createErrorType();
        } else if (globalLookup(type->name) != NULL) { // A named type.
            addSymbol(id, type);
            
            if (localLookup(type->name) == NULL) { // Need to bring into local scope.
                addSymbol(type->name, type);
            }
            
            return type;
        }
        symbol *newType= createSymbol(id, NULL, OC_TYPE, (void *) type->desc.type_attr);
        
        addSymbol(id, newType);
    } else {
        symExistsError(id);
    }
    return type;
}

symbol *
addNewVar(const char *id, symbol *type) {
    if (localLookup(id) == NULL) {
        if (type == NULL) {
            typeNotDefinedError(id);
            type = createErrorType();
        } else if (localLookup(type->name) == NULL
                && globalLookup(type->name) != NULL) { // A named type. Need to bring into local scope.
            addSymbol(type->name, type);
        }
        symbol *newVar = createSymbol(id, type, OC_VAR, (void *) createVarDesc());
        
        addSymbol(id, newVar);
    } else {
        symExistsError(id);
    }
    return type;
}

symbol *
addNewConst(const char *id, symbol *result) {
    if (localLookup(id) == NULL) {
        result->name = id;
        addSymbol(id, result);
    } else {
        symExistsError(id);
    }
    return result;
}

struct type_desc *
addNewSymbolAnonType(const char *id, struct type_desc *type, object_class objClass) {
    if (localLookup(id) == NULL) {
        symbol *newSym;
        if (type == NULL) {
            typeNotDefinedError(id);
            struct type_desc *errorType = calloc(1, sizeof(struct type_desc));
            errorType->type = TC_ERROR;
            
            newSym = createSymbolAnonType(id, errorType, objClass, NULL);
        } else {
            newSym = createSymbolAnonType(id, type, objClass, NULL);
        }
        addSymbol(id, newSym);
    } else {
        symExistsError(id);
    }
    return type;
}

symbol *
getType(const char *id) {
    symbol *typeSymbol = globalLookup(id);
    
    if (typeSymbol == NULL) {
        symNotDefinedError(id);
        return NULL;
      //TODO: error no symbol for this
    }
    if (OC_TYPE == typeSymbol->oc) {
      DEBUG_PRINT(("is an oc type\n"));
      DEBUG_PRINT(("typeSymbol: %p\n", typeSymbol->desc.type_attr));
      return typeSymbol;
    } else {
        symNotDefinedError(id);
      //TODO: error abot it not being a type class
      //O no. what to do here?
      return NULL;
    }
}

symbol *
createScalarList(GArray *nameList) {
    int listSize = nameList->len;
    GArray *scalarList = g_array_new(1, 1, sizeof(symbol *));
    
    struct tc_const *constTypeClass = calloc(1, sizeof(struct tc_const));
    constTypeClass->len = listSize;
    struct type_desc *constType = calloc(1, sizeof(struct type_desc));
    constType->type = TC_CONST;
    constType->desc.enumeration = constTypeClass;
    symbol *typeSym = createTypeSym(NULL, constType);
    
//     struct const_desc *constDesc;
    symbol *scalar;
    int i, tmp;
    
    for (i = 0; i < listSize; i++) {
//         constDesc = calloc(1, sizeof(struct const_desc));
//         constDesc->type = constType;
//         constDesc->value.integer = i + 1;
//         
//         scalar = calloc(1, sizeof(symbol));
//         scalar->name = g_array_index(nameList, const char *, i);
//         scalar->oc = OC_CONST;
//         scalar->desc.const_attr = constDesc;
        
        const char *name = g_array_index(nameList, const char *, i);
        tmp = i + 1;
        scalar = localLookup(name);
        if (scalar == NULL) { // New symbol.
            scalar = createSymbol(name, typeSym, OC_CONST, (void *) &tmp);
        } else if ((scalar->oc != OC_CONST)
                || (scalar->symbol_type->desc.type_attr->type != TC_CONST)) {
                symNotValidEnumError();
                continue;
        }
        addSymbol(name, scalar);
        
        g_array_append_val(scalarList, scalar);
    }
    g_array_free(nameList, 0); // Just free the wrapper, but not the names themselves.
    
    struct tc_scalar *scalarTypeClass = calloc(1, sizeof(struct tc_scalar));
    scalarTypeClass->len = listSize;
    scalarTypeClass->const_list = scalarList;
    
    struct type_desc *scalarListType = calloc(1, sizeof(struct type_desc));
    scalarListType->type = TC_SCALAR;
    scalarListType->desc.scalar = scalarTypeClass;
    
    return createTypeSym(NULL, scalarListType);
}

GArray *
addScalar(GArray *scalarList, const char *scalar) {
    if (scalarList == NULL) {
        scalarList = g_array_new(1, 1, sizeof(const char *));
    }
    int listSize = scalarList->len;
    int duplicate = 0;
    int i;
    
    for (i = 0; i < listSize; i++) {
        if (strcmp(g_array_index(scalarList, const char *, i), scalar) == 0) {
            duplicate = 1;
        }
    }
    
    if (localLookup(scalar) == NULL && duplicate == 0) {
        g_array_prepend_val(scalarList, scalar); // Added in 'correct' order.
    } else {
        symExistsError(scalar);
    }
    return scalarList;
}

symbol *
createArray(symbol *indexType, symbol *objType) {
    DEBUG_PRINT (("Inside create array\n"));
    DEBUG_PRINT (("index type: %p object type: %p\n", indexType, objType));
    // indexType->symbol_type is NULL! Fix it!
    int indexClass = indexType->symbol_type->desc.type_attr->type;
    int size = 0;

    if (indexClass == TC_SUBRANGE) {
        struct tc_subrange *subrange =
                         indexType->symbol_type->desc.type_attr->desc.subrange;
        size = subrange->high - subrange->low + 1;
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
    newType->type = TC_ARRAY;
    newType->desc.array = newArray;
    
    return createTypeSym(NULL, newType);
}

symbol *
createArrayIndex(symbol *low, symbol *high) {
    object_class highClass = high->oc;
    
    if (highClass == OC_CONST) {
        if (low == NULL) {
            arrayMissLowerError();
            return createErrorType();
        }
        object_class lowClass = low->oc;
        
        if (lowClass != OC_CONST) {
            arrayLowerNotConstError();
            return createErrorType();
        }
        symbol *highType = high->symbol_type;
        symbol *lowType = low->symbol_type;
        
        if (lowType != highType) {
            arrayBoundTypeError();
            return createErrorType();            
        }
        type_class typeClass = lowType->desc.type_attr->type;
        int highValue;
        int lowValue;
        
        if (typeClass == TC_INTEGER) {
            highValue = high->desc.const_attr->value.integer;
            lowValue = low->desc.const_attr->value.integer;
        } else if (typeClass == TC_BOOLEAN) {
            highValue = high->desc.const_attr->value.boolean;
            lowValue = low->desc.const_attr->value.boolean;
        } else {
            arrayBoundInvalidError();
            return createErrorType();
        }   
        
        if (lowValue > highValue) {
            lowerGreaterThanUpperError();
            return createErrorType();  
        }
        struct tc_subrange *subrange = calloc(1, sizeof(struct tc_subrange));
        subrange->low = lowValue;
        subrange->high = highValue;
        subrange->len = highValue - lowValue + 1;
        subrange->mother_type = lowType;
        
        struct type_desc *subrangeType = calloc(1, sizeof(struct type_desc));
        subrangeType->type = TC_SUBRANGE;
        subrangeType->desc.subrange = subrange;
        
        return createTypeSym(NULL, subrangeType);        
    } else if (highClass == OC_TYPE) {
        // Special Cases!
    }
    arrayIndexTypeError();
    return createErrorType();
}

symbol *
createRecord(GArray *fieldList) {
    struct tc_record *newRecord = calloc(1, sizeof(struct tc_record));
    newRecord->field_list = fieldList;
    
    struct type_desc *newType = calloc(1, sizeof(struct type_desc));
    newType->type = TC_RECORD;
    newType->desc.record = newRecord;
    
    return createTypeSym(NULL, newType);
}

GArray *
addField(GArray *fieldList, symbol *newField) {
    if (fieldList == NULL) {
        fieldList = g_array_new(1, 1, sizeof(symbol *));
    }
    int listSize = fieldList->len;
    const char *newName = newField->name;
    int i;
    
    for (i = 0; i < listSize; i++) {
        if (strcmp(g_array_index(fieldList, symbol *, i)->name, newName) == 0) {
            duplicateFieldError(newName);
            return fieldList;
        }
    }
    g_array_prepend_val(fieldList, newField); // Added in 'correct' order.
    
    return fieldList;
}
