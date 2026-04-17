#include <stdio.h>
#include <string.h>

#include "plumbing/cat-file.h"
#include "plumbing/hash-content.h"
#include "plumbing/rev-parse.h"
#include "porcelain/init.h"
#include "utils/errors.h"

#define MINIGIT_NAME "minigit"

int main(int argc, char *argv[]) {
    mg_error_t error = {0}; // TODO: TEMPORAL PLACEHOLDER

    if (argc < 2) {
        printf("Usage: %s <command> [<args>]\n", MINIGIT_NAME);
        printf("\nRun '%s --help' for help with the use of minigit\n", MINIGIT_NAME);
        return 0;
    }

    if (strcmp(argv[1], "init") == 0) {
        initRepo();
    }

    else if (strcmp(argv[1], HASH_CONTENT_COMMAND) == 0) {
        HashContentArgs hashContentArgs = {0};
        if (handleHashContentArgsFromCLI(argc, argv, &hashContentArgs, &error) != 0) {
            return 0;
        }

        hashContent(&hashContentArgs, &error);
    }

    else if (strcmp(argv[1], "cat-file") == 0) {
        CatFileArgs catFileArgs = {0};
        if (handleCatFileArgsFromCLI(argc, argv, &catFileArgs, &error) != 0) {
            return 0;
        }

        catFile(&catFileArgs, &error);
    }

    else if (strcmp(argv[1], "rev-parse") == 0) {
        RevParseArgs revParseArgs = {0};
        if (handleRevParseArgsFromCLI(argc, argv, &revParseArgs, &error) != 0) {
            return 0;
        }

        revParse(&revParseArgs, &error);
        // If everything went well, echo the hash
        // TODO: this could be multiple hashes. In the future we must acknowledge this...
        printf("%s\n", revParseArgs.rev_hash);

        // Clean up shit
        if (revParseArgs.file_ptr != NULL) {
            fclose(revParseArgs.file_ptr);
        }
        if (revParseArgs.rev_header != NULL) {
            free(revParseArgs.rev_header);
        }
    }

    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("usage: %s <command>\n\n", MINIGIT_NAME);
        printf("Available commands:\n");
        printf("   init          Create an empty minigit repository\n");
        printf("   hash-object   Compute object ID and optionally creates a blob from a file\n");
        printf("   cat-file      something something\n");
        printf("   rev-parse     something something\n");
    }

    else {
        printf("minigit: '%s' is not a minigit command. See 'minigit --help'.\n", argv[1]);
        return 1;
    }

    return 0;
}
