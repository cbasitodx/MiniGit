#include "minigit/cli/dispatch.h"

#include <stdio.h>
#include <string.h>

#include "minigit/commands/plumbing/cat-file.h"
#include "minigit/commands/plumbing/hash-content.h"
#include "minigit/commands/plumbing/rev-parse.h"
#include "minigit/commands/porcelain/init.h"
#include "minigit/utils/errors.h"

#define MINIGIT_NAME "minigit"

typedef int (*mg_cli_handler_t)(int argc, char **argv, mg_error_t *err);

typedef struct {
    const char *name;
    const char *description;
    mg_cli_handler_t handler;
} mg_cli_command_t;

static const mg_cli_command_t commands[] = {
    {"init", "Create an empty minigit repository", initCommand},
    {"hash-content", "Compute object ID and optionally creates a blob from a file", hashContentCommand},
    {"cat-file", "Inspect objects in the object database", catFileCommand},
    {"rev-parse", "Resolve a revision name to an object ID", revParseCommand},
};

static void print_usage(void) {
    printf("Usage: %s <command> [<args>]\n", MINIGIT_NAME);
    printf("\nRun '%s --help' for help with the use of minigit\n", MINIGIT_NAME);
}

static void print_help(void) {
    size_t command_count = sizeof(commands) / sizeof(commands[0]);

    printf("usage: %s <command>\n\n", MINIGIT_NAME);
    printf("Available commands:\n");
    for (size_t i = 0; i < command_count; i++) {
        printf("   %-12s %s\n", commands[i].name, commands[i].description);
    }
}

int mg_cli_dispatch(int argc, char **argv, mg_error_t *err) {
    if (argc < 2) {
        print_usage();
        return MG_SUCCESS;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_help();
        return MG_SUCCESS;
    }

    size_t command_count = sizeof(commands) / sizeof(commands[0]);
    for (size_t i = 0; i < command_count; i++) {
        if (strcmp(argv[1], commands[i].name) == 0) {
            return commands[i].handler(argc, argv, err);
        }
    }

    printf("minigit: '%s' is not a minigit command. See 'minigit --help'.\n", argv[1]);
    return MG_ERR_GENERIC;
}
