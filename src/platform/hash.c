#include "minigit/platform/hash.h"

#include <openssl/evp.h>

int mg_sha1(const uint8_t *data, size_t size, uint8_t out_hash[MG_SHA1_RAW_SIZE], mg_error_t *err) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        return mgSetError(err, MG_ERR_ALLOCATION_FAILED, "Failed to allocate SHA1 context");
    }

    unsigned int hash_len = 0;
    if (!EVP_DigestInit_ex(ctx, EVP_sha1(), NULL) ||
        !EVP_DigestUpdate(ctx, data, size) ||
        !EVP_DigestFinal_ex(ctx, out_hash, &hash_len)) {
        EVP_MD_CTX_free(ctx);
        return mgSetError(err, MG_ERR_GENERIC, "Failed to compute SHA1 hash");
    }

    EVP_MD_CTX_free(ctx);
    return MG_SUCCESS;
}
