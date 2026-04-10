#ifndef REV_PARSE_H
#define REV_PARSE_H

#include "utils/errors.h"

#include <stdbool.h>

typedef struct {
    char *rev_name;
} RevParseArgs;

int revParse(RevParseArgs *args, mg_error_t *err);
int handleRevParseArgsFromCLI(int argc, char **args_in, RevParseArgs *args_out, mg_error_t *err);

#endif /* REV_PARSE_H */
