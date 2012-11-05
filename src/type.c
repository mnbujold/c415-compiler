/**
 * Author: James Osgood
 * Type creation, assignment, and checking for the grammar.
 */

#include <stdio.h>
#include "type.h"

struct symbol *
addNewSymbol(const char *id, struct symbol *type, int obj_class) {
    if (localLookup(id) == NULL) {
        struct symbol *newSym = NULL;
        // create the new symbol ...
        if (obj_class == OC_CONST) {
            // ...
        } else if (obj_class == OC_VAR) {
            // ...
        } else if (obj_class == OC_TYPE) {
            // ...
        } else if (obj_class == OC_PROC) {
            // ...
        } else if (obj_class == OC_FUNC) {
            // ...
        } else if (obj_class == OC_PARM) {
            // ...
        }
        
        addSymbol(id, newSym);
    } else {
        printf("type error\n");
        // typeError(...)
    }
    return type;
}
