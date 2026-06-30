#ifndef MINIGIT_CORE_OID_H
#define MINIGIT_CORE_OID_H

#include <stdbool.h>
#include <stdint.h>

#include "minigit/platform/hash.h"
#include "minigit/utils/errors.h"

#define MG_OID_HEX_SIZE (MG_SHA1_RAW_SIZE * 2)
#define MG_OID_HEX_BUFFER_SIZE (MG_OID_HEX_SIZE + 1)
#define MG_OID_DIR_PREFIX_LEN 2

typedef struct {
    char hex[MG_OID_HEX_BUFFER_SIZE];
} mg_oid_t;

bool mg_oid_is_valid_hex(const char *hex);
int mg_oid_from_hex(const char *hex, mg_oid_t *out, mg_error_t *err);
void mg_oid_from_raw_sha1(const uint8_t raw[MG_SHA1_RAW_SIZE], mg_oid_t *out);

#endif
