#include <stdio.h>

#ifdef DEBUG
# define DEBUG_PRINT(x) do {printf ("DEBUG: "); printf x; printf ("\n");} while (0)
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

