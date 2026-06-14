#ifndef HASH_CONTENT_H
#define HASH_CONTENT_H

#include <openssl/evp.h>
#include <stdbool.h>
#include <unistd.h>

#include "utils/errors.h"

#define EVP_SHA1_HASH_LENGTH 20
#define SHA1_HASH_HEX_LENGTH (EVP_SHA1_HASH_LENGTH * 2)
#define SHA1_DIR_PREFIX_LEN 2
#define HEADER_SIZE 64
#define HASH_CONTENT_COMMAND "hash-content"

#define BLOB_READ_MODE "rb"
#define BLOB_WRITE_MODE "wb"
#define BLOB_FILE_DIRECTORY ".minigit/objects"

typedef struct {
    bool write;
    bool use_stdin;
    char *file_path;
} HashContentArgs;

int hashContent(HashContentArgs *args, mg_error_t *err);
int handleHashContentArgsFromCLI(int argc, char **args_in, HashContentArgs *args_out, mg_error_t *err);

#endif /* HASH_CONTENT_H */
