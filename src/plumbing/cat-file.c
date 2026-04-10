#include "plumbing/cat-file.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// TODO: This should agree with the one in hash-content.h,
// find an idiomatic solution without importing hash-content.h???
#define SHA1_HEX_LENGTH 40

// TODO: Add more features. For now, we'll work exclusively with these 4 flags
// Adding more features means adding a variable number of arguments
#define CAT_FILE_MAX_ARGS 2

#define OPTION_TYPE "-t"
#define OPTION_SIZE "-s"
#define OPTION_EXISTS "-e"
#define OPTION_PRINT "-p"

bool isValidHash(const char *str) {
    if (strlen(str) != SHA1_HEX_LENGTH) {
        return false;
    }
    for (int i = 0; i < SHA1_HEX_LENGTH; i++) {
        if (!((str[i] >= '0' && str[i] <= '9') ||
              (str[i] >= 'a' && str[i] <= 'f'))) {
            return false;
        }
    }
    return true;
}

bool handleCatFileArgsFromCLI(int argc, char **args_in, CatFileArgs *args_out) {
    if (argc < CAT_FILE_MAX_ARGS + 2) {
        fprintf(stderr, "Not enough arguments\n");
        return false;
    }

    if (argc > CAT_FILE_MAX_ARGS + 2) {
        fprintf(stderr, "Too many arguments\n");
        return false;
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

    return true;
}

void catFile(CatFileArgs *args) {
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
}
