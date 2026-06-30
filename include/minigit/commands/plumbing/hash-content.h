#ifndef HASH_CONTENT_H
#define HASH_CONTENT_H

#include <stdbool.h>

#include "minigit/utils/errors.h"

typedef struct {
    bool write;
    bool use_stdin;
    char *file_path;
} HashContentArgs;

int hashContent(HashContentArgs *args, mg_error_t *err);
int handleHashContentArgsFromCLI(int argc, char **args_in, HashContentArgs *args_out, mg_error_t *err);
int hashContentCommand(int argc, char **argv, mg_error_t *err);

#endif
