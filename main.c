#include "include/minigit.h"
#include "plumbing/hash-content.h"

#define MINIGIT_NAME "minigit"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command> [<args>]\n", MINIGIT_NAME);
        printf("\nRun '%s --help' for help with the use of minigit\n", MINIGIT_NAME);
        return 0;
    }

    if (strcmp(argv[1], "init") == 0) {
        initRepo();
    }

    else if (strcmp(argv[1], "hash-object") == 0) {
        hashContent(argc, argv);
    }

    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("usage: %s <command>\n\n", MINIGIT_NAME);
        printf("Available commands:\n");
        printf("   init          Create an empty minigit repository\n");
        printf("   hash-object   Compute object ID and optionally creates a blob from a file\n");
    }

    else {
        printf("minigit: '%s' is not a minigit command. See 'minigit --help'.\n", argv[1]);
        return 1;
    }

    return 0;
}
