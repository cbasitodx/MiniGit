#ifndef REV_PARSE_H
#define REV_PARSE_H

#include <stdio.h>

#include "utils/errors.h"

typedef struct {
    FILE *file_ptr; // whoever uses this is responsible of closing it...
    char *rev_name;
    char *rev_hash;
    char *rev_header; // whoever uses this is responsible of freeing it...
} RevParseArgs;

int revParse(RevParseArgs *args, mg_error_t *err);
int handleRevParseArgsFromCLI(int argc, char **args_in, RevParseArgs *args_out, mg_error_t *err);

#endif /* REV_PARSE_H */
