#include "plumbing/rev-parse.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minigit.h"
#include "plumbing/hash-content.h"
#include "utils/errors.h"

// TODO: Add more features. Adding more features means adding a variable number of arguments
#define REV_PARSE_MAX_ARGS 1


bool isValidHash(const char *str) {
    if (strlen(str) != SHA1_HASH_HEX_LENGTH) {
        return false;
    }
    for (int i = 0; i < SHA1_HASH_HEX_LENGTH; i++) {
        if (!((str[i] >= '0' && str[i] <= '9') ||
              (str[i] >= 'a' && str[i] <= 'f'))) {
            return false;
        }
    }
    return true;
}

int checkIfExistsHashFile(RevParseArgs *args, mg_error_t *err) {
    // Let's first check if the .minigit/ directory exists
    if (opendir(MINIGIT_INIT_DIR) == NULL) {
        return mgSetError(
            err,
            MG_ERR_DIR_OPEN_FAILED,
            "minigit repository was not initialized. Please initialize it with 'minigit init'"
        );
    }

    // Now check if the objects/ directory exists inside .minigit/
    if (opendir(MINIGIT_OBJECTS_PATH) == NULL) {
        return mgSetError(
            err,
            MG_ERR_DIR_OPEN_FAILED,
            "minigit repository does not contains an objects directory."
        );
    }

    // Separate the first two chars of the hash and then the remaining ones.
    // Notice that we know the length (it's SHA1_HASH_HEX_LENGTH)
    // (remember to append the end of string char. That's the reason behind the +1)
    char firstTwoChars[SHA1_DIR_PREFIX_LEN + 1];
    char remainingChars[(SHA1_HASH_HEX_LENGTH - SHA1_DIR_PREFIX_LEN) + 1];

    strncpy(firstTwoChars, args->rev_name, SHA1_DIR_PREFIX_LEN);
    strncpy(remainingChars, (args->rev_name + SHA1_DIR_PREFIX_LEN), SHA1_HASH_HEX_LENGTH - SHA1_DIR_PREFIX_LEN);

    // Now we check if the directory .minigit/objects/{firstTwoChars} exists
    size_t firstTwoCharsDirLen = strlen(MINIGIT_OBJECTS_PATH "/") + SHA1_DIR_PREFIX_LEN + 1;
    char firstTwoCharsDir[firstTwoCharsDirLen];
    sprintf(firstTwoCharsDir, "%s%s", MINIGIT_OBJECTS_PATH "/", firstTwoChars);

    if (opendir(firstTwoCharsDir) == NULL) {
        return mgSetError(
            err,
            MG_ERR_DIR_OPEN_FAILED,
            "This is not an object of minigit objects database"
        );
    }

    // Lastly, check if the object file exists
    size_t objectFilePathLen = firstTwoCharsDirLen + 1 + strlen(remainingChars) + 1;
    char objectFilePath[objectFilePathLen];
    sprintf(objectFilePath, "%s/%s", firstTwoCharsDir, remainingChars);

    FILE *objectFile = fopen(objectFilePath, "rb");
    if (objectFile == NULL) {
        return mgSetError(
            err,
            MG_ERR_DIR_OPEN_FAILED,
            "This is not an object of minigit objects database"
        );
    }

    // Read header up to first \0 into a dynamically growing buffer
    size_t headerCap = HEADER_SIZE, headerLen = 0;
    char *headerBuf = malloc(headerCap);
    int c;
    while ((c = fgetc(objectFile)) != EOF && c != '\0') {
        if (headerLen + 1 >= headerCap) {
            headerBuf = realloc(headerBuf, headerCap *= 2);
        }
        headerBuf[headerLen++] = (char)c;
    }
    headerBuf[headerLen] = '\0';
    args->rev_header = headerBuf;

    // If everything went right, save the file hash and pointer
    args->file_ptr = objectFile;
    args->rev_hash = args->rev_name; // In this case, the name is already the hash

    return MG_SUCCESS;
}

/**
 * Parse command-line arguments for the rev-parse command and store the results
 * in a RevParseArgs struct.
 *
 * @param argc     The number of command-line arguments.
 * @param args_in  The array of command-line arguments.
 * @param args_out The output struct to store the parsed arguments.
 * @param err      Output error struct populated on failure.
 *
 * @return MG_SUCCESS on success, non-zero error code otherwise.
 */
int handleRevParseArgsFromCLI(int argc, char **args_in, RevParseArgs *args_out, mg_error_t *err) {
    if (argc < REV_PARSE_MAX_ARGS + 2) {
        return mgSetError(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "Not enough args [ERR MSG IN CONSTRUCTION]"
        );
    }

    if (argc > REV_PARSE_MAX_ARGS + 2) {
        return mgSetError(
            err,
            MG_ERR_TOO_MANY_ARGS,
            "Too many arguments [ERR MSG IN CONSTRUCTION]"
        );
    }

    args_out->rev_name = args_in[2];
    args_out->file_ptr = NULL;
    args_out->rev_hash = NULL;
    args_out->rev_header = NULL;

    return MG_SUCCESS;
}

/**
 * Resolve a revision name to its corresponding object in the object store.
 * Currently only supports full SHA1 hex hashes. On success, args->file_ptr,
 * args->rev_hash, and args->rev_header are populated.
 * It is the responsibility of the caller to close args->file_ptr and free args->rev_header.
 *
 * @param args Populated RevParseArgs struct with rev_name set.
 * @param err  Output error struct populated on failure.
 *
 * @return MG_SUCCESS on success, non-zero error code otherwise.
 */
int revParse(RevParseArgs *args, mg_error_t *err) {
    if (isValidHash(args->rev_name)) {
        if (checkIfExistsHashFile(args, err) != MG_SUCCESS) {
            return err->code;
        }
    }

    else {
        return mgSetError(
            err,
            MG_ERR_GENERIC,
            "rev-parse does not support symbolic names yet"
        );
    }

    return MG_SUCCESS;
}
