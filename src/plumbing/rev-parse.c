#include "plumbing/rev-parse.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plumbing/hash-content.h"
#include "utils/errors.h"

// TODO: Add more features. Adding more features means adding a variable number of arguments
#define REV_PARSE_MAX_ARGS 1

#define SHA1_HASH_HEX_LENGTH EVP_SHA1_HASH_LENGTH * 2

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
    if (opendir(".minigit") == NULL) {
        return mgSetError(
            err,
            MG_ERR_DIR_OPEN_FAILED,
            "minigit repository was not initialized. Please initialize it with 'minigit init'"
        );
    }

    // Now check if the objects/ directory exists inside .minigit/
    if (opendir(".minigit/objects") == NULL) {
        return mgSetError(
            err,
            MG_ERR_DIR_OPEN_FAILED,
            "minigit repository does not contains an objects directory."
        );
    }

    // Separate the first two chars of the hash and then the remaining ones.
    // Notice that we know the length (it's SHA1_HASH_HEX_LENGTH)
    // (remember to append the end of string char. That's the reason behind the +1)
    char firstTwoChars[2 + 1];
    char remainingChars[(SHA1_HASH_HEX_LENGTH - 2) + 1];

    strncpy(firstTwoChars, args->rev_name, 2);
    strncpy(remainingChars, (args->rev_name + 2), SHA1_HASH_HEX_LENGTH - 2);

    // Now we check if the directory .minigit/objects/{firstTwoChars} exists
    size_t firstTwoCharsDirLen = strlen(".minigit/objects/") + 2 + 1;
    char firstTwoCharsDir[firstTwoCharsDirLen];
    sprintf(firstTwoCharsDir, "%s%s", ".minigit/objects/", firstTwoChars);

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
    size_t headerCap = 64, headerLen = 0;
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
            MG_ERR_NOT_ENOUGH_ARGS,
            "Too many arguments [ERR MSG IN CONSTRUCTION]"
        );
    }

    args_out->rev_name = args_in[2];
    args_out->file_ptr = NULL;
    args_out->rev_hash = NULL;
    args_out->rev_header = NULL;

    return MG_SUCCESS;
}

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
