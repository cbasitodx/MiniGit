#ifndef MINIGIT_CORE_REPO_H
#define MINIGIT_CORE_REPO_H

#include <limits.h>

#include "minigit/utils/errors.h"

typedef struct {
    char git_dir[PATH_MAX];
    char objects_dir[PATH_MAX];
} mg_repo_t;

int mg_repo_open(mg_repo_t *repo, mg_error_t *err);
int mg_repo_init(mg_error_t *err);

#endif
