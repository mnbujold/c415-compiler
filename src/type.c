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
    object_class sym1_oc = sym1->oc;
    object_class sym2_oc = sym2->oc;

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
    } else if (tcSym1 == tcSym2) {
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
    // TODO: need some error printing in here!
  
    struct tc_array *sym1ArrayDescription = getArrayDescription (sym1);
    struct tc_array *sym2ArrayDescription = getArrayDescription (sym2);
    //obviously if the objects are not the same, then can't assign
    
    if (getTypeClass(sym1ArrayDescription->obj_type) != getTypeClass(sym2ArrayDescription->obj_type)) {
      return 0;
    }
    if (getTypeClass (sym1ArrayDescription->index_type) != getTypeClass (sym2ArrayDescription->index_type)) {
      return 0;
    }
    
    //TODO: Check they are exactly the same enumerated type if we are using
    //enum
    
    if (sym1ArrayDescription->minIndex != sym2ArrayDescription->minIndex) {
      return 0;
    }
    if (sym1ArrayDescription->maxIndex != sym2ArrayDescription->maxIndex) {
      return 0;
    }
    
    //if (sym1->
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
    return 1;
}

/**
 * Note: We assume we know the symbol is an array
 * If it is not, we are screwed
 */
type_class getArrayType (symbol *sym) {
    
    struct tc_array *arrayDescription = getArrayDescription (sym);
    //printf ("Type of array: %d\n", getTypeClass (arrayDescription->obj_type));
    return getTypeClass (arrayDescription->obj_type);
}


/**
 * Given symbol sym, check 
 * if it is an array of characters, or a string
 */
int isString (symbol *sym) {
  if (getTypeClass (sym) == TC_ARRAY) {
    if (getArrayType (sym) == TC_CHAR) {
      return 1;
    }
    else {
      return 0;
    }
  }
  return 0;
}
/**
 * Gets String const from symbol
 * returns NULL if it is not a string
 */
char *getString (symbol *sym) {
  if (!isString) {
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
    union constant_values highValue = {.integer = (strlen(string) + 1)};
    symbol *low = createConstant(TC_INTEGER, lowValue);
    symbol *high = createConstant(TC_INTEGER, highValue);
    
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

symbol *
addNewParam(const char *id, const char *typeId, int varParam) {
    symbol *type = globalLookup(typeId);
    if (type == NULL) {
        typeNotDefinedError(typeId);
        type = createErrorType();
    } else if (type->oc != OC_TYPE) {
        symNotATypeError(typeId);
        type = createErrorType();
    }
    return createSymbol(id, type, OC_PARAM, (void *) createParamDesc(varParam));
}

symbol *
addNewProc(const char *id, GPtrArray *paramList) {
    symbol *newProc;
    
    if (localLookup(id) == NULL) {
        struct tc_none *noneType = calloc(1, (sizeof(struct tc_none)));
        struct type_desc *typeDesc = calloc(1, (sizeof(struct type_desc)));
        typeDesc->type = TC_NONE;
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
    symbol *paramType;
    int listSize = paramList->len;
    int i;
    
    for (i = 0; i < listSize; i++) {
        newParam = (symbol *) g_ptr_array_index(paramList, i);
        paramType = newParam->symbol_type;
        
        if (paramType->desc.type_attr->type != TC_ERROR) {
            addSymbol(paramType->name, paramType);
        }
        addSymbol(newParam->name, newParam);
    }
    
    return newProc;
}


symbol *
addNewFunc(const char *id, const char *typeId, GPtrArray *paramList) {
    symbol *newFunc;
    symbol *returnType = globalLookup(typeId);
    
    if (localLookup(id) == NULL) {
        if (returnType == NULL) {
            typeNotDefinedError(typeId);
            returnType = createErrorType();
        }
        newFunc = createSymbol(id, returnType, OC_FUNC,
                               createFunctionDesc(paramList, returnType));
        
        addSymbol(id, newFunc);
    } else {
        symExistsError(id);
        newFunc = createErrorSym(OC_FUNC);
        newFunc->name = id;
    }
    pushLevel();
    
    symbol *newParam;
    symbol *paramType;
    int listSize = paramList->len;
    int i;
    
    for (i = 0; i < listSize; i++) {
        newParam = (symbol *) g_ptr_array_index(paramList, i);
        paramType = newParam->symbol_type;
        
        if (paramType->desc.type_attr->type != TC_ERROR) {
            addSymbol(paramType->name, paramType);
        }
        addSymbol(newParam->name, newParam);
    }
    
    if (localLookup(newFunc->name) == NULL) {
        addSymbol(newFunc->name, newFunc);
    } else {
        symExistsError(newFunc->name);
    }
    
    if (returnType->desc.type_attr->type != TC_ERROR
     && localLookup(newFunc->name) == NULL) {
        addSymbol(newFunc->name, newFunc);
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
        return createErrorType();
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
      return createErrorType();
    }
}

symbol *
createScalarList(GPtrArray *nameList) {
    int listSize = nameList->len;
    GPtrArray *scalarList = g_ptr_array_new();
    
    struct type_desc *scalarListType = calloc(1, sizeof(struct type_desc));
    scalarListType->type = TC_SCALAR;
    
    symbol *scalarType = createTypeSym(NULL, scalarListType);
    
//     struct tc_const *constTypeClass = calloc(1, sizeof(struct tc_const));
//     constTypeClass->len = listSize;
//     struct type_desc *constType = calloc(1, sizeof(struct type_desc));
//     constType->type = TC_CONST;
//     constType->desc.enumeration = constTypeClass;
//     symbol *typeSym = createTypeSym(NULL, constType);
    
    symbol *scalar;
    int i, tmp;
    
    for (i = 0; i < listSize; i++) {
        const char *name = (const char *) g_ptr_array_index(nameList, i);
        tmp = i + 1;
        scalar = localLookup(name);
        if (scalar == NULL) { // New symbol.
            scalar = createSymbol(name, scalarType, OC_CONST, (void *) &tmp);
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
    int size = 0;

    if (indexClass != TC_SUBRANGE) {
        arrayIndexTypeError();
        return createErrorType();
    }
    struct tc_subrange *subrange = indexType->desc.type_attr->desc.subrange;
    size = subrange->high - subrange->low + 1;
    
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
        } else if (typeClass == TC_CHAR) {
            highValue = high->desc.const_attr->value.character;
            lowValue = low->desc.const_attr->value.character;
        } else if (typeClass == TC_CONST) {
            highValue = high->desc.const_attr->value.integer;
            lowValue = low->desc.const_attr->value.integer;
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
    } else if (low == NULL && highClass == OC_TYPE) {
        type_class type = high->desc.type_attr->type;
        int correct = (type == TC_INTEGER || type == TC_CHAR || type == TC_SCALAR);
        int lowValue;
        int highValue;
        
        if (type == TC_INTEGER){
            int maxInt = topLevelLookup("maxint")->desc.const_attr->value.integer;
            lowValue = (-1) * maxInt;
            highValue = maxInt;
        } else if (type == TC_CHAR) {
            lowValue = 0;
            highValue = 256; // 2^8 = maximum char value;
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
            return high;
        }
    }
    arrayIndexTypeError();
    return createErrorType();
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

symbol *arrayAccessWithIndex (symbol *array, symbol *index) {
  //printf ("Within array access\n");
  if (getTypeClass (array) != TC_ARRAY) {
    addTypeError ("Trying to access something that is not an array");
    return createErrorType();
  }
  //check if index is the right indexing type
  struct tc_array *arrayDescription = array->desc.type_attr->desc.array;
  if (!(index->symbol_type  == arrayDescription->index_type)) {
    return createErrorType ();
  }
  
  //check if it is within bounds for constant
  //assume constant has value
  if (index->oc = OC_CONST) {
    int value = index->desc.const_attr->value.integer;
    if (value > arrayDescription->minIndex || value < arrayDescription->maxIndex) {
      //TODO:
      //Er...Because arrays are not actually assigned, just return this
      return arrayDescription->obj_type;
    }
    else {
      //arrayOutOfBoundsError (); 
      return createErrorType();
    }
  }
  
  //TODO: We will need to be able to access with a variable as well
  
}

symbol *recordAccess (symbol *record, symbol *key) {
  
  //check that record is a record
  if (getTypeClass (record) != TC_RECORD) {
    addTypeError ("Trying to access something that is not a record");
    return createErrorType();

  }
  GPtrArray *fieldList = record->desc.type_attr->desc.record->field_list;
  int numElements = fieldList->len;
  int i = 0;
  const char *keyString = key->name;
  while (i < numElements) {
    symbol *element = (symbol *) g_ptr_array_index (fieldList, i);
    const char *elementString = element->name;
    if (strcmp (keyString, elementString) == 0) {
      return element;
    }
    i++;
  }
  //otherwise not in here, return error
  return createErrorType();
  
  
}


void doVarAssignment (symbol *var, symbol *expr) {
//   if (var->oc != OC_VAR) {
//     assignmentError ();
//   }
  if (assignmentCompatibleSym(var, expr, 1) == 1) {
    //What does it even mean to assign right now...
    //Point to the expression, as far as I can tell

    symbol *varLookup = globalLookup(var->name);
    
    if (varLookup != NULL && varLookup->oc == OC_FUNC) {
        varLookup->desc.func_attr->returnValSet = 1;
    }    
    //var->desc.var_attr->expression = expr;
  } else {
    //They are not assignment compatible, give error
    // do nothing assignmentCompatibilityError();
  }
}

void
checkFuncValSet(symbol *func) {
    if (func->oc == OC_FUNC
     && func->symbol_type->desc.type_attr->type != TC_ERROR
     && func->desc.func_attr->returnValSet != 1) {
        missFuncRetError();
    }
}


symbol *
getRecordField(symbol *record, const char *fieldName) {
    if (record->oc != OC_VAR || record->symbol_type->desc.type_attr->type != TC_RECORD) {
        symNotRecordError();
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
    return createErrorSym(OC_VAR);
}

void
callProc(const char *procname, GPtrArray *arguments) {
    checkCallAndArgs(procname, arguments, OC_PROC, "procedure");
}

symbol *
callFunc(const char *funcname, GPtrArray *arguments) {
    int goodCall = checkCallAndArgs(funcname, arguments, OC_FUNC, "function");
    
    if (goodCall == 0) {
        return createErrorSym(OC_FUNC);
    }
    
    return globalLookup(funcname);
}

/**
 * Checks arguments for functions and procedures.
 */
int
checkCallAndArgs(const char *procname, GPtrArray *arguments, object_class oc,
                 const char *callable) {
    symbol *proc = globalLookup(procname);
    int numArgs;
    
    if (proc == NULL) {
        symNotDefinedError(procname);
        return 0;
    } else if (proc->oc != oc) {
        notCallableError(procname, callable);
        return 0;
    }
    
    if (arguments == NULL) {
        numArgs = 0;
    } else {
        numArgs = arguments->len;
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
        
        if (param->desc.parm_attr->varParam == 1 && arg->oc != OC_VAR) {
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

void checkWriteln() {
    if (globalLookup("writeln") != topLevelLookup("writeln")) {
        addTypeError ("invalid procedure call");
    }
}

//TODO: Should not need this for checkpoint 2...
symbol *createAnonymousVar(symbol *o1, symbol *o2) {
  return NULL;
}
