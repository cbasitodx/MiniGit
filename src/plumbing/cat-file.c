#include "plumbing/cat-file.h"

#include "utils/errors.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// TODO: Add more features. For now, we'll work exclusively with these 4 flags
// Adding more features means adding a variable number of arguments
#define CAT_FILE_MAX_ARGS 2

#define OPTION_TYPE "-t"
#define OPTION_SIZE "-s"
#define OPTION_EXISTS "-e"
#define OPTION_PRINT "-p"

int handleCatFileArgsFromCLI(int argc, char **args_in, CatFileArgs *args_out, mg_error_t *err) {
    if (argc < CAT_FILE_MAX_ARGS + 2) {
        return mg_set_error(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "Not enough args [ERR MSG IN CONSTRUCTION]"
        );
    }

    if (argc > CAT_FILE_MAX_ARGS + 2) {
        return mg_set_error(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "Too many arguments [ERR MSG IN CONSTRUCTION]"
        );
    }

    args_out->opt_type = false;
    args_out->opt_size = false;
    args_out->opt_exists = false;
    args_out->opt_print = false;
    args_out->object = NULL;

    // Start at 2 to skip the command name and subcommand
    if (strncmp(args_in[2], OPTION_TYPE, strlen(OPTION_TYPE)) == 0) {
        args_out->opt_type = true;
    } else if (strncmp(args_in[2], OPTION_SIZE, strlen(OPTION_SIZE)) == 0) {
        args_out->opt_size = true;
    } else if (strncmp(args_in[2], OPTION_EXISTS, strlen(OPTION_EXISTS)) == 0) {
        args_out->opt_exists = true;
    } else if (strncmp(args_in[2], OPTION_PRINT, strlen(OPTION_PRINT)) == 0) {
        args_out->opt_print = true;
    }

    args_out->object = args_in[3];

    return MG_SUCCESS;
}

int catFile(CatFileArgs *args, mg_error_t *err) {
    if (args->opt_type) {
        printf("ttt\n");
    }

    else if (args->opt_size) {
        printf("sss\n");
    }

    else if (args->opt_exists) {
        printf("eee\n");
    }

    else if (args->opt_print) {
        printf("ppp\n");
    }

    return MG_SUCCESS;
}
