#include "minigit/commands/plumbing/hash-content.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minigit/core/object_store.h"
#include "minigit/core/oid.h"
#include "minigit/utils/errors.h"
#include "minigit/utils/read.h"

#define HASH_CONTENT_MIN_ARGS 1
#define HASH_CONTENT_MAX_ARGS 3
#define HASH_CONTENT_COMMAND "hash-content"
#define OPTION_STD_IN "--stdin"
#define OPTION_PATH "--path="
#define OPTION_WRITE "-w"

#define BLOB_READ_MODE "rb"

#define HC_STD_PATH_CONFLICT 100

int hashContent(HashContentArgs *args, mg_error_t *err) {
    Blob blob = {0};
    FILE *file = NULL;
    bool should_close_file = false;

    if (args->use_stdin) {
        file = stdin;
    } else {
        file = fopen(args->file_path, BLOB_READ_MODE);
        if (file == NULL) {
            return mgSetError(
                err,
                MG_ERR_FILE_OPEN_FAILED,
                "Failed to open file: %s",
                args->file_path
            );
        }
        free(args->file_path);
        args->file_path = NULL;
        should_close_file = true;
    }

    blob = readData(file);
    mg_oid_t oid = {0};

    int ret = mg_object_hash_blob(blob.data, blob.size, &oid, err);
    if (ret != MG_SUCCESS) {
        free(blob.data);
        if (should_close_file) {
            fclose(file);
        }
        return ret;
    }

    if (args->use_stdin) {
        printf("%s\n", oid.hex);
    }

    if (args->write) {
        printf("Write functionality is not implemented yet\n");
    }

    free(blob.data);
    if (should_close_file) {
        fclose(file);
    }

    return MG_SUCCESS;
}

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

    for (int i = 2; i < argc; i++) {
        if (strncmp(args_in[i], OPTION_STD_IN, strlen(OPTION_STD_IN)) == 0) {
            args_out->use_stdin = true;
        }

        else if (strncmp(args_in[i], OPTION_PATH, strlen(OPTION_PATH)) == 0) {
            int start_index = strlen(OPTION_PATH);

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

int hashContentCommand(int argc, char **argv, mg_error_t *err) {
    HashContentArgs args = {0};
    int ret = handleHashContentArgsFromCLI(argc, argv, &args, err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    return hashContent(&args, err);
}
