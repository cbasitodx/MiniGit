#ifndef CAT_FILE_H
#define CAT_FILE_H

#include <stdbool.h>

// TODO: This should agree with the one in hash-content.h,
// find an idiomatic solution without importing hash-content.h???
#define SHA1_HEX_LENGTH 40

// TODO: Add more features. For now, we'll work exclusively with these 4 flags
// Adding more features means adding a variable number of arguments
#define CAT_FILE_MAX_ARGS 2

#define OPTION_TYPE "-t"
#define OPTION_SIZE "-s"
#define OPTION_EXISTS "-e"
#define OPTION_PRINT "-p"

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
