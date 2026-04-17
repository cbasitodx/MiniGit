#ifndef REV_PARSE_H
#define REV_PARSE_H

#include <stdbool.h>
#include <stdio.h>

#include "utils/errors.h"

// BIG FAT TODO: TAKE THIS SHIT AND MAKE IT INTO A PARSER THAT CAN BE USED BY cat-file AND rev-parse.
// THIS IS THE WAY git DOES IT.
// I HATE REFACTORS :crying-emoji:

typedef struct {
    FILE *file_ptr; // whoever uses this is responsible of closing it...
    char *rev_name;
    char *rev_hash;
    char *rev_header; // whoever uses this is responsible of freeing it...
} RevParseArgs;

int revParse(RevParseArgs *args, mg_error_t *err);
int handleRevParseArgsFromCLI(int argc, char **args_in, RevParseArgs *args_out, mg_error_t *err);

#endif /* REV_PARSE_H */
