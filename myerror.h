#ifndef _myerrorh_
#define _myerrorh_

typedef struct myerror {
	int		id;
	char		message[256];
	char		*text;
	int		location;
	int		errorTextLength;
	int 		line;
	struct myerror 	*next;
	struct myerror 	*last;
} myerror;

myerror *addError(myerror *in, const char *message, int location, int line);
myerror *findError(myerror *in, char *message);
void showAllErrors(myerror *in);
void updateErrorText(myerror *in, char* text);
myerror *deleteError(myerror *in, char *message);
myerror *deleteAllErrors(myerror *in);
myerror *detectDupError(myerror *in, int location, int line);
char *createErrorText(int size, int *errorTextLength);
char *appendErrorText(char *oldString, char *newString, int *errorTextLength);
void writeAllErrors(myerror *in, FILE *outFile);

void add();
int countlines (char *);
#endif
