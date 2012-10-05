#ifndef _myerrorh_
#define _myerrorh_

typedef struct myerror {
	int		id;
	char	message[256];
	char	text[4096];
	int		location;;
	int 	line;
	struct myerror 	*next;
	struct myerror 	*last;
} myerror;

myerror *addError(myerror *in, const char *message, char *text, int location, int line);
myerror *findError(myerror *in, char *message);
void showAllErrors(myerror *in);
myerror *updateErrorText(myerror *in, char* text);
myerror *deleteError(myerror *in, char *message);
myerror *deleteAllErrors(myerror *in);
myerror *detectDupError(myerror *in, int location, int line);
void add();

#endif
