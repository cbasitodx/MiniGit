#ifndef CAT_FILE_H
#define CAT_FILE_H

#include <stdbool.h>

#include "utils/errors.h"

typedef struct {
    bool opt_type;
    bool opt_size;
    bool opt_exists;
    bool opt_print;
    char *object;
} CatFileArgs;

int catFile(CatFileArgs *args, mg_error_t *err);
int handleCatFileArgsFromCLI(int argc, char **args_in, CatFileArgs *args_out, mg_error_t *err);

#endif /* CAT_FILE_H */
