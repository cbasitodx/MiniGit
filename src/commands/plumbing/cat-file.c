#include "minigit/commands/plumbing/cat-file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minigit/core/object.h"
#include "minigit/core/object_store.h"
#include "minigit/core/oid.h"
#include "minigit/core/repo.h"
#include "minigit/core/revision.h"
#include "minigit/utils/errors.h"

#define CAT_FILE_MAX_ARGS 2

#define OPTION_TYPE "-t"
#define OPTION_SIZE "-s"
#define OPTION_EXISTS "-e"
#define OPTION_PRINT "-p"

int handleCatFileArgsFromCLI(int argc, char **args_in, CatFileArgs *args_out, mg_error_t *err) {
    if (argc < CAT_FILE_MAX_ARGS + 2) {
        return mgSetError(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "Not enough args [ERR MSG IN CONSTRUCTION]"
        );
    }

    if (argc > CAT_FILE_MAX_ARGS + 2) {
        return mgSetError(
            err,
            MG_ERR_TOO_MANY_ARGS,
            "Too many arguments [ERR MSG IN CONSTRUCTION]"
        );
    }

    args_out->opt_type = false;
    args_out->opt_size = false;
    args_out->opt_exists = false;
    args_out->opt_print = false;
    args_out->object_name = NULL;

    if (strncmp(args_in[2], OPTION_TYPE, strlen(OPTION_TYPE)) == 0) {
        args_out->opt_type = true;
    } else if (strncmp(args_in[2], OPTION_SIZE, strlen(OPTION_SIZE)) == 0) {
        args_out->opt_size = true;
    } else if (strncmp(args_in[2], OPTION_EXISTS, strlen(OPTION_EXISTS)) == 0) {
        args_out->opt_exists = true;
    } else if (strncmp(args_in[2], OPTION_PRINT, strlen(OPTION_PRINT)) == 0) {
        args_out->opt_print = true;
    }

    args_out->object_name = args_in[3];
    return MG_SUCCESS;
}

int catFile(CatFileArgs *args, mg_error_t *err) {
    mg_repo_t repo = {0};
    int ret = mg_repo_open(&repo, err);
    if (ret != MG_SUCCESS) {
        if (args->opt_exists) {
            printf("Object does not exists\n");
            return MG_SUCCESS;
        }
        return ret;
    }

    mg_oid_t oid = {0};
    ret = mg_revision_resolve(&repo, args->object_name, &oid, err);
    if (ret != MG_SUCCESS) {
        if (args->opt_exists) {
            printf("Object does not exists\n");
            return MG_SUCCESS;
        }
        return ret;
    }

    if (args->opt_type) {
        mg_object_t object = {0};
        ret = mg_object_store_read_header(&repo, &oid, &object, err);
        if (ret != MG_SUCCESS) {
            return ret;
        }

        printf("The type is %s\n", object.type);
    }

    else if (args->opt_size) {
        mg_object_t object = {0};
        ret = mg_object_store_read_header(&repo, &oid, &object, err);
        if (ret != MG_SUCCESS) {
            return ret;
        }

        printf("The size is %zu\n", object.size);
    }

    else if (args->opt_exists) {
        printf("Object exists\n");
    }

    else if (args->opt_print) {
        mg_object_t object = {0};
        ret = mg_object_store_read(&repo, &oid, &object, err);
        if (ret != MG_SUCCESS) {
            return ret;
        }

        fwrite(object.data, 1, object.size, stdout);
        mg_object_free(&object);
    }

    return MG_SUCCESS;
}

int catFileCommand(int argc, char **argv, mg_error_t *err) {
    CatFileArgs args = {0};
    int ret = handleCatFileArgsFromCLI(argc, argv, &args, err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    return catFile(&args, err);
}
