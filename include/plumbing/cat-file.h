#ifndef CAT_FILE_H
#define CAT_FILE_H

#include <stdbool.h>
#include <stdio.h>

#include "utils/errors.h"

#define SOME_BIG_NUMBER 1024 // TODO: change this crap

typedef struct {
    bool opt_type;
    bool opt_size;
    bool opt_exists;
    bool opt_print;
    char *object_name;
    FILE *object_file;
    char *object_hash;
    char *object_header;
    bool object_exists;
} CatFileArgs;

int catFile(CatFileArgs *args, mg_error_t *err);
int handleCatFileArgsFromCLI(int argc, char **args_in, CatFileArgs *args_out, mg_error_t *err);

#endif /* CAT_FILE_H */
