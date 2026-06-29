#include <stdio.h>
#include <string.h>

#include "test/fixtures.h"
#include "../munit/munit.h"
#include "plumbing/cat-file.h"
#include "porcelain/init.h"
#include "utils/errors.h"

/* ── stdout capture wrappers ──────────────────────────────────────────────── */

typedef struct {
    CatFileArgs *args;
    mg_error_t  *err;
} CatFileCall;

static void run_cat_file(void *arg) {
    CatFileCall *c = arg;
    catFile(c->args, c->err);
}

/* ── arg parsing ──────────────────────────────────────────────────────────── */

static MunitResult test_too_few_args_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "cat-file", "-t" };
    CatFileArgs args = {0};

    munit_assert_int(handleCatFileArgsFromCLI(3, argv, &args, &err), !=, MG_SUCCESS);
    munit_assert_int(err.code, ==, MG_ERR_NOT_ENOUGH_ARGS);
    return MUNIT_OK;
}

static MunitResult test_too_many_args_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "cat-file", "-t", "aabbcc", "extra" };
    CatFileArgs args = {0};

    munit_assert_int(handleCatFileArgsFromCLI(5, argv, &args, &err), !=, MG_SUCCESS);
    munit_assert_int(err.code, ==, MG_ERR_TOO_MANY_ARGS);
    return MUNIT_OK;
}

static MunitResult test_type_flag_is_parsed(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();
    mg_error_t err  = {0};
    char *sha1      = fixture_create_blob("hello.txt");
    char *argv[]    = { "minigit", "cat-file", "-t", sha1 };
    CatFileArgs args = {0};

    munit_assert_int(handleCatFileArgsFromCLI(4, argv, &args, &err), ==, MG_SUCCESS);
    munit_assert_true(args.opt_type);
    munit_assert_false(args.opt_size);
    munit_assert_false(args.opt_exists);
    munit_assert_false(args.opt_print);

    if (args.object_file) fclose(args.object_file);
    free(args.object_header);
    free(sha1);
    return MUNIT_OK;
}

static MunitResult test_size_flag_is_parsed(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();
    mg_error_t err  = {0};
    char *sha1      = fixture_create_blob("hello.txt");
    char *argv[]    = { "minigit", "cat-file", "-s", sha1 };
    CatFileArgs args = {0};

    munit_assert_int(handleCatFileArgsFromCLI(4, argv, &args, &err), ==, MG_SUCCESS);
    munit_assert_true(args.opt_size);

    if (args.object_file) fclose(args.object_file);
    free(args.object_header);
    free(sha1);
    return MUNIT_OK;
}

/* ── cat-file output ──────────────────────────────────────────────────────── */

static MunitResult test_type_prints_blob(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();

    char *sha1   = fixture_create_blob("hello.txt");
    char *argv[] = { "minigit", "cat-file", "-t", sha1 };
    mg_error_t err   = {0};
    CatFileArgs args  = {0};
    handleCatFileArgsFromCLI(4, argv, &args, &err);

    CatFileCall call = { &args, &err };
    char *out = capture_stdout(run_cat_file, &call);

    munit_assert_not_null(strstr(out, "blob"));

    free(out);
    free(sha1);
    return MUNIT_OK;
}

static MunitResult test_size_prints_correct_size(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();

    /* hello.txt contains "hello\n" — 6 bytes */
    char *sha1   = fixture_create_blob("hello.txt");
    char *argv[] = { "minigit", "cat-file", "-s", sha1 };
    mg_error_t err   = {0};
    CatFileArgs args  = {0};
    handleCatFileArgsFromCLI(4, argv, &args, &err);

    CatFileCall call = { &args, &err };
    char *out = capture_stdout(run_cat_file, &call);

    munit_assert_not_null(strstr(out, "6"));

    free(out);
    free(sha1);
    return MUNIT_OK;
}

static MunitResult test_exists_returns_true_for_existing_object(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();

    char *sha1   = fixture_create_blob("hello.txt");
    char *argv[] = { "minigit", "cat-file", "-e", sha1 };
    mg_error_t err   = {0};
    CatFileArgs args  = {0};
    handleCatFileArgsFromCLI(4, argv, &args, &err);

    CatFileCall call = { &args, &err };
    char *out = capture_stdout(run_cat_file, &call);

    munit_assert_not_null(strstr(out, "exists"));

    free(out);
    free(sha1);
    return MUNIT_OK;
}

static MunitResult test_exists_returns_false_for_missing_object(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();

    char hash[]  = "aabbccddeeff00112233445566778899aabbccdd";
    char *argv[] = { "minigit", "cat-file", "-e", hash };
    mg_error_t err   = {0};
    CatFileArgs args  = {0};
    handleCatFileArgsFromCLI(4, argv, &args, &err);

    CatFileCall call = { &args, &err };
    char *out = capture_stdout(run_cat_file, &call);

    munit_assert_not_null(strstr(out, "does not"));

    free(out);
    return MUNIT_OK;
}

static MunitResult test_print_outputs_file_content(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();

    char *sha1   = fixture_create_blob("hello.txt");
    char *argv[] = { "minigit", "cat-file", "-p", sha1 };
    mg_error_t err   = {0};
    CatFileArgs args  = {0};
    handleCatFileArgsFromCLI(4, argv, &args, &err);

    CatFileCall call = { &args, &err };
    char *out = capture_stdout(run_cat_file, &call);

    munit_assert_not_null(strstr(out, "hello"));

    free(out);
    free(sha1);
    return MUNIT_OK;
}

/* ── exported tests (prefix = "/cat_file") ───────────────────────────────── */

MunitTest cat_file_tests[] = {
    { (char *)"/cat_file/too_few_args_fails",                 test_too_few_args_fails,                      fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/cat_file/too_many_args_fails",                test_too_many_args_fails,                     fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/cat_file/type_flag_is_parsed",                test_type_flag_is_parsed,                     fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/cat_file/size_flag_is_parsed",                test_size_flag_is_parsed,                     fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/cat_file/type_prints_blob",                   test_type_prints_blob,                        fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/cat_file/size_prints_correct_size",           test_size_prints_correct_size,                fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/cat_file/exists_true_for_existing_object",    test_exists_returns_true_for_existing_object, fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/cat_file/exists_false_for_missing_object",    test_exists_returns_false_for_missing_object, fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/cat_file/print_outputs_file_content",         test_print_outputs_file_content,              fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
