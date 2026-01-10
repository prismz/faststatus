#ifndef HANDLER_H
#define HANDLER_H

#include <stdio.h>

extern const char *dump_path;
extern FILE *dump_fp;
extern int dump_fileno;

int setup_dump_file(void);

void segfault_handler();

#endif  /* HANDLER_H */
