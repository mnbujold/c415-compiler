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
  //TODO:
  //hm...how to do this?
  return 0;
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


/**
 * Test to see if 2 symbols are assignment compatible
 * Assigning symbol 2 TO symbol 1
 * sym1 = sym2;
 */
int
assignmentCompatibleSym(symbol *sym1, symbol *sym2, int showErrors) {
    //printf ("In assignment compatible sym\n");
    object_class sym1_oc = sym1->oc;
    object_class sym2_oc = sym2->oc;
    //printf ("Object class: %d\n", sym1_oc);
    if (sym1_oc != OC_VAR && sym1_oc != OC_PARAM) {
        if (showErrors != 0) {
            assignNotVarParamError();
        }
        
        return 0; // sym1 must be a variable or parameter
    }
    
    if (sym2_oc == OC_TYPE) {
        assignToError("type");
        return 0;
    } else if (sym2_oc == OC_PROC) {
        assignToError("procedure");
        return 0;
    }
    
    type_class tcSym1 = getTypeClass(sym1);
    type_class tcSym2 = getTypeClass(sym2);

    if (tcSym1 == TC_ERROR || tcSym2 == TC_ERROR) {
        return 1;
    }
    
    if (tcSym1 == TC_ARRAY && tcSym2 == TC_ARRAY) {
       return arrayAssignmentCompatible (sym1, sym2, showErrors);
      //return array assignment compatiblity
    }
    
    if (tcSym1 == tcSym2) {
        if (sym1->symbol_type == sym2->symbol_type
         || sym1->symbol_type->desc.type_attr == sym2->symbol_type->desc.type_attr) {
            return 1;
        }
        if (showErrors != 0) {
            assignBadTypesError();
        }
        return 0;
    } else if (tcSym1 == TC_REAL && tcSym2 == TC_INTEGER) {
      return 1;
    }
    //TODO: Subrange case: NOT IMPLEMENTED
    if (showErrors != 0) {
        assignBadTypesError();
    }
    return 0;
}

/**
 * Get description of array from symbol
 * Assume that we know symbol is an array
 */
struct tc_array *getArrayDescription (symbol *sym) {
    DEBUG_PRINT (("Inside get array description\n"));
    symbol *tempSymbol = sym;
    while (tempSymbol != NULL){
        DEBUG_PRINT (("temp SYmbol address: %p\n", tempSymbol));
        DEBUG_PRINT (("temp symbol oc: %d\n", tempSymbol->oc));

    if (tempSymbol->oc == OC_TYPE) {
        DEBUG_PRINT(("type description: %p\n", tempSymbol->desc.type_attr));
        DEBUG_PRINT (("Type class: %d\n", getTypeClass (tempSymbol)));
        if (getTypeClass (tempSymbol) == TC_ARRAY) {
            DEBUG_PRINT(("type class: %d\n", getTypeClass (tempSymbol->desc.type_attr->desc.array->obj_type)));
            return tempSymbol->desc.type_attr->desc.array;
        }
    }
    tempSymbol = tempSymbol->symbol_type;
    } 
    return NULL;
}
/**
 * IMPORTANT: Assumes that these 2 symbols are arrays, 
 * We do not check if they are arrays here
 */
int
arrayAssignmentCompatible(symbol *sym1, symbol *sym2, int showErrors) {
    struct tc_array *arrayType1 = sym1->symbol_type->desc.type_attr->desc.array;
    struct tc_array *arrayType2 = sym2->symbol_type->desc.type_attr->desc.array;
    symbol *objType1 = createSymbol(NULL, arrayType1->obj_type, OC_VAR, (void *) createVarDesc());
    symbol *objType2 = createSymbol(NULL, arrayType2->obj_type, OC_VAR, (void *) createVarDesc());
    
    if (assignmentCompatibleSym(objType1, objType2, 0) == 0
     || assignmentCompatibleSym(objType2, objType1, 0) == 0) {
        if (showErrors != 0) {
            illArrayAssignObjError();
        }
        return 0; // object types not assignment compatible both ways
    }
    
    symbol *indType1 = arrayType1->index_type;
    symbol *indType2 = arrayType2->index_type;
    
    if (indexTypesCompatible(indType1, indType2) == 0) {
        if (showErrors != 0) {
            illArrayAssignIndError();
        }
        return 0; // index types not the same
    }
    
    int minIndex1 = arrayType1->minIndex;
    int minIndex2 = arrayType2->minIndex;
    int maxIndex1 = arrayType1->maxIndex;
    int maxIndex2 = arrayType2->maxIndex;
    
    if (minIndex1 != minIndex2) {
        if (showErrors != 0) {
            illArrayAssignMinError(minIndex1, minIndex2);
        }
        return 0; // lower bounds not the same
    }
    
    if (maxIndex1 != maxIndex2) {
        if (showErrors != 0) {
            illArrayAssignMaxError(maxIndex1, maxIndex2);
        }
        return 0; // upper bounds not the same
    }
    
    return 1;
}

/**
 * Returns 1 if index1 and index2 are compatible subrange types. Returns 0 otherwise.
 */
int
indexTypesCompatible(symbol *index1, symbol *index2) {
    if (index1 == index2) {
        return 1;
    }
    
    struct tc_subrange *subrange1 = index1->desc.type_attr->desc.subrange;
    struct tc_subrange *subrange2 = index2->desc.type_attr->desc.subrange;
    
    if (subrange1 == subrange2) {
        return 1;
    }
    
    symbol *indexType1 = subrange1->mother_type;
    symbol *indexType2 = subrange2->mother_type;
    
    if (indexType1 == indexType2 || indexType1->desc.type_attr == indexType2->desc.type_attr) {
        return 1;
    }
    return 0;
}

/**
 * Note: We assume we know the symbol is an array
 * If it is not, we are screwed
 */
type_class getArrayType (symbol *sym) {
    
    struct tc_array *arrayDescription = getArrayDescription (sym);
    return getTypeClass (arrayDescription->obj_type);
}


/**
 * Given symbol sym, check 
 * if it is an array of characters, or a string
 */
int isString (symbol *sym) {
    if (getTypeClass(sym) == TC_ARRAY && getArrayType(sym) == TC_CHAR) {
        return sym->symbol_type->desc.type_attr->desc.array->minIndex == 1;
    }
    return 0;
}
/**
 * Gets String const from symbol
 * returns NULL if it is not a string
 */
char *getString (symbol *sym) {
  if (!isString(sym)) {
    return NULL;
  }
  
  if (sym->oc ==OC_CONST) {
    return sym->desc.const_attr->value.string;
  }
    
   return NULL;
  //now do this for vars
}
symbol *
createConstant(type_class type, union constant_values value) {
    struct const_desc *constant = createConstDesc(value);
    symbol *constSym = calloc(1, sizeof(symbol));
    constSym->name = NULL;
    constSym->oc = OC_CONST;
    constSym->desc.const_attr = constant;

    if (type == TC_INTEGER) {
        constSym->symbol_type = topLevelLookup("integer");        
    } else if (type == TC_REAL) {
        constSym->symbol_type = topLevelLookup("real");
    } else if (type == TC_CHAR) {
        constSym->symbol_type = topLevelLookup("char");
    } else if (type == TC_BOOLEAN) {
        constSym->symbol_type = topLevelLookup("boolean");
    } else if (type == TC_STRING) {
        constSym->symbol_type = stringToArray(value.string);
    } else {
        constSym->symbol_type = NULL; // You asked for it. Well, not really, but I'm lazy.
    }
    
    return constSym;
}

symbol *
stringToArray(const char *string) {
    union constant_values lowValue = {.integer = 1};
    union constant_values highValue = {.integer = strlen(string)};
    symbol *low = createConstant(TC_INTEGER, lowValue);
    symbol *high = createConstant(TC_INTEGER, highValue);
    
    symbol *indexSym = createArrayIndex(low, high);
    symbol *objectSym = topLevelLookup("char");
    
    return createArray(indexSym, objectSym);
}

symbol *
createErrorType(const char *name) {
    struct type_desc *errorType = calloc(1, sizeof(struct type_desc));
    errorType->type = TC_ERROR;
    
    return createTypeSym(name, errorType);
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
            type = createErrorType(NULL);
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
            type = createErrorType(NULL);
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
    symbol *newVar;
    if (localLookup(id) == NULL) {
        if (type == NULL) {
            typeNotDefinedError(id);
            type = createErrorType(NULL);
        } else if (localLookup(type->name) == NULL
                && globalLookup(type->name) != NULL) { // A named type. Need to bring into local scope.
            addSymbol(type->name, type);
        }
        newVar = createSymbol(id, type, OC_VAR, (void *) createVarDesc());
        
        if (localLookup(id) == NULL) {
            addSymbol(id, newVar);
        } else {
            newVar->name = NULL;
            symExistsError(id);
        }
    } else {
        newVar = createSymbol(NULL, type, OC_VAR, (void *) createVarDesc());
        symExistsError(id);
    }
    return newVar;
}

symbol *
addNewConst(const char *id, symbol *result) {
    if (localLookup(id) == NULL) {
        if (result == NULL) {
            result = createErrorSym(OC_CONST);
        }
        result->name = id;
        addSymbol(id, result);
    } else {
        symExistsError(id);
    }
    return result;
}

symbol *
addNewParam(const char *id, const char *typeId, int varParam) {
    symbol *type = globalLookup(typeId);
    if (type == NULL) {
        typeNotDefinedError(typeId);
        type = createErrorType(typeId);
    } else if (type->oc != OC_TYPE) {
        symNotATypeError(typeId);
        type = createErrorType(typeId);
    }
    return createSymbol(id, type, OC_PARAM, (void *) createParamDesc(varParam));
}

symbol *
addNewProc(const char *id, GPtrArray *paramList) {
    symbol *newProc;
    int badDefn = paramList == NULL;
    int listSize = badDefn ? 0 : paramList->len;
    int i;
    symbol *paramType;
    
    if (localLookup(id) == NULL) {
        struct tc_none *noneType = calloc(1, (sizeof(struct tc_none)));
        struct type_desc *typeDesc = calloc(1, (sizeof(struct type_desc)));
        typeDesc->type = badDefn ? TC_ERROR : TC_NONE;
        
        for (i = 0; i < listSize; i++) {
            paramType = ((symbol *) g_ptr_array_index(paramList, i))->symbol_type;
            
            if (paramType->desc.type_attr->type != TC_ERROR
            && localLookup(paramType->name) == NULL) {
                addSymbol(paramType->name, paramType);
            }
        }
        
        typeDesc->desc.none = noneType;
        
        symbol *type = createTypeSym(NULL, typeDesc);
        newProc = createSymbol(id, type, OC_PROC, createProcedureDesc(paramList));
    } else {
        symExistsError(id);
        newProc = createErrorSym(OC_PROC);
        newProc->name = id;
    }
    addSymbol(id, newProc);
    pushLevel();

    symbol *newParam;
    
    for (i = 0; i < listSize; i++) {
        newParam = (symbol *) g_ptr_array_index(paramList, i);
        paramType = newParam->symbol_type;
        
        if (paramType->desc.type_attr->type != TC_ERROR
         && localLookup(paramType->name) == NULL) {
            addSymbol(paramType->name, paramType);
        }
        
        if (localLookup(newParam->name) == NULL) {
            addSymbol(newParam->name, newParam);
        } else {
            symExistsError(newParam->name);
        }
    }
    
    return newProc;
}


symbol *
addNewFunc(const char *id, const char *typeId, GPtrArray *paramList) {
    symbol *newFunc;
    symbol *returnType = globalLookup(typeId);
    int badDefn = paramList == NULL;
    int listSize = badDefn ? 0 : paramList->len;
    int i;
    symbol *paramType;
    
    if (localLookup(id) == NULL) {
        if (badDefn == 1) {
            returnType = createErrorType(NULL);
        } else if (returnType == NULL) {
            typeNotDefinedError(typeId);
            returnType = createErrorType(typeId);
        }
        
        if (returnType->desc.type_attr->type != TC_ERROR
        && localLookup(returnType->name) == NULL) {
            addSymbol(returnType->name, returnType);
        }
        
        for (i = 0; i < listSize; i++) {
            paramType = ((symbol *) g_ptr_array_index(paramList, i))->symbol_type;
            
            if (paramType->desc.type_attr->type != TC_ERROR
            && localLookup(paramType->name) != NULL) {
                addSymbol(paramType->name, paramType);
            }
        }
        
        newFunc = createSymbol(id, returnType, OC_FUNC,
                               createFunctionDesc(paramList, returnType, -1));
    } else {
        symExistsError(id);
        newFunc = createErrorSym(OC_FUNC);
        newFunc->name = id;
    }
    addSymbol(id, newFunc);
    pushLevel();
    
    symbol *newParam;
    
    for (i = 0; i < listSize; i++) {
        newParam = (symbol *) g_ptr_array_index(paramList, i);
        paramType = newParam->symbol_type;
        
        if (paramType->desc.type_attr->type != TC_ERROR
         && localLookup(paramType->name) == NULL) {
            addSymbol(paramType->name, paramType);
        }
        
        if (localLookup(newParam->name) == NULL) {
            addSymbol(newParam->name, newParam);
        } else {
            symExistsError(newParam->name);
        }
    }
    
    if (localLookup(newFunc->name) == NULL) {
        addSymbol(newFunc->name, newFunc);
    } else {
        symExistsError(newFunc->name);
    }
    
    if (returnType == NULL) {
        return newFunc;
    }
    
    if (returnType->desc.type_attr->type != TC_ERROR
     && localLookup(returnType->name) == NULL) {
        addSymbol(returnType->name, returnType);
    }
    
    return newFunc;
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
        typeSymbol = createErrorType(id);
        
        addSymbol(id, typeSymbol);
        
        return typeSymbol;
    }
    if (OC_TYPE == typeSymbol->oc) {
      DEBUG_PRINT(("is an oc type\n"));
      DEBUG_PRINT(("typeSymbol: %p\n", typeSymbol->desc.type_attr));
      return typeSymbol;
    } else {
        symNotDefinedError(id);
        typeSymbol = createErrorType(id);
        
        addSymbol(id, typeSymbol);

        return typeSymbol;
    }
}

symbol *
createScalarList(GPtrArray *nameList) {
    int listSize = nameList->len;
    GPtrArray *scalarList = g_ptr_array_new();
    
    struct type_desc *scalarListType = calloc(1, sizeof(struct type_desc));
    scalarListType->type = TC_SCALAR;
    
    symbol *scalarType = createTypeSym(NULL, scalarListType);    
    symbol *scalar;
    int i;
    union constant_values constVal = { .integer = 0 };
    
    for (i = 0; i < listSize; i++) {
        const char *name = (const char *) g_ptr_array_index(nameList, i);
        constVal.integer = i;
        scalar = localLookup(name);
        if (scalar == NULL) { // New symbol.
            scalar = createSymbol(name, scalarType, OC_CONST, createConstDesc(constVal));
        } else {
                symExistsError(name);
                continue;
        }
        addSymbol(name, scalar);
        g_ptr_array_add(scalarList, scalar);
    }
    g_ptr_array_free(nameList, 0); // Just free the wrapper, but not the names themselves.
    
    struct tc_scalar *scalarTypeClass = calloc(1, sizeof(struct tc_scalar));
    scalarTypeClass->len = listSize;
    scalarTypeClass->const_list = scalarList;
    
    scalarListType->desc.scalar = scalarTypeClass;
    
    return scalarType;
}

GPtrArray *
addScalar(GPtrArray *scalarList, const char *scalar) {
    if (scalarList == NULL) {
        scalarList = g_ptr_array_new();
    }
    int listSize = scalarList->len;
    int duplicate = 0;
    int i;
    
    for (i = 0; i < listSize; i++) {
        if (strcmp((const char *) g_ptr_array_index(scalarList, i), scalar) == 0) {
            duplicate = 1;
        }
    }
    
    if (localLookup(scalar) == NULL && duplicate == 0) {
        g_ptr_array_add(scalarList, (char *) scalar); // Added in 'correct' order. REVERSE!
    } else {
        symExistsError(scalar);
    }
    return scalarList;
}

symbol *
createArray(symbol *indexType, symbol *objType) {
    DEBUG_PRINT (("Inside create array\n"));
    DEBUG_PRINT (("index type: %p object type: %p\n", indexType, objType));
    DEBUG_PRINT (("Type: %d, %d\n", getTypeClass (indexType), getTypeClass (objType)));
#if DEBUG
    showAllSymbols ();
#endif
    int indexClass = indexType->desc.type_attr->type;

    if (indexClass == TC_ERROR) {
        return createErrorType(NULL);
    }
    
    if (indexClass != TC_SUBRANGE) {
        arrayIndexTypeError();
        return createErrorType(NULL);
    }
    struct tc_subrange *subrange = indexType->desc.type_attr->desc.subrange;
    int minIndex = subrange->low;
    int maxIndex = subrange->high;
    int size = maxIndex - minIndex + 1;
    
    struct tc_array *newArray = calloc(1, sizeof(struct tc_array));
    newArray->minIndex = minIndex;
    newArray->maxIndex = maxIndex;
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
            return createErrorType(NULL);
        }
        object_class lowClass = low->oc;
        
        if (lowClass != OC_CONST) {
            arrayLowerNotConstError();
            return createErrorType(NULL);
        }
        symbol *highType = high->symbol_type;
        symbol *lowType = low->symbol_type;
        
        if (lowType != highType
         && lowType->desc.type_attr != highType->desc.type_attr) {
            arrayBoundTypeError();
            return createErrorType(NULL);  
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
        } else if (typeClass == TC_CHAR) {
            highValue = high->desc.const_attr->value.character;
            lowValue = low->desc.const_attr->value.character;
        } else if (typeClass == TC_SCALAR) {
            highValue = high->desc.const_attr->value.integer;
            lowValue = low->desc.const_attr->value.integer;
        } else {
            arrayBoundInvalidError();
            return createErrorType(NULL);
        }   
        
        if (lowValue > highValue) {
            lowerGreaterThanUpperError();
            return createErrorType(NULL);  
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
    } else if (low == NULL && highClass == OC_TYPE) {
        type_class type = high->desc.type_attr->type;
        int correct = (type == TC_INTEGER || type == TC_BOOLEAN || type == TC_CHAR || type == TC_SCALAR);
        int lowValue;
        int highValue;
        
        if (type == TC_INTEGER){
            int maxInt = topLevelLookup("maxint")->desc.const_attr->value.integer;
            lowValue = (-1) * maxInt;
            highValue = maxInt;
            addTypeWarning("index from -maxint to maxint may be too large for system");
        } else if (type == TC_BOOLEAN) {
            lowValue = 0;
            highValue = 1;
        } else if (type == TC_CHAR) {
            lowValue = 0;
            highValue = MAX_CHAR_VALUE;
        } else if (type == TC_SCALAR) {
            GPtrArray *list = high->desc.type_attr->desc.scalar->const_list;
            lowValue = ((symbol *) g_ptr_array_index(list, 0))->desc.const_attr->value.integer;
            highValue = ((symbol *) g_ptr_array_index(list, (list->len)-1))->desc.const_attr->value.integer;
        }
        
        if (correct == 1) {
            struct tc_subrange *subrange = calloc(1, sizeof(struct tc_subrange));
            subrange->low = lowValue;
            subrange->high = highValue;
            subrange->len = highValue - lowValue + 1;
            subrange->mother_type = high;
            
            struct type_desc *subrangeType = calloc(1, sizeof(struct type_desc));
            subrangeType->type = TC_SUBRANGE;
            subrangeType->desc.subrange = subrange;
            
            return createTypeSym(NULL, subrangeType);
        }
    }
    arrayIndexTypeError();
    return createErrorType(NULL);
}

symbol *
createRecord(GPtrArray *fieldList) {
    struct tc_record *newRecord = calloc(1, sizeof(struct tc_record));
    newRecord->field_list = fieldList;
    
    struct type_desc *newType = calloc(1, sizeof(struct type_desc));
    newType->type = TC_RECORD;
    newType->desc.record = newRecord;

    return createTypeSym(NULL, newType);
}

GPtrArray *
addField(GPtrArray *fieldList, symbol *newField) {
    if (fieldList == NULL) {
        fieldList = g_ptr_array_new();
    }
    int listSize = fieldList->len;
    const char *newName = newField->name;
    int i;
    
    for (i = 0; i < listSize; i++) {
        if (strcmp(((symbol *) g_ptr_array_index(fieldList, i))->name, newName) == 0) {
            duplicateFieldError(newName);
            return fieldList;
        }
    }
    g_ptr_array_add(fieldList, newField); // Added in 'correct' order. REVERSE

    return fieldList;
}

GPtrArray *
addParam(GPtrArray *paramList, symbol *newParam) {
    if (paramList == NULL) {
        paramList = g_ptr_array_new();
    }
    
    if (newParam == NULL) {
        return paramList;
    }
  
    const char *newName = newParam->name;
    int listSize = paramList->len;
    int duplicate = 0;
    int i;
    
    for (i = 0; i < listSize; i++) {
        if (strcmp(((symbol *) g_ptr_array_index(paramList, i))->name, newName) == 0) {
            duplicate = 1;
        }
    }
    
    if (duplicate == 0) {
        g_ptr_array_add(paramList, newParam); // Added in 'correct' order. REVERSE
    } else {
        symExistsError(newName);
    }
    return paramList;    
}

symbol *
accessArray(symbol *array, symbol *index) {
    
    if (getTypeClass(array) == TC_ERROR || getTypeClass(index) == TC_ERROR) {
        return createErrorSym(OC_VAR);
    }
    
    if (getTypeClass(array) != TC_ARRAY) {
        symNotArrayError(array->name);
        return createErrorSym(OC_VAR); // symbol not an array
    }
    
    object_class indexOC = index->oc;
    
    if (indexOC != OC_CONST && indexOC != OC_VAR && indexOC != OC_PARAM) {
        symNotAVarParmConstError(NULL);
        return createErrorSym(OC_VAR); // index not a constant, variable, or parameter
    }
    struct tc_array *arrayType = array->symbol_type->desc.type_attr->desc.array;
    symbol *motherType = arrayType->index_type->desc.type_attr->desc.subrange->mother_type;
    symbol *accessType = index->symbol_type;
    
    if (motherType != accessType
     && motherType->desc.type_attr != accessType->desc.type_attr) {
        illArrayAccessIndError();
        return createErrorSym(OC_VAR); // index types not the same
    }
    
    if (indexOC != OC_CONST || index->desc.const_attr->hasValue != 1) {
        return createSymbol(NULL, arrayType->obj_type, OC_VAR, (void *) createVarDesc());
    }
    
    type_class indexType = getTypeClass(index);
    union constant_values values = index->desc.const_attr->value;
    int indexVal;
    
    if (indexType == TC_INTEGER) {
        indexVal = values.integer;
    } else if (indexType == TC_BOOLEAN) {
        indexVal = values.boolean;
    } else { // indexType == TC_CHAR
        indexVal = (int) values.character;
    }
    
    int minIndex = arrayType->minIndex;
    int maxIndex = arrayType->maxIndex;
    
    if (indexVal < minIndex) {
        illArrayAccessMinError(indexVal, minIndex);
        return createErrorSym(OC_VAR); // index below lower bound
    } else if (indexVal > maxIndex) {
        illArrayAccessMaxError(indexVal, maxIndex);
        return createErrorSym(OC_VAR); // index above upper bound
    }
    
    return createSymbol(NULL, arrayType->obj_type, OC_VAR, (void *) createVarDesc());
}

int
doVarAssignment (symbol *var, symbol *expr) {
    if (assignmentCompatibleSym(var, expr, 1) == 1) {
        symbol *varLookup = localLookup(var->name);

        if (varLookup != NULL && varLookup->oc == OC_FUNC && getTypeClass(varLookup) != TC_ERROR) {
            varLookup->desc.func_attr->returnValSet = 1;
            
            return 1;
        }
    }
    
    return 0;    
}

int
checkFuncValSet(symbol *func) {
    if (func->oc == OC_FUNC && func->symbol_type->desc.type_attr->type != TC_ERROR) {
        func->desc.func_attr->defnState = 1;
        if (func->desc.func_attr->returnValSet < 1) {
            missFuncRetError();
            return 0;
        }
    }
    return 1;
}

symbol *
getRecordField(symbol *record, const char *fieldName) {
    object_class oc = record->oc;

    if ((oc != OC_VAR && oc != OC_PARAM && oc != OC_CONST)
      || getTypeClass(record) != TC_RECORD) {
        symNotRecordError(record->name);
        return createErrorSym(OC_VAR);
    }

    GPtrArray *fieldList = record->symbol_type->desc.type_attr->desc.record->field_list;
    int listSize = fieldList->len;
    int i;
    symbol *currField;
    
    for (i = 0; i < listSize; i += 1) {
        currField = (symbol *) g_ptr_array_index(fieldList, i);
        if (strcmp(currField->name, fieldName) == 0) {
            return currField;
        }
    }
    fieldNotInRecordError(record->name, fieldName);
    
    return createErrorSym(OC_VAR);
}

int
callProc(const char *procname, GPtrArray *arguments) {
    return checkCallAndArgs(procname, arguments, OC_PROC, "procedure");
}

symbol *
callFunc(const char *funcname, GPtrArray *arguments) {
    if (specialBuiltinFunc(funcname)) {
        return callBuiltinFunc(funcname, arguments);
    }

    if (checkCallAndArgs(funcname, arguments, OC_FUNC, "function") == 0) {
        return createErrorSym(OC_FUNC);
    }
    
    if (implementedBuiltinFunction(funcname)) {
        symbol *arg = (symbol *) g_ptr_array_index(arguments, 0);
        if (canEvaluate(arg)) {
            return evaluateBuiltin(funcname, arg);
        }
    }
    
    return globalLookup(funcname);
}

/**
 * Checks if funcname is a special builtin.
 */
int
specialBuiltinFunc(const char *funcname) {
    if (strcmp(funcname, "abs") != 0 && strcmp(funcname, "sqr") != 0
     && strcmp(funcname, "ord") != 0 && strcmp(funcname, "pred") != 0
     && strcmp(funcname, "succ") != 0) {
        return 0;
    }
    return globalLookup(funcname) == topLevelLookup(funcname);
}

int
implementedBuiltinFunction(const char *funcname) {
    if (strcmp(funcname, "abs") != 0 && strcmp(funcname, "sqr") != 0
     && strcmp(funcname, "ord") != 0 && strcmp(funcname, "pred") != 0
     && strcmp(funcname, "succ") != 0 && strcmp(funcname, "chr") != 0
     && strcmp(funcname, "odd") != 0 && strcmp(funcname, "round") != 0
     && strcmp(funcname, "trunc") != 0) {
        return 0;
    }
    
    return globalLookup(funcname) == topLevelLookup(funcname);
}

/**
 * Calls funcname with arguments assuming it has been identified as a special builtin.
 */
symbol *
callBuiltinFunc(const char *funcname, GPtrArray *arguments) {
    int numArgs = (arguments == NULL) ? 0 : arguments->len;
    int numParams = globalLookup(funcname)->desc.func_attr->params->len;

    if (numParams > numArgs) {
        notEnoughParamsError("function");
    } else if (numParams < numArgs) {
        tooManyParamsError("function");
    }
    
    if (numArgs == 0) {
        return createErrorSym(OC_FUNC);
    }
    
    symbol *arg = (symbol *) g_ptr_array_index(arguments, 0);
    object_class arg_oc = arg->oc;

    if (arg_oc == OC_TYPE) {
        assignToError("type");
        return createErrorSym(OC_FUNC);
    } else if (arg_oc == OC_PROC) {
        assignToError("procedure");
        return createErrorSym(OC_FUNC);
    }
    type_class arg_tc = getTypeClass(arg);

    if (arg_tc == TC_ERROR) {
        return createErrorSym(OC_FUNC);
    }
 
    if (strcmp(funcname, "abs") == 0 || strcmp(funcname, "sqr") == 0) { //  number
        if (arg_tc == TC_INTEGER || arg_tc == TC_REAL) {
            if (canEvaluate(arg)) {
                return evaluateBuiltin(funcname, arg);
            }
            return createAnonymousConst(arg, NULL);
        }
    } else if (strcmp(funcname, "ord") == 0) { // takes ordinal returns integer
        if (arg_tc == TC_INTEGER || arg_tc == TC_CHAR || arg_tc == TC_SCALAR || arg_tc == TC_BOOLEAN) {
            if (canEvaluate(arg)) {
                return evaluateBuiltin(funcname, arg);
            }
            struct const_desc *constDesc = calloc(1, sizeof(struct const_desc));
            constDesc->hasValue = 0;
            
            return createSymbol(NULL, topLevelLookup("integer"), OC_CONST, (void *) constDesc);
        }
    } else { // (pred or succ) ordinal
        if (arg_tc == TC_INTEGER || arg_tc == TC_CHAR || arg_tc == TC_SCALAR || arg_tc == TC_BOOLEAN) {
            if (canEvaluate(arg)) {
                return evaluateBuiltin(funcname, arg);
            }
            return createAnonymousConst(arg, NULL);
        }
    }
    
    badProcArgError(1, funcname);
    return createErrorSym(OC_FUNC);
}

/**
 * Checks arguments for functions and procedures.
 */
int
checkCallAndArgs(const char *procname, GPtrArray *arguments, object_class oc,
                 const char *callable) {
    symbol *proc = globalLookup(procname);
 
    if (proc == NULL) {
        symNotDefinedError(procname);

        addSymbol(procname, createErrorType(procname));

        return 0;
    } else if (proc->oc != oc) {
        notCallableError(procname, callable);
        return 0;
    }

    if (getTypeClass(proc) == TC_ERROR) {
        return 0;
    }
    
    int numArgs = (arguments == NULL) ? 0 : arguments->len;
    
    if (checkIOProc(procname, 0) == 1) {
        return callIOProc(procname, proc, arguments, numArgs);
    }
    
    GPtrArray *params;
    if (oc == OC_PROC) {
        params = proc->desc.proc_attr->params;
    } else {
        params = proc->desc.func_attr->params;
    }
    int numParams = params->len;
    int minLen = numArgs;
    int goodCall = 1;

    if (numParams > numArgs) {
        notEnoughParamsError(callable);
        goodCall = 0;
    } else if (numParams < numArgs) {
        tooManyParamsError(callable);
        minLen = numParams;
        goodCall = 0;
    }
    symbol *param;
    symbol *arg;
    int i;
    
    for (i = 0; i < minLen; i += 1) {
        param = (symbol *) g_ptr_array_index(params, i);
        arg = (symbol *) g_ptr_array_index(arguments, i);
        
        if (param->desc.parm_attr->varParam == 1 && !(arg->oc == OC_VAR
        || (arg->oc == OC_PARAM && arg->desc.parm_attr->varParam == 1))) {
            missingVarParamError(i + 1, procname);
            goodCall = 0;
        }
        
        if (assignmentCompatibleSym(param, arg, 0) != 1) {
            badProcArgError(i + 1, procname);
            goodCall = 0;
        }
    }
    
    return goodCall;
}

int
callIOProc(const char *procname, symbol *proc, GPtrArray *arguments, int numArgs) {
    int goodCall = 1;
    symbol *arg;
    type_class tc;
    int i;
    
    for (i = 0; i < numArgs; i += 1) {
        arg = (symbol *) g_ptr_array_index(arguments, i);
        tc = getTypeClass(arg);
        
        if (tc != TC_INTEGER && tc != TC_REAL && tc != TC_CHAR
         && isString(arg) == 0) {
            badProcArgError(i + 1, procname);
            goodCall = 0;
        }
    }
    
    return goodCall;
}

struct pf_invok *
createArgList(const char *id, symbol *arg) {
    GPtrArray *args = g_ptr_array_new();
    g_ptr_array_add(args, arg);
    
    struct pf_invok *invok = calloc(1, sizeof(struct pf_invok));
    invok->id = id;
    invok->paramList = args;
    
    return invok;
}

struct pf_invok *
addArgument(struct pf_invok *invok, symbol *arg) {
    g_ptr_array_add(invok->paramList, arg);
    
    return invok;
}

int
checkIOProc(const char *proc_name, int showErrors) {
    if (strcmp(proc_name, "write") != 0 && strcmp(proc_name, "writeln") != 0
     && strcmp(proc_name, "read") != 0 && strcmp(proc_name, "readln") != 0) {
        return 0;
    }

    if (globalLookup(proc_name) != topLevelLookup(proc_name)) {
        if (showErrors == 1) {
            addTypeError("invalid procedure call (not a builtin IO procedure)");
        }
        return 0;
    }
    return 1;
}

int
checkConditional(symbol *expr) {
    if (expr->symbol_type == NULL) {
        addTypeError("conditional cannot be a type");
        return 0;
    } else {
        type_class type = expr->symbol_type->desc.type_attr->type;
        
        if (type != TC_ERROR && type != TC_BOOLEAN) {
            addTypeError("conditional is not of boolean type");
            return 0;
        }
    }
    return 1;
}

int
checkControlFlow(int loopLevel, const char *controlType) {
    if (loopLevel <= 0) {
        ctrlTypeNotInLoopError(controlType);
        return 0;
    }
    return 1;
}

symbol *
createBooleanConst() {
    struct const_desc *constDesc = calloc(1, sizeof(struct const_desc));
    constDesc->hasValue = 0;
    
    return createSymbol(NULL, topLevelLookup("boolean"), OC_CONST, (void *) constDesc);
}

symbol *
createAnonymousConst(symbol *o1, symbol *o2) {
    struct const_desc *constDesc = calloc(1, sizeof(struct const_desc));
    constDesc->hasValue = 0;
    type_class tc1 = getTypeClass(o1);
    type_class tc2;
    
    if (o2 == NULL) {
        tc2 = tc1;
    } else {
         tc2 = getTypeClass(o2);
    }
    
    symbol *numType;
    
    if (tc1 == TC_BOOLEAN) {
        numType = topLevelLookup("boolean");
    } else if (tc1 == TC_REAL || tc2 == TC_REAL) {
        numType = topLevelLookup("real");
    } else if (tc1 == TC_INTEGER) {
        numType = topLevelLookup("integer");
    } else if (tc1 == TC_SCALAR) {
        numType = o1->symbol_type;
    }
    
    return createSymbol(NULL, numType, OC_CONST, (void *) constDesc);
}
