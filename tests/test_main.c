#include <stdlib.h>

#include "munit/munit.h"

extern MunitTest init_tests[];

static MunitTest porcelain_all_tests[
    1
    + 1
];

extern MunitTest hash_content_tests[];
extern MunitTest rev_parse_tests[];
extern MunitTest cat_file_tests[];

static MunitTest plumbing_all_tests[
    1
    + 1
    + 1
    + 1
];

extern MunitTest errors_tests[];
extern MunitTest read_tests[];

static MunitTest utils_all_tests[
    1
    + 1
    + 1
];

static void append_tests(MunitTest *dst, int *pos, MunitTest *src) {
    for (int i = 0; src[i].test != NULL; i++) {
        dst[(*pos)++] = src[i];
    }
}

int main(int argc, char *argv[]) {
    int pp = 0;
    append_tests(porcelain_all_tests, &pp, init_tests);
    porcelain_all_tests[pp] = (MunitTest){ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL };

    int pl = 0;
    append_tests(plumbing_all_tests, &pl, hash_content_tests);
    append_tests(plumbing_all_tests, &pl, rev_parse_tests);
    append_tests(plumbing_all_tests, &pl, cat_file_tests);
    plumbing_all_tests[pl] = (MunitTest){ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL };

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
