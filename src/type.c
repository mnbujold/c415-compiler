#include <stdio.h>
#include "type.h"

struct symbol *
addNewSymbol(const char *id, struct symbol *type, int kind) {
    if (localLookup(id) == NULL) {
        struct symbol *newSym = NULL;
        // create the new symbol ...
        /*if (kind == enumeration) {
            // ...
        } else if (kind == variable) {
            // ...
        } else if (kind == type) {
            // ...
        } else if (kind == procedure) {
            // ...
        } else if (kind == function) {
            // ...
        } else if (kind == parameter) {
            // ...
        }*/
        
        addSymbol(id, newSym);
    } else {
        printf("type error\n");
        // typeError(...)
    }
    return type;
}
