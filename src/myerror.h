#ifndef _myerrorh_
#define _myerrorh_

enum error_type {
    ET_ERROR,
    ET_WARNING
};

typedef enum error_type error_type;

typedef struct myerror {
    error_type errType;
	int		id;
	char		message[256];
	char		*text;
	int		location;
	int		errorTextLength;
	int 		line;
	struct myerror 	*next;
	struct myerror 	*last;
} myerror;

char illegalChar;

myerror *addError(myerror *in, const char *message, int location, int line, error_type errType);
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

int getNumErrors ();
int getNumWarnings ();
#endif
