#include "plumbing/hash-content.h"

/**
 * Helper function for hashing blocks of data.
 *
 * @param data The raw bytes to hash.
 * @param len The length of the data in bytes.
 * @param out_hash The output buffer for the hash (must be at least SHA_DIGEST_LENGTH bytes).
 */
bool hashBlob(const unsigned char *data, size_t len, unsigned char *out_hash) {
    EVP_MD_CTX *ctx = NULL;
    const EVP_MD *md = NULL;
    unsigned int hash_len = 0;

    // 1. Create a message digest context
    ctx = EVP_MD_CTX_new();
    if (!ctx) {
        return false;
    }

    // 2. Fetch the SHA1 algorithm
    md = EVP_sha1();

    // 3. Initialize the digest operation
    if (!EVP_DigestInit_ex(ctx, md, NULL)) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // 4. Prepare and hash the Git header: "blob <size>\0"
    char header[64];
    int header_len = snprintf(header, sizeof(header), "blob %zu", len) + 1;

    if (!EVP_DigestUpdate(ctx, header, header_len) ||
        !EVP_DigestUpdate(ctx, data, len)) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // 5. Finalize the hash
    if (!EVP_DigestFinal_ex(ctx, out_hash, &hash_len)) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // 6. Clean up memory
    EVP_MD_CTX_free(ctx);
    return true;
}

bool handleArgs(int argc, char ***args_in, char ***args_out) {
    if (argc == 1) {
        fprintf(stderr, "Not enough arguments\n");
        *args_out = NULL;
        return false;
    }

    if (argc == 2) {
        if (strcmp(args_in[1], OPTION_STD_IN) == 0) {
        }
    }
}

void hashContent(int argc, char ***args) {
    char **args_processed = NULL;

    handleArgs(argc, args, &args_processed);
}
