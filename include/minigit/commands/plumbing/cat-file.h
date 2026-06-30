#ifndef CAT_FILE_H
#define CAT_FILE_H

#include <stdbool.h>

#include "minigit/utils/errors.h"

typedef struct {
    bool opt_type;
    bool opt_size;
    bool opt_exists;
    bool opt_print;
    char *object_name;
} CatFileArgs;

int catFile(CatFileArgs *args, mg_error_t *err);
int handleCatFileArgsFromCLI(int argc, char **args_in, CatFileArgs *args_out, mg_error_t *err);
int catFileCommand(int argc, char **argv, mg_error_t *err);

#endif
