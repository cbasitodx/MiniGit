#ifndef CAT_FILE_H
#define CAT_FILE_H

#include <stdbool.h>

typedef struct {
    bool opt_type;
    bool opt_size;
    bool opt_exists;
    bool opt_print;
    char* object;
} CatFileArgs;

void catFile(CatFileArgs *args);
bool handleCatFileArgsFromCLI(int argc, char **args_in, CatFileArgs *args_out);

#endif /* CAT_FILE_H */
