#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "myerror.h"

myerror *addError(myerror *in, const char *message, char *text, int location, int line) {
	myerror *sNew = NULL;
	if(detectDupError(in, location, line) != NULL) {
		return in;
	} /* if */
	sNew = (myerror*)malloc(sizeof(myerror));
	memset(sNew->message, '\0', 256);
	memset(sNew->text, '\0', 4096);
	if(message != NULL) {
		strcpy(sNew->message, message);
	} else {
		return NULL;
	} /*if*/
	if(text != NULL) {
		strcpy(sNew->text, text);
	} /*if*/
	sNew->line = line;
	sNew->location = location;
	sNew->next = NULL;
	sNew->last = sNew;
	if(in == NULL) {
		in = sNew;
	} else {
		in->last->next = sNew;
		in->last = sNew;
	} /*if*/
	return in;
} /*addError*/

myerror *detectDupError(myerror *in, int location, int line) {
	myerror *find = NULL;
	for(find=in; find != NULL && (find->location != location || find->line != line); find = find->next);
	return find;
} /* detectDupError */

myerror *deleteError(myerror *in, char *message) {
	myerror *find = in, *prev = NULL;
	while(find != NULL) {
		if(strcmp(find->message,message) == 0) {
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
} /*deleteError*/

myerror *findError(myerror *in, char *message) {
	myerror *find = NULL;
	for(find=in; find != NULL && strcmp(find->message,message) != 0; find = find->next);
	return find;
} /*findError*/

myerror *updateErrorText(myerror *in, char *text) {
	while(in != NULL&&text != NULL) {
		strcpy(in->text, text);
		in = in->next;
	} /* while */
} /*showAllErrors*/

void showAllErrors(myerror *in) {
	int nTemp = 0;
	if(in == NULL) return;
	while(in != NULL) {
		printf("Error! line: %d char: %d - %s\n", in->line, in->location, in->message);
		printf("%s\n", in->text);
		nTemp = in->location;
		while(nTemp > 1) {
			printf(" ");
			nTemp--;
		} /*while*/
		printf("^\n");
		in = in->next;
	} /* while */
	printf("\n");
} /*showAllErrors*/

myerror *deleteAllErrors(myerror *in) {
	myerror *del;
	int n = 0;
	while(in != NULL) {
		/*printf("%d--%d\n", in->line, in->location);*/
		del = in;
		in = in->next;
		free(del);
		n++;
	} /*while*/
	/*printf("# of errors deleted:>%d\n", n);*/
	return in;
} /*deleteAllErrors*/
