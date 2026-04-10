#include "plumbing/hash-content.h"

#include <stdio.h>
#include <string.h>

#include "utils/errors.h"
#include "utils/read.h"

#define HASH_CONTENT_MIN_ARGS 1
#define HASH_CONTENT_MAX_ARGS 3
#define HASH_CONTENT_COMMAND "hash-content"
#define OPTION_STD_IN "--stdin"
#define OPTION_PATH "--path="
#define OPTION_WRITE "-w"

#define BLOB_READ_MODE "rb"
#define BLOB_WRITE_MODE "wb"
#define BLOB_FILE_DIRECTORY ".minigit/objects"

#define HC_STD_PATH_CONFLICT 100

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
    return false;
}

void printHash(const uint8_t *hash) {
    for (int i = 0; i < EVP_SHA1_HASH_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

int hashContent(HashContentArgs *args, mg_error_t *err) {
    Blob blob = {0};
    FILE *file = NULL;

    if (args->use_stdin) {
        file = stdin;
    } else {
        file = fopen(args->file_path, "rb");
        if (file == NULL) {
            return mgSetError(
                err,
                MG_ERR_FILE_OPEN_FAILED,
                "Failed to open file: %s",
                args->file_path
            );
        }
        free(args->file_path);
    }

    blob = readData(file);
    uint8_t hash[EVP_SHA1_HASH_LENGTH];

    if (!writeHeaderToBlob(&blob) ||
        !hashBlob(&blob, hash)) {
        fclose(file);
        return mgSetError(
            err,
            MG_ERR_ALLOCATION_FAILED,
            "Failed to process blob data"
        );
    }

    if (args->use_stdin) {
        printHash(hash);
    }

    if (args->write) {
        printf("Write functionality is not implemented yet\n");
    }

    free(blob.data);
    fclose(file);

    return MG_SUCCESS;
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
 * @return 0 if successful, non-zero if there's an error.
 */
int handleHashContentArgsFromCLI(int argc, char **args_in, HashContentArgs *args_out, mg_error_t *err) {
    if (argc == 2) {
        return mgSetError(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "%s requires at least %d argument",
            HASH_CONTENT_COMMAND,
            HASH_CONTENT_MIN_ARGS
        );
    }

    if (argc > HASH_CONTENT_MAX_ARGS + 2) {
        return mgSetError(
            err,
            MG_ERR_TOO_MANY_ARGS,
            "%s accepts at most %d arguments",
            HASH_CONTENT_COMMAND,
            HASH_CONTENT_MAX_ARGS
        );
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
                return mgSetError(
                    err,
                    MG_ERR_ALLOCATION_FAILED,
                    "Memory allocation failed for file path"
                );
            }
        }

        else if (strncmp(args_in[i], OPTION_WRITE, strlen(OPTION_WRITE)) == 0) {
            args_out->write = true;
        }

        else {
            free(args_out->file_path);
            return mgSetError(err, MG_ERR_UNKNOWN_OPTION, "Unknown option: %s", args_in[i]);
        }
    }

    if (args_out->use_stdin && args_out->file_path != NULL) {
        free(args_out->file_path);
        return mgSetError(
            err,
            HC_STD_PATH_CONFLICT,
            "%s: cannot use both %s and %s options",
            HASH_CONTENT_COMMAND,
            OPTION_STD_IN,
            OPTION_PATH
        );
    }

    if (!args_out->use_stdin && args_out->file_path == NULL) {
        return mgSetError(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "%s requires either %s or %s option",
            HASH_CONTENT_COMMAND,
            OPTION_STD_IN,
            OPTION_PATH
        );
    }

    return MG_SUCCESS;
}
