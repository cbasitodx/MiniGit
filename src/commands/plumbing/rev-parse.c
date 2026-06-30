#include "minigit/commands/plumbing/rev-parse.h"

#include <stdio.h>

#include "minigit/core/oid.h"
#include "minigit/core/repo.h"
#include "minigit/core/revision.h"
#include "minigit/utils/errors.h"

#define REV_PARSE_MAX_ARGS 1

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

    return MG_SUCCESS;
}

int revParse(RevParseArgs *args, mg_error_t *err) {
    mg_repo_t repo = {0};
    int ret = mg_repo_open(&repo, err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    mg_oid_t oid = {0};
    ret = mg_revision_resolve(&repo, args->rev_name, &oid, err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    printf("%s\n", oid.hex);
    return MG_SUCCESS;
}

int revParseCommand(int argc, char **argv, mg_error_t *err) {
    RevParseArgs args = {0};
    int ret = handleRevParseArgsFromCLI(argc, argv, &args, err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    return revParse(&args, err);
}
