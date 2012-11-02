#ifndef _symbolh_
#define _symbolh_

typedef struct symbol {
	int id;
	char identifier[256];
	char value[256];
	struct symbol *next;
	struct symbol *last;
} symbol;

symbol *addSymbol(char const *identifier, symbol *symbol);
symbol *findSymbol(symbol *in, char *identifier);
void showAllSymbols(symbol *in);
symbol *deleteSymbol(symbol *in, char *identifier);
symbol *deleteAllSymbols(symbol *in);

#endif
