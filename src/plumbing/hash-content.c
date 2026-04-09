#include "plumbing/hash-content.h"

/**
 * Write the git blob header to the beginning of the blob data.
 * If the reallocation fails, it is the responsibility of the caller to free the original blob data.
 *
 * @param blob The blob to write the header to.
 *
 * @return true if the header was successfully written, false otherwise.
 */
bool writeHeaderToBlob(Blob *blob) {
    char header[HEADER_SIZE];
    int header_len = snprintf(header, sizeof(header), "blob %zu", blob->size) + 1;

    size_t new_size = header_len + blob->size;

    uint8_t *temp = (uint8_t *)realloc(blob->data, new_size);
    if (temp == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return false;
    }
    blob->data = temp;

    memmove(blob->data + header_len, blob->data, blob->size);
    memcpy(blob->data, header, header_len);
    blob->size = new_size;

    return true;
}

/**
 * Helper function for hashing blocks of data.
 *
 * @param data The raw bytes to hash.
 * @param len The length of the data in bytes.
 * @param out_hash The output buffer for the hash (must be at least EVP_SHA1_HASH_LENGTH bytes).
 *
 * @return true if the hashing was successful, false otherwise.
 */
bool hashBlob(Blob *blob, uint8_t *out_hash) {
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

    if (!EVP_DigestUpdate(ctx, blob->data, blob->size)) {
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

bool writeHashToFile(Blob *blob, uint8_t *hash) {
}

void printHash(const uint8_t *hash) {
    for (int i = 0; i < EVP_SHA1_HASH_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

void hashContent(HashContentArgs *args) {
    Blob blob = {0};
    FILE *file = NULL;

    if (args->use_stdin) {
        file = stdin;
    } else {
        file = fopen(args->file_path, "rb");
        free(args->file_path);
    }

    blob = readData(file);
    uint8_t hash[EVP_SHA1_HASH_LENGTH];

    if (!writeHeaderToBlob(&blob) ||
        !hashBlob(&blob, hash)) {
        fprintf(stderr, "Failed to hash data\n");
    }

    if (args->use_stdin) {
        printHash(hash);
    }

    if (args->write) {
        printf("Write functionality is not implemented yet\n");
    }

    free(blob.data);
}

/**
 * Handle command-line arguments for the hashContent function
 * and save the results in a hashContentArgs struct.
 * It is the responsibility of the caller to free the file_path
 * field in the struct if it is set.
 *
 * @param argc The number of command-line arguments.
 * @param args_in The array of command-line arguments.
 * @param args_out The output struct to store the parsed arguments.
 *
 * @return true if the arguments were successfully parsed, false otherwise.
 */
bool handleHashContentArgsFromCLI(int argc, char **args_in, HashContentArgs *args_out) {
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
            free(args_out->file_path);
            return false;
        }
    }

    if (args_out->use_stdin && args_out->file_path != NULL) {
        fprintf(stderr, "Cannot use both --stdin and --path options\n");
        free(args_out->file_path);
        return false;
    }

    if (!args_out->use_stdin && args_out->file_path == NULL) {
        fprintf(stderr, "No input source specified\n");
        return false;
    }

    return true;
}
