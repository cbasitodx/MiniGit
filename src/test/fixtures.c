#include "test/fixtures.h"

#include <openssl/evp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

#include "munit/munit.h"
#include "plumbing/hash-content.h"

/* ── helpers ──────────────────────────────────────────────────────────────── */

static void write_file(const char *path, const char *content) {
    FILE *f = fopen(path, "w");
    munit_assert_not_null(f);
    fputs(content, f);
    fclose(f);
}

/* ── fixture setup / teardown ─────────────────────────────────────────────── */

/*
 * Populate the current working directory with a basic file tree.
 * Add new fixtures here as new commands need them.
 */
static void populate(void) {
    write_file("hello.txt",  FIXTURE_HELLO_CONTENT);
    write_file("world.txt",  FIXTURE_WORLD_CONTENT);
    write_file("empty.txt",  "");

    mkdir("subdir", 0777);
    write_file("subdir/nested.txt", FIXTURE_NESTED_CONTENT);
}

void *fixture_setup(const MunitParameter params[], void *user_data) {
    (void)params;
    (void)user_data;

    TestFixture *fix = malloc(sizeof(TestFixture));
    munit_assert_not_null(fix);

    munit_assert_not_null(getcwd(fix->original_dir, sizeof(fix->original_dir)));

    char tmpl[] = "/tmp/minigit_tmp.XXXXXX";
    munit_assert_not_null(mkdtemp(tmpl));
    strncpy(fix->test_dir, tmpl, sizeof(fix->test_dir) - 1);
    fix->test_dir[sizeof(fix->test_dir) - 1] = '\0';

    munit_assert_int(chdir(fix->test_dir), ==, 0);
    populate();

    return fix;
}

void fixture_teardown(void *fixture) {
    TestFixture *fix = fixture;
    chdir(fix->original_dir);

    char cmd[PATH_MAX + 8];
    snprintf(cmd, sizeof(cmd), "rm -rf %s", fix->test_dir);
    system(cmd);

    free(fix);
}

/* ── blob object creation ─────────────────────────────────────────────────── */

char *fixture_create_blob(const char *filepath) {
    /* Read file content */
    FILE *f = fopen(filepath, "rb");
    munit_assert_not_null(f);

    fseek(f, 0, SEEK_END);
    long content_len = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *content = malloc(content_len);
    munit_assert_not_null(content);
    fread(content, 1, content_len, f);
    fclose(f);

    /* Build "blob <size>\0<content>" */
    char header[64];
    int header_len = snprintf(header, sizeof(header), "blob %ld", content_len) + 1;
    size_t total   = header_len + content_len;
    uint8_t *data  = malloc(total);
    munit_assert_not_null(data);
    memcpy(data, header, header_len);
    memcpy(data + header_len, content, content_len);

    /* SHA1 over the full uncompressed data */
    uint8_t raw_hash[EVP_SHA1_HASH_LENGTH];
    unsigned int hash_len = 0;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    munit_assert_not_null(ctx);
    EVP_DigestInit_ex(ctx, EVP_sha1(), NULL);
    EVP_DigestUpdate(ctx, data, total);
    EVP_DigestFinal_ex(ctx, raw_hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    /* Hex-encode */
    char *hex = malloc(SHA1_HASH_HEX_LENGTH + 1);
    munit_assert_not_null(hex);
    for (int i = 0; i < EVP_SHA1_HASH_LENGTH; i++) {
        sprintf(hex + i * 2, "%02x", raw_hash[i]);
    }
    hex[SHA1_HASH_HEX_LENGTH] = '\0';

    /* Compress the content part only (matches cat-file's expected layout) */
    uLongf compressed_bound = compressBound(content_len);
    uint8_t *compressed = malloc(compressed_bound);
    munit_assert_not_null(compressed);
    compress(compressed, &compressed_bound, content, content_len);

    /* Write to .minigit/objects/<xx>/<remaining> */
    char dir_path[PATH_MAX];
    snprintf(dir_path, sizeof(dir_path), ".minigit/objects/%.2s", hex);
    mkdir(dir_path, 0777);

    char obj_path[PATH_MAX];
    snprintf(obj_path, sizeof(obj_path), "%s/%s", dir_path, hex + 2);

    FILE *obj = fopen(obj_path, "wb");
    munit_assert_not_null(obj);
    /* File layout: uncompressed header + compressed content */
    fwrite(header, 1, header_len, obj);
    fwrite(compressed, 1, compressed_bound, obj);
    fclose(obj);

    free(content);
    free(data);
    free(compressed);
    return hex;
}

/* ── stdout capture ───────────────────────────────────────────────────────── */

char *capture_stdout(CaptureFunc fn, void *arg) {
    char tmp[] = "/tmp/minigit_cap.XXXXXX";
    int fd = mkstemp(tmp);
    munit_assert_int(fd, >=, 0);

    fflush(stdout);
    int saved_fd = dup(STDOUT_FILENO);
    dup2(fd, STDOUT_FILENO);
    close(fd);

    fn(arg);
    fflush(stdout);

    dup2(saved_fd, STDOUT_FILENO);
    close(saved_fd);

    FILE *f = fopen(tmp, "r");
    char buf[4096] = {0};
    if (f) {
        fread(buf, 1, sizeof(buf) - 1, f);
        fclose(f);
    }
    unlink(tmp);

    return strdup(buf);
}
