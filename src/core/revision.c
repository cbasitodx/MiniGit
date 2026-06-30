#include "minigit/core/revision.h"

#include <stdbool.h>

#include "minigit/core/object_store.h"

int mg_revision_resolve(const mg_repo_t *repo, const char *name, mg_oid_t *out, mg_error_t *err) {
    int ret = mg_oid_from_hex(name, out, err);
    if (ret != MG_SUCCESS) {
        return mgSetError(err, MG_ERR_GENERIC, "rev-parse does not support symbolic names yet");
    }

    bool exists = false;
    ret = mg_object_store_exists(repo, out, &exists, err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    if (!exists) {
        return mgSetError(err, MG_ERR_DIR_OPEN_FAILED, "This is not an object of minigit objects database");
    }

    return MG_SUCCESS;
}
