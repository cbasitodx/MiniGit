#ifndef HASH_CONTENT_H
#define HASH_CONTENT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>

#define HEADER_SIZE 64
#define OPTION_STD_IN "--stdin"

void hashContent(int argc, char ***args);

#endif /* HASH_CONTENT_H */