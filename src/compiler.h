#ifndef COMPILER_H
#define COMPILER_H

FILE *source_file;
FILE *asc_file;
FILE *listing_file;

/* Default options - switchable by command line options*/
int leave_asc = 0;
//extern int prog_listing = 1;
int bounds_check = 1;
int execute = 1;

void usage(void);
void parse_args(int argc, char* argv[]);

#endif
