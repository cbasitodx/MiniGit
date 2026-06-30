#include "minigit/core/object_store.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minigit/platform/compress.h"
#include "minigit/platform/hash.h"

static int mg_object_path(
    const mg_repo_t *repo,
    const mg_oid_t *oid,
    char *out,
    size_t out_size,
    mg_error_t *err
) {
    int written = snprintf(
        out,
        out_size,
        "%s/%.*s/%s",
        repo->objects_dir,
        MG_OID_DIR_PREFIX_LEN,
        oid->hex,
        oid->hex + MG_OID_DIR_PREFIX_LEN
    );

    if (written < 0 || (size_t)written >= out_size) {
        return mgSetError(err, MG_ERR_GENERIC, "Object path is too long");
    }

    return MG_SUCCESS;
}

static int mg_read_object_header(FILE *file, mg_object_t *out, mg_error_t *err) {
    size_t cap = MG_OBJECT_HEADER_INITIAL_SIZE;
    size_t len = 0;
    char *header = malloc(cap);
    if (header == NULL) {
        return mgSetError(err, MG_ERR_ALLOCATION_FAILED, "Failed to allocate object header");
    }

    int c;
    while ((c = fgetc(file)) != EOF && c != '\0') {
        if (len + 1 >= cap) {
            size_t new_cap = cap * 2;
            char *temp = realloc(header, new_cap);
            if (temp == NULL) {
                free(header);
                return mgSetError(err, MG_ERR_ALLOCATION_FAILED, "Failed to grow object header");
            }
            header = temp;
            cap = new_cap;
        }
        header[len++] = (char)c;
    }
    header[len] = '\0';

    if (sscanf(header, "%15s %zu", out->type, &out->size) != 2) {
        free(header);
        return mgSetError(err, MG_ERR_GENERIC, "Invalid object header");
    }

    free(header);
    return MG_SUCCESS;
}

static int mg_object_payload_from_blob(const uint8_t *data, size_t size, uint8_t **out, size_t *out_size, mg_error_t *err) {
    char header[MG_OBJECT_HEADER_INITIAL_SIZE];
    int header_len = snprintf(header, sizeof(header), "blob %zu", size) + 1;
    if (header_len <= 0 || header_len >= (int)sizeof(header)) {
        return mgSetError(err, MG_ERR_GENERIC, "Blob header is too large");
    }

    *out_size = (size_t)header_len + size;
    *out = malloc(*out_size);
    if (*out == NULL) {
        return mgSetError(err, MG_ERR_ALLOCATION_FAILED, "Failed to allocate blob payload");
    }

    memcpy(*out, header, (size_t)header_len);
    memcpy(*out + header_len, data, size);
    return MG_SUCCESS;
}

int mg_object_hash_blob(const uint8_t *data, size_t size, mg_oid_t *out, mg_error_t *err) {
    uint8_t *payload = NULL;
    size_t payload_size = 0;
    int ret = mg_object_payload_from_blob(data, size, &payload, &payload_size, err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    uint8_t raw_hash[MG_SHA1_RAW_SIZE];
    ret = mg_sha1(payload, payload_size, raw_hash, err);
    free(payload);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    mg_oid_from_raw_sha1(raw_hash, out);
    return MG_SUCCESS;
}

int mg_object_store_exists(const mg_repo_t *repo, const mg_oid_t *oid, bool *exists, mg_error_t *err) {
    char path[PATH_MAX];
    int ret = mg_object_path(repo, oid, path, sizeof(path), err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        *exists = false;
        return MG_SUCCESS;
    }

    fclose(file);
    *exists = true;
    return MG_SUCCESS;
}

int mg_object_store_read_header(const mg_repo_t *repo, const mg_oid_t *oid, mg_object_t *out, mg_error_t *err) {
    memset(out, 0, sizeof(*out));

    char path[PATH_MAX];
    int ret = mg_object_path(repo, oid, path, sizeof(path), err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return mgSetError(err, MG_ERR_DIR_OPEN_FAILED, "This is not an object of minigit objects database");
    }

    ret = mg_read_object_header(file, out, err);
    fclose(file);
    return ret;
}

int mg_object_store_read(const mg_repo_t *repo, const mg_oid_t *oid, mg_object_t *out, mg_error_t *err) {
    memset(out, 0, sizeof(*out));

    char path[PATH_MAX];
    int ret = mg_object_path(repo, oid, path, sizeof(path), err);
    if (ret != MG_SUCCESS) {
        return ret;
    }

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return mgSetError(err, MG_ERR_DIR_OPEN_FAILED, "This is not an object of minigit objects database");
    }

    ret = mg_read_object_header(file, out, err);
    if (ret != MG_SUCCESS) {
        fclose(file);
        return ret;
    }

    long current_pos = ftell(file);
    if (current_pos < 0 || fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return mgSetError(err, MG_ERR_FILE_OPEN_FAILED, "Failed to inspect object file");
    }

    long end_pos = ftell(file);
    if (end_pos < current_pos || fseek(file, current_pos, SEEK_SET) != 0) {
        fclose(file);
        return mgSetError(err, MG_ERR_FILE_OPEN_FAILED, "Failed to inspect object file");
    }

    size_t compressed_size = (size_t)(end_pos - current_pos);
    uint8_t *compressed = malloc(compressed_size);
    out->data = malloc(out->size);
    if (compressed == NULL || out->data == NULL) {
        free(compressed);
        fclose(file);
        mg_object_free(out);
        return mgSetError(err, MG_ERR_ALLOCATION_FAILED, "Failed to allocate object data");
    }

    size_t read_size = fread(compressed, 1, compressed_size, file);
    fclose(file);
    if (read_size != compressed_size) {
        free(compressed);
        mg_object_free(out);
        return mgSetError(err, MG_ERR_FILE_OPEN_FAILED, "Failed to read object data");
    }

    ret = mg_inflate_buffer(compressed, compressed_size, out->data, out->size, err);
    free(compressed);
    if (ret != MG_SUCCESS) {
        mg_object_free(out);
        return ret;
    }

    return MG_SUCCESS;
}
