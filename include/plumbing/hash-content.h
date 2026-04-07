#ifndef HASH_CONTENT_H
#define HASH_CONTENT_H

#include "utils/read.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/evp.h>

#define HEADER_SIZE 64
#define EVP_SHA1_HASH_LENGTH 20

#define MAX_ARGS 3
#define OPTION_STD_IN "--stdin"
#define OPTION_PATH "--path="
#define OPTION_WRITE "-w"

typedef struct {
    bool write;
    bool use_stdin;
    char *file_path;
} HashContentArgs;

void hashContent(HashContentArgs *args);
bool handleHashContentArgsFromCLI(int argc, char **args_in, HashContentArgs *args_out);

#endif /* HASH_CONTENT_H */
