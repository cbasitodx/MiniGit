#ifndef HASH_CONTENT_H
#define HASH_CONTENT_H

#include <openssl/evp.h>
#include <stdbool.h>
#include <unistd.h>

#define HEADER_SIZE 64

typedef struct {
    bool write;
    bool use_stdin;
    char *file_path;
} HashContentArgs;

void hashContent(HashContentArgs *args);
bool handleHashContentArgsFromCLI(int argc, char **args_in, HashContentArgs *args_out);

#endif /* HASH_CONTENT_H */
