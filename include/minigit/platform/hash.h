#ifndef MINIGIT_PLATFORM_HASH_H
#define MINIGIT_PLATFORM_HASH_H

#include <stddef.h>
#include <stdint.h>

#include "minigit/utils/errors.h"

#define MG_SHA1_RAW_SIZE 20

int mg_sha1(const uint8_t *data, size_t size, uint8_t out_hash[MG_SHA1_RAW_SIZE], mg_error_t *err);

#endif
