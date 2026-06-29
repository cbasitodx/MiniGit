#include <stdlib.h>

#include "munit/munit.h"

/*
 * Each test file exports a MunitTest[] with fully-qualified test names
 * (e.g. "/init/creates_dir").  Here we combine them into two flat suites
 * so that filtering by "/porcelain" or "/plumbing" works out of the box:
 *
 *   ./minigit-tests /porcelain           — all porcelain tests
 *   ./minigit-tests /plumbing            — all plumbing tests
 *   ./minigit-tests /porcelain/init      — all init tests
 *   ./minigit-tests /plumbing/hash_content/no_args_fails — one test
 *
 * To add a new command's tests:
 *   1. Create src/test/{porcelain,plumbing}/test_<cmd>.c exporting
 *      MunitTest <cmd>_tests[].
 *   2. extern-declare it here, append it into the right *_all_tests array,
 *      and bump the static array size below.
 */

/* ── porcelain ────────────────────────────────────────────────────────────── */

extern MunitTest init_tests[];   /* 4 tests */

static MunitTest porcelain_all_tests[
    4   /* init */
    + 1 /* NULL sentinel */
];

/* ── plumbing ─────────────────────────────────────────────────────────────── */

extern MunitTest hash_content_tests[]; /* 12 tests */
extern MunitTest rev_parse_tests[];    /*  8 tests */
extern MunitTest cat_file_tests[];     /*  9 tests */

static MunitTest plumbing_all_tests[
    12  /* hash_content */
    + 8  /* rev_parse */
    + 9  /* cat_file */
    + 1  /* NULL sentinel */
];

/* ── utils ────────────────────────────────────────────────────────────────── */

extern MunitTest errors_tests[]; /* 4 tests */
extern MunitTest read_tests[];   /* 4 tests */

static MunitTest utils_all_tests[
    4   /* errors */
    + 4 /* read */
    + 1 /* NULL sentinel */
];

/* ── helpers ──────────────────────────────────────────────────────────────── */

/* Append a NULL-terminated MunitTest[] into dst, advancing *pos. */
static void append_tests(MunitTest *dst, int *pos, MunitTest *src) {
    for (int i = 0; src[i].test != NULL; i++) {
        dst[(*pos)++] = src[i];
    }
}

/* ── main ─────────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    /* Build porcelain_all_tests */
    int pp = 0;
    append_tests(porcelain_all_tests, &pp, init_tests);
    porcelain_all_tests[pp] = (MunitTest){ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL };

    /* Build plumbing_all_tests */
    int pl = 0;
    append_tests(plumbing_all_tests, &pl, hash_content_tests);
    append_tests(plumbing_all_tests, &pl, rev_parse_tests);
    append_tests(plumbing_all_tests, &pl, cat_file_tests);
    plumbing_all_tests[pl] = (MunitTest){ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL };

    /* Build utils_all_tests */
    int ut = 0;
    append_tests(utils_all_tests, &ut, errors_tests);
    append_tests(utils_all_tests, &ut, read_tests);
    utils_all_tests[ut] = (MunitTest){ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL };

    static MunitSuite top_suites[] = {
        { (char *)"/porcelain", porcelain_all_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE },
        { (char *)"/plumbing",  plumbing_all_tests,  NULL, 1, MUNIT_SUITE_OPTION_NONE },
        { (char *)"/utils",     utils_all_tests,     NULL, 1, MUNIT_SUITE_OPTION_NONE },
        { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
    };

    static const MunitSuite root = {
        (char *)"",
        NULL,
        top_suites,
        1,
        MUNIT_SUITE_OPTION_NONE
    };

    return munit_suite_main(&root, NULL, argc, argv);
}
