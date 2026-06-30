#include "fixtures.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

#include "munit/munit.h"
#include "minigit/core/object_store.h"
#include "minigit/core/oid.h"

static void write_file(const char *path, const char *content) {
    FILE *f = fopen(path, "w");
    munit_assert_not_null(f);
    fputs(content, f);
    fclose(f);
}

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

char *fixture_create_blob(const char *filepath) {
    FILE *f = fopen(filepath, "rb");
    munit_assert_not_null(f);

    fseek(f, 0, SEEK_END);
    long content_len = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *content = malloc(content_len);
    munit_assert_not_null(content);
    fread(content, 1, content_len, f);
    fclose(f);

    char header[64];
    int header_len = snprintf(header, sizeof(header), "blob %ld", content_len) + 1;

    mg_error_t err = {0};
    mg_oid_t oid = {0};
    munit_assert_int(mg_object_hash_blob(content, content_len, &oid, &err), ==, MG_SUCCESS);

    char *hex = malloc(MG_OID_HEX_BUFFER_SIZE);
    munit_assert_not_null(hex);
    memcpy(hex, oid.hex, MG_OID_HEX_BUFFER_SIZE);

    uLongf compressed_bound = compressBound(content_len);
    uint8_t *compressed = malloc(compressed_bound);
    munit_assert_not_null(compressed);
    compress(compressed, &compressed_bound, content, content_len);

    char dir_path[PATH_MAX];
    snprintf(dir_path, sizeof(dir_path), ".minigit/objects/%.2s", hex);
    mkdir(dir_path, 0777);

    char obj_path[PATH_MAX];
    snprintf(obj_path, sizeof(obj_path), "%s/%s", dir_path, hex + 2);

    FILE *obj = fopen(obj_path, "wb");
    munit_assert_not_null(obj);
    fwrite(header, 1, header_len, obj);
    fwrite(compressed, 1, compressed_bound, obj);
    fclose(obj);

    free(content);
    free(compressed);
    return hex;
}

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
