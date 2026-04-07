#include "plumbing/hash-content.h"
#include <stdint.h>

/**
 * Helper function for hashing blocks of data.
 *
 * @param data The raw bytes to hash.
 * @param len The length of the data in bytes.
 * @param out_hash The output buffer for the hash (must be at least EVP_SHA1_HASH_LENGTH bytes).
 */
bool hashBlob(const uint8_t *data, size_t len, uint8_t *out_hash) {
    EVP_MD_CTX *ctx = NULL;
    const EVP_MD *md = NULL;
    unsigned int hash_len = 0;

    // Create a message digest context
    ctx = EVP_MD_CTX_new();
    if (!ctx) {
        return false;
    }

    // Fetch the SHA1 algorithm
    md = EVP_sha1();

    // Initialize the digest operation
    if (!EVP_DigestInit_ex(ctx, md, NULL)) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // Prepare and hash the Git header: "blob <size>\0"
    char header[HEADER_SIZE];
    int header_len = snprintf(header, sizeof(header), "blob %zu", len) + 1;

    if (!EVP_DigestUpdate(ctx, header, header_len) ||
        !EVP_DigestUpdate(ctx, data, len)) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // Finalize the hash
    if (!EVP_DigestFinal_ex(ctx, out_hash, &hash_len)) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // Clean up memory
    EVP_MD_CTX_free(ctx);
    return true;
}

/**
 * Handle command-line arguments for the hashContent function
 * and save the results in a hashContentArgs struct.
 *
 * @param argc The number of command-line arguments.
 * @param args_in The array of command-line arguments.
 * @param args_out The output struct to store the parsed arguments.
 *
 * @return true if the arguments were successfully parsed, false otherwise.
 */
bool handleArgs(int argc, char **args_in, hashContentArgs *args_out) {
    if (argc == 2) {
        fprintf(stderr, "Not enough arguments\n");
        return false;
    }

    if (argc > MAX_ARGS + 2) {
        fprintf(stderr, "Too many arguments\n");
        return false;
    }

    // Start at 2 to skip the command name and subcommand
    for (int i = 2; i < argc; i++) {
        if (strncmp(args_in[i], OPTION_STD_IN, strlen(OPTION_STD_IN)) == 0) {
            args_out->use_stdin = true;
        }

        else if (strncmp(args_in[i], OPTION_PATH, strlen(OPTION_PATH)) == 0) {
            int start_index = strlen(OPTION_PATH); // Skip "path="

            args_out->file_path = strdup(args_in[i] + start_index);
            if (args_out->file_path == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                return false;
            }
        }

        else if (strncmp(args_in[i], OPTION_WRITE, strlen(OPTION_WRITE)) == 0) {
            args_out->write = true;
        }

        else {
            fprintf(stderr, "Unknown option: %s\n", args_in[i]);
            return false;
        }
    }

    if (args_out->use_stdin && args_out->file_path != NULL) {
        fprintf(stderr, "Cannot use both --stdin and --path options\n");
        free(args_out->file_path);
        return false;
    }

    return true;
}

bool hashFromStdin(bool write) {
    Blob blob = readData(stdin);
    uint8_t hash[EVP_SHA1_HASH_LENGTH];

    if (!hashBlob(blob.data, blob.size, hash)) {
        fprintf(stderr, "Failed to hash data from stdin\n");
        free(blob.data);
        return false;
    }

    for (int i = 0; i < EVP_SHA1_HASH_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    if (write) {
        printf("Write functionality is not implemented yet\n");
    }

    return true;
}

void hashContent(int argc, char **args) {
    hashContentArgs args_struct;

    if (!handleArgs(argc, args, &args_struct)) {
        fprintf(stderr, "Failed to parse arguments\n");
        return;
    }

    if (args_struct.use_stdin) {
        hashFromStdin(args_struct.write);
    }
}
