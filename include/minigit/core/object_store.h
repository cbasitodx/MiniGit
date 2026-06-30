#ifndef MINIGIT_CORE_OBJECT_STORE_H
#define MINIGIT_CORE_OBJECT_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "minigit/core/object.h"
#include "minigit/core/oid.h"
#include "minigit/core/repo.h"
#include "minigit/utils/errors.h"

int mg_object_hash_blob(const uint8_t *data, size_t size, mg_oid_t *out, mg_error_t *err);
int mg_object_store_exists(const mg_repo_t *repo, const mg_oid_t *oid, bool *exists, mg_error_t *err);
int mg_object_store_read_header(const mg_repo_t *repo, const mg_oid_t *oid, mg_object_t *out, mg_error_t *err);
int mg_object_store_read(const mg_repo_t *repo, const mg_oid_t *oid, mg_object_t *out, mg_error_t *err);

#endif
