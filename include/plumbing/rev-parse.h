#ifndef REV_PARSE_H
#define REV_PARSE_H

#include <stdbool.h>

typedef struct {
    char *rev_name;
} RevParseArgs;

void revParse(RevParseArgs *args);
bool handleRevParseArgsFromCLI(int argc, char **args_in, RevParseArgs *args_out);

#endif /* REV_PARSE_H */
