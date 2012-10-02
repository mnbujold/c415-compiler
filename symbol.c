#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "symbol.h"

symbol *addSymbol(symbol *in, char *identifier, char *value) {
	symbol *sNew = NULL;
	sNew = (symbol*)malloc(sizeof(symbol));
	if(identifier != NULL) {
		strcpy(sNew->identifier, identifier);
	} else {
		return NULL;
	} /*if*/
	if(value != NULL) {
		strcpy(sNew->value, value);
	} else {
		memset(sNew->value,'\0', 256);
	} /* if */
	sNew->next = NULL;
	sNew->last = sNew;
	if(in == NULL) {
		in = sNew;
	} else {
		in->last->next = sNew;
		in->last = sNew;
	} /*if*/
	return in;
} /*addSymbol*/

symbol *deleteSymbol(symbol *in, char *identifier) {
	symbol *find = in, *prev = NULL;
	while(find != NULL) {
		if(strcmp(find->identifier,identifier) == 0) {
			break;
		} /* if */
		prev = find;
		find = find->next;
	} /* while */
	if(prev != NULL && find != NULL) {
		prev->next = find->next;
	} else {
		if(find != NULL) {
			in = find->next;
		} else {
			printf("Item not found!  No changes made!\n");
		} /* if */
	} /* if */
	if(find != NULL) {
		free(find);
	} /* if */
	return in;
} /*deleteSymbol*/

symbol *findSymbol(symbol *in, char *identifier) {
	symbol *find = NULL;
	for(find=in; find != NULL && strcmp(find->identifier,identifier) != 0; find = find->next);
	return find;
} /*findSymbol*/

void showAllSymbols(symbol *in) {
	while(in != NULL) {
		printf("Operator:>%s | Value:>%s\n", in->identifier, in->value);
		in = in->next;
	} /* while */
} /*showAllSymbols*/

symbol *deleteAllSymbols(symbol *in) {
	symbol *del;
	while(in != NULL) {
		/*printf("del=>%s\n", in->identifier);*/
		del = in;
		in = in->next;
		free(del);
	} /*while*/
	/*printf("All Symbols deleted\n");*/
	return in;
} /*deleteAllSymbols*/
