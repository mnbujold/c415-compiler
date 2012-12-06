#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "myerror.h"

extern int prog_listing;

int numErrors = 0;
int numWarnings = 0;

int getNumErrors () {
    return numErrors;
}

int getNumWarnings () {
    return numWarnings;
}

myerror *addError(myerror *in, const char *message, int location, int line, error_type errType) {
    if (errType == ET_ERROR) {
        numErrors += 1;
    } else {
        numWarnings += 1;
    }
	myerror *sNew = NULL;
	int errorTextLength = 0;
    line++;
	if(detectDupError(in, location, line) != NULL) {
		return in;
	} /* if */
	sNew = (myerror*)malloc(sizeof(myerror));
	memset(sNew->message, '\0', 256);
    if (illegalChar != 0) {
        char illegalCharMsg[21];
        sprintf(illegalCharMsg, "illegal character, %c", illegalChar);
        strcpy(sNew->message, illegalCharMsg);
        illegalChar = 0;
    } else if(message != NULL) {
		strcpy(sNew->message, message);
	} else {
		return NULL;
	} /*if*/
	sNew->text = NULL;
	sNew->errorTextLength = 0;
	sNew->line = line - 1;
	sNew->location = location;
	sNew->next = NULL;
	sNew->last = sNew;
    sNew->errType = errType;
	if(in == NULL) {
		in = sNew;
	} else {
		in->last->next = sNew;
		in->last = sNew;
	} /*if*/
	return in;
} /*addError*/

char *createErrorText(int size, int *errorTextLength) {
	char *temp;
	
	temp = (char*)malloc(size);
	if(temp == NULL) {
		printf("Error creating string!  Not enough memory!\n");
	} /* if */
	(*errorTextLength) = size;
	return temp;
} /* createErrorText */

char *appendErrorText(char *oldString, char *newString, int *errorTextLength) {
	char *temp, *realloctemp;
	int oldStringLength, newStringLength;
	int newmemsize = 200;
	/* catch null string */
	if(newString == NULL) return oldString;;
	/* catch null string */
	if(oldString == NULL) {
		oldString = createErrorText(newmemsize, errorTextLength);	
		memset(oldString, '\0', newmemsize);
		(*errorTextLength) = newmemsize;
	} /* if */
	oldStringLength = strlen(oldString);
	newStringLength = strlen(newString);
	if(((oldStringLength + newStringLength)+2) > (*errorTextLength)) {
		temp = (char*)malloc((*errorTextLength));
		memset(temp, '\0', *errorTextLength);
		strcpy(temp, oldString);
		(*errorTextLength) = (*errorTextLength) + newStringLength + newmemsize;
		oldString = (char*)realloc(oldString,(*errorTextLength));
		strcpy(oldString, temp);
		strcat(oldString, newString);
	} else {
		strcat(oldString,newString);
	} /* if */
	return oldString;
} /* apendErrorText */

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
		free(find->text);
		free(find);
	} /* if */
	return in;
} /*deleteError*/

myerror *findError(myerror *in, char *message) {
	myerror *find = NULL;
	for(find=in; find != NULL && strcmp(find->message,message) != 0; find = find->next);
	return find;
} /*findError*/

void updateErrorText(myerror *in, char *text) {
	while(in != NULL&&text != NULL) {
		in->text = appendErrorText(in->text, text, &(in->errorTextLength)); 
		in = in->next;
	} /* while */
} /*showAllErrors*/

void showAllErrors(myerror *in) {
	int nTemp = 0;
    int countTemp;
    
	if(in == NULL) return;
    //int number = 0;
	while(in != NULL) {
          if(prog_listing)
            printf ("{\n");
        const char *errName = (in->errType == ET_ERROR) ? "Error" : "Warning";
        countTemp = (in->errType == ET_ERROR) ? numErrors : numWarnings;

		printf("%s %d! %d:%d - %s\n", errName, countTemp, in->line, in->location, in->message);
        if (in->text != NULL) {
            printf("%s\n", in->text);
        }
		nTemp = in->location;
		while(nTemp > 1) {
			printf(" ");
			nTemp--;
		} /*while*/
		printf("^\n");
		in = in->next;

	} /* while */
        if(prog_listing)
          printf("}\n");
	
} /*showAllErrors*/

void writeAllErrors(myerror *in, FILE *outFile) {
	int nTemp = 0;
	if(in == NULL) return;
	while(in != NULL) {
            const char *errName = (in->errType == ET_ERROR) ? "Error" : "Warning";
            
          fprintf (outFile, "{\n");
          fprintf(outFile, "%s! %d:%d - %s\n", errName, in->line, in->location, in->message);
          fprintf(outFile, "%s\n", in->text);
		nTemp = in->location;
		while(nTemp > 1) {
                  fprintf(outFile, " ");
			nTemp--;
		} /*while*/
		fprintf(outFile,"^\n");
		in = in->next;
	} /* while */
	fprintf(outFile,"}\n");
	
} /*showAllErrors*/


myerror *deleteAllErrors(myerror *in) {
	myerror *del;
	int n = 0;
	while(in != NULL) {
		del = in;
		in = in->next;
		if(del->text != NULL) {
			free(del->text);
		} /* if */
		free(del);
		n++;
	} /*while*/
	return in;
} /*deleteAllErrors*/
