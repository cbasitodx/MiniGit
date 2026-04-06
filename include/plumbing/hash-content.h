#ifndef HASH_CONTENT_H
#define HASH_CONTENT_H

#include "read.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/evp.h>

#define HEADER_SIZE 64

#define MAX_ARGS 3
#define OPTION_STD_IN "--stdin"
#define OPTION_PATH "--path="
#define OPTION_WRITE "-w"

void hashContent(int argc, char **args);

#endif /* HASH_CONTENT_H */
