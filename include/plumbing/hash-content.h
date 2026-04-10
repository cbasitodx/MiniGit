#ifndef HASH_CONTENT_H
#define HASH_CONTENT_H

#include <openssl/evp.h>
#include <stdbool.h>
#include <unistd.h>

#include "utils/errors.h"

#define EVP_SHA1_HASH_LENGTH 20
#define HEADER_SIZE 64

typedef struct {
    bool write;
    bool use_stdin;
    char *file_path;
} HashContentArgs;

int hashContent(HashContentArgs *args, mg_error_t *err);
int handleHashContentArgsFromCLI(int argc, char **args_in, HashContentArgs *args_out, mg_error_t *err);

#endif /* HASH_CONTENT_H */
