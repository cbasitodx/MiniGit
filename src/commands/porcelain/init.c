#include "minigit/commands/porcelain/init.h"

#include "minigit/core/repo.h"
#include "minigit/utils/errors.h"

int initRepo(mg_error_t *err) {
    int ret = mg_repo_init(err);
    if (ret != MG_SUCCESS) {
        return -1;
    }

    return MG_SUCCESS;
}

int initCommand(int argc, char **argv, mg_error_t *err) {
    (void)argc;
    (void)argv;
    return initRepo(err);
}
