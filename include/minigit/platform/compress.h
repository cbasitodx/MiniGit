#ifndef MINIGIT_PLATFORM_COMPRESS_H
#define MINIGIT_PLATFORM_COMPRESS_H

#include <stddef.h>
#include <stdint.h>

#include "minigit/utils/errors.h"

int mg_inflate_buffer(
    const uint8_t *compressed,
    size_t compressed_size,
    uint8_t *out,
    size_t out_size,
    mg_error_t *err
);

#endif
