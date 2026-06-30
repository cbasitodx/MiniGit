#include <stdlib.h>

#include "fixtures.h"
#include "../munit/munit.h"
#include "minigit/commands/plumbing/hash-content.h"
#include "minigit/utils/errors.h"

static MunitResult test_hash_file_success(const MunitParameter params[], void *fixture) {
    (void)params;
    (void)fixture;

    mg_error_t err = {0};
    HashContentArgs args = {
        .file_path = strdup("hello.txt"),
    };

    munit_assert_int(hashContent(&args, &err), ==, MG_SUCCESS);
    return MUNIT_OK;
}

MunitTest hash_content_tests[] = {
    { (char *)"/hash_content/hash_file_success", test_hash_file_success, fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
