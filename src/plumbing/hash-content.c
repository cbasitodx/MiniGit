#include "plumbing/hash-content.h"

/**
 * Helper function for hashing blocks of data.
 *
 * @param data The raw bytes to hash.
 * @param len The length of the data in bytes.
 * @param md The output buffer for the hash (must be at least SHA_DIGEST_LENGTH bytes).
 */
void hashBlob(const unsigned char *data, size_t len, unsigned char *md) {
    // Prepare the header
    char header[HEADER_SIZE];
    int header_len = snprintf(header, sizeof(header), "blob %lld", len) + 1; // Account for \0

    // Prepare the hash
    SHA_CTX context;
    SHA1_Init(&context);

    // Hash the header
    SHA1_Update(&context, header, header_len);

    // Hash the file content
    SHA1_Update(&context, data, len);

    SHA1_Final(md, &context);
}

void hashContent(bool write) {
}
