#include "minigit/core/oid.h"

#include <stdio.h>
#include <string.h>

bool mg_oid_is_valid_hex(const char *hex) {
    if (hex == NULL || strlen(hex) != MG_OID_HEX_SIZE) {
        return false;
    }

    for (int i = 0; i < MG_OID_HEX_SIZE; i++) {
        if (!((hex[i] >= '0' && hex[i] <= '9') ||
              (hex[i] >= 'a' && hex[i] <= 'f'))) {
            return false;
        }
    }

    return true;
}

int mg_oid_from_hex(const char *hex, mg_oid_t *out, mg_error_t *err) {
    if (!mg_oid_is_valid_hex(hex)) {
        return mgSetError(err, MG_ERR_GENERIC, "Invalid object ID");
    }

    memcpy(out->hex, hex, MG_OID_HEX_BUFFER_SIZE);
    return MG_SUCCESS;
}

void mg_oid_from_raw_sha1(const uint8_t raw[MG_SHA1_RAW_SIZE], mg_oid_t *out) {
    for (int i = 0; i < MG_SHA1_RAW_SIZE; i++) {
        snprintf(out->hex + (i * 2), 3, "%02x", raw[i]);
    }
    out->hex[MG_OID_HEX_SIZE] = '\0';
}
