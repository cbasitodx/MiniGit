#ifndef MINIGIT_CORE_REVISION_H
#define MINIGIT_CORE_REVISION_H

#include "minigit/core/oid.h"
#include "minigit/core/repo.h"
#include "minigit/utils/errors.h"

int mg_revision_resolve(const mg_repo_t *repo, const char *name, mg_oid_t *out, mg_error_t *err);

#endif
