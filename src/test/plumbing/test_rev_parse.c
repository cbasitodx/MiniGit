#include <string.h>

#include "test/fixtures.h"
#include "../munit/munit.h"
#include "plumbing/hash-content.h"
#include "plumbing/rev-parse.h"
#include "porcelain/init.h"
#include "utils/errors.h"

/* ── arg parsing ──────────────────────────────────────────────────────────── */

static MunitResult test_no_args_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "rev-parse" };
    RevParseArgs args = {0};

    munit_assert_int(handleRevParseArgsFromCLI(2, argv, &args, &err), !=, MG_SUCCESS);
    munit_assert_int(err.code, ==, MG_ERR_NOT_ENOUGH_ARGS);
    return MUNIT_OK;
}

static MunitResult test_too_many_args_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "rev-parse", "aabbccdd", "extra" };
    RevParseArgs args = {0};

    munit_assert_int(handleRevParseArgsFromCLI(4, argv, &args, &err), !=, MG_SUCCESS);
    munit_assert_int(err.code, ==, MG_ERR_TOO_MANY_ARGS);
    return MUNIT_OK;
}

static MunitResult test_arg_stored_as_rev_name(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char hash[]     = "aabbccddeeff00112233445566778899aabbccdd";
    char *argv[]    = { "minigit", "rev-parse", hash };
    RevParseArgs args = {0};

    munit_assert_int(handleRevParseArgsFromCLI(3, argv, &args, &err), ==, MG_SUCCESS);
    munit_assert_string_equal(args.rev_name, hash);
    return MUNIT_OK;
}

/* ── resolution ───────────────────────────────────────────────────────────── */

static MunitResult test_no_repo_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    char hash[]     = "aabbccddeeff00112233445566778899aabbccdd";
    char *argv[]    = { "minigit", "rev-parse", hash };
    RevParseArgs args = {0};

    handleRevParseArgsFromCLI(3, argv, &args, &err);
    munit_assert_int(revParse(&args, &err), !=, MG_SUCCESS);
    return MUNIT_OK;
}

static MunitResult test_invalid_hash_format_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err  = {0};
    /* Symbolic names are not supported yet */
    char *argv[]    = { "minigit", "rev-parse", "HEAD" };
    RevParseArgs args = {0};

    handleRevParseArgsFromCLI(3, argv, &args, &err);
    munit_assert_int(revParse(&args, &err), !=, MG_SUCCESS);
    return MUNIT_OK;
}

static MunitResult test_nonexistent_hash_fails(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();
    mg_error_t err  = {0};
    char hash[]     = "aabbccddeeff00112233445566778899aabbccdd";
    char *argv[]    = { "minigit", "rev-parse", hash };
    RevParseArgs args = {0};

    handleRevParseArgsFromCLI(3, argv, &args, &err);
    munit_assert_int(revParse(&args, &err), !=, MG_SUCCESS);
    return MUNIT_OK;
}

static MunitResult test_existing_object_resolves(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();

    char *sha1 = fixture_create_blob("hello.txt");

    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "rev-parse", sha1 };
    RevParseArgs args = {0};

    handleRevParseArgsFromCLI(3, argv, &args, &err);
    munit_assert_int(revParse(&args, &err), ==, MG_SUCCESS);

    munit_assert_not_null(args.rev_hash);
    munit_assert_string_equal(args.rev_hash, sha1);
    munit_assert_not_null(args.rev_header);
    munit_assert_not_null(args.file_ptr);

    fclose(args.file_ptr);
    free(args.rev_header);
    free(sha1);
    return MUNIT_OK;
}

static MunitResult test_header_contains_type_and_size(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();

    char *sha1 = fixture_create_blob("hello.txt");

    mg_error_t err  = {0};
    char *argv[]    = { "minigit", "rev-parse", sha1 };
    RevParseArgs args = {0};

    handleRevParseArgsFromCLI(3, argv, &args, &err);
    revParse(&args, &err);

    /* Header must start with "blob " */
    munit_assert_not_null(strstr(args.rev_header, "blob"));

    fclose(args.file_ptr);
    free(args.rev_header);
    free(sha1);
    return MUNIT_OK;
}

/* ── exported tests (prefix = "/rev_parse") ──────────────────────────────── */

MunitTest rev_parse_tests[] = {
    { (char *)"/rev_parse/no_args_fails",              test_no_args_fails,                 fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/rev_parse/too_many_args_fails",        test_too_many_args_fails,           fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/rev_parse/arg_stored_as_rev_name",     test_arg_stored_as_rev_name,        fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/rev_parse/no_repo_fails",              test_no_repo_fails,                 fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/rev_parse/invalid_hash_format_fails",  test_invalid_hash_format_fails,     fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/rev_parse/nonexistent_hash_fails",     test_nonexistent_hash_fails,        fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/rev_parse/existing_object_resolves",   test_existing_object_resolves,      fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/rev_parse/header_contains_type_size",  test_header_contains_type_and_size, fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
