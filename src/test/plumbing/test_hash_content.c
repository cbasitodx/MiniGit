#include <string.h>

#include "test/fixtures.h"
#include "../munit/munit.h"
#include "plumbing/hash-content.h"
#include "utils/errors.h"
#include "utils/read.h"

/* Forward-declare internal helpers that are not in the public header. */
bool writeHeaderToBlob(Blob *blob);
bool hashBlob(Blob *blob, uint8_t *out_hash);

/* ── arg parsing ──────────────────────────────────────────────────────────── */

static MunitResult test_no_args_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "hash-content" };
    HashContentArgs args = {0};

    munit_assert_int(handleHashContentArgsFromCLI(2, argv, &args, &err), !=, MG_SUCCESS);
    munit_assert_int(err.code, ==, MG_ERR_NOT_ENOUGH_ARGS);
    return MUNIT_OK;
}

static MunitResult test_too_many_args_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "hash-content", "--stdin", "--path=hello.txt", "-w", "extra" };
    HashContentArgs args = {0};

    munit_assert_int(handleHashContentArgsFromCLI(6, argv, &args, &err), !=, MG_SUCCESS);
    munit_assert_int(err.code, ==, MG_ERR_TOO_MANY_ARGS);
    return MUNIT_OK;
}

static MunitResult test_stdin_and_path_conflict(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "hash-content", "--stdin", "--path=hello.txt" };
    HashContentArgs args = {0};

    munit_assert_int(handleHashContentArgsFromCLI(4, argv, &args, &err), !=, MG_SUCCESS);
    return MUNIT_OK;
}

static MunitResult test_unknown_option_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "hash-content", "--bogus" };
    HashContentArgs args = {0};

    munit_assert_int(handleHashContentArgsFromCLI(3, argv, &args, &err), !=, MG_SUCCESS);
    munit_assert_int(err.code, ==, MG_ERR_UNKNOWN_OPTION);
    return MUNIT_OK;
}

static MunitResult test_path_arg_is_parsed(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "hash-content", "--path=hello.txt" };
    HashContentArgs args = {0};

    munit_assert_int(handleHashContentArgsFromCLI(3, argv, &args, &err), ==, MG_SUCCESS);
    munit_assert_false(args.use_stdin);
    munit_assert_string_equal(args.file_path, "hello.txt");

    free(args.file_path);
    return MUNIT_OK;
}

static MunitResult test_write_flag_is_parsed(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "hash-content", "--path=hello.txt", "-w" };
    HashContentArgs args = {0};

    munit_assert_int(handleHashContentArgsFromCLI(4, argv, &args, &err), ==, MG_SUCCESS);
    munit_assert_true(args.write);

    free(args.file_path);
    return MUNIT_OK;
}

static MunitResult test_stdin_flag_is_parsed(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "hash-content", "--stdin" };
    HashContentArgs args = {0};

    munit_assert_int(handleHashContentArgsFromCLI(3, argv, &args, &err), ==, MG_SUCCESS);
    munit_assert_true(args.use_stdin);
    return MUNIT_OK;
}

/* ── hashing ──────────────────────────────────────────────────────────────── */

static MunitResult test_hash_file_returns_success(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err = {0};

    HashContentArgs args = { .file_path = strdup("hello.txt") };
    munit_assert_int(hashContent(&args, &err), ==, MG_SUCCESS);
    return MUNIT_OK;
}

static MunitResult test_hash_missing_file_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err = {0};

    HashContentArgs args = { .file_path = strdup("does_not_exist.txt") };
    munit_assert_int(hashContent(&args, &err), !=, MG_SUCCESS);
    munit_assert_int(err.code, ==, MG_ERR_FILE_OPEN_FAILED);
    return MUNIT_OK;
}

static MunitResult test_header_prepended_correctly(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;

    const char *content = "hello";
    Blob blob = {
        .data = (uint8_t *)strdup(content),
        .size = strlen(content)
    };

    munit_assert_true(writeHeaderToBlob(&blob));
    /* blob should now start with "blob 5\0" (7 bytes) */
    munit_assert_memory_equal(7, blob.data, "blob 5\0");
    munit_assert_size(blob.size, ==, 7 + strlen(content));

    free(blob.data);
    return MUNIT_OK;
}

static MunitResult test_same_content_same_hash(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;

    uint8_t hash_a[EVP_SHA1_HASH_LENGTH];
    uint8_t hash_b[EVP_SHA1_HASH_LENGTH];

    Blob blob_a = { .data = (uint8_t *)strdup(FIXTURE_HELLO_CONTENT), .size = strlen(FIXTURE_HELLO_CONTENT) };
    Blob blob_b = { .data = (uint8_t *)strdup(FIXTURE_HELLO_CONTENT), .size = strlen(FIXTURE_HELLO_CONTENT) };

    munit_assert_true(writeHeaderToBlob(&blob_a));
    munit_assert_true(writeHeaderToBlob(&blob_b));
    munit_assert_true(hashBlob(&blob_a, hash_a));
    munit_assert_true(hashBlob(&blob_b, hash_b));

    munit_assert_memory_equal(EVP_SHA1_HASH_LENGTH, hash_a, hash_b);

    free(blob_a.data);
    free(blob_b.data);
    return MUNIT_OK;
}

static MunitResult test_different_content_different_hash(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;

    uint8_t hash_a[EVP_SHA1_HASH_LENGTH];
    uint8_t hash_b[EVP_SHA1_HASH_LENGTH];

    Blob blob_a = { .data = (uint8_t *)strdup(FIXTURE_HELLO_CONTENT), .size = strlen(FIXTURE_HELLO_CONTENT) };
    Blob blob_b = { .data = (uint8_t *)strdup(FIXTURE_WORLD_CONTENT), .size = strlen(FIXTURE_WORLD_CONTENT) };

    munit_assert_true(writeHeaderToBlob(&blob_a));
    munit_assert_true(writeHeaderToBlob(&blob_b));
    munit_assert_true(hashBlob(&blob_a, hash_a));
    munit_assert_true(hashBlob(&blob_b, hash_b));

    munit_assert_memory_not_equal(EVP_SHA1_HASH_LENGTH, hash_a, hash_b);

    free(blob_a.data);
    free(blob_b.data);
    return MUNIT_OK;
}

/* ── exported tests (prefix = "/hash_content") ───────────────────────────── */

MunitTest hash_content_tests[] = {
    { (char *)"/hash_content/no_args_fails",               test_no_args_fails,               fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/too_many_args_fails",         test_too_many_args_fails,         fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/stdin_and_path_conflict",     test_stdin_and_path_conflict,     fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/unknown_option_fails",        test_unknown_option_fails,        fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/path_arg_is_parsed",          test_path_arg_is_parsed,          fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/write_flag_is_parsed",        test_write_flag_is_parsed,        fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/stdin_flag_is_parsed",        test_stdin_flag_is_parsed,        fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/hash_file_returns_success",   test_hash_file_returns_success,   fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/hash_missing_file_fails",     test_hash_missing_file_fails,     fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/header_prepended_correctly",  test_header_prepended_correctly,  fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/same_content_same_hash",      test_same_content_same_hash,      fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/hash_content/different_content_diff_hash", test_different_content_different_hash, fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
