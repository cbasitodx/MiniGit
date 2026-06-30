#include <stdlib.h>
#include <string.h>

#include "fixtures.h"
#include "../munit/munit.h"
#include "minigit/commands/plumbing/rev-parse.h"
#include "minigit/commands/porcelain/init.h"
#include "minigit/utils/errors.h"

typedef struct {
    RevParseArgs *args;
    mg_error_t *err;
} RevParseCall;

static void run_rev_parse(void *arg) {
    RevParseCall *call = arg;
    revParse(call->args, call->err);
}

static MunitResult test_rev_parse_existing_object(const MunitParameter params[], void *fixture) {
    (void)params;
    (void)fixture;

    mg_error_t err = {0};
    munit_assert_int(initRepo(&err), ==, MG_SUCCESS);

    char *sha1 = fixture_create_blob("hello.txt");
    RevParseArgs args = { .rev_name = sha1 };
    RevParseCall call = { .args = &args, .err = &err };

    char *out = capture_stdout(run_rev_parse, &call);
    munit_assert_not_null(strstr(out, sha1));

    free(out);
    free(sha1);
    return MUNIT_OK;
}

MunitTest rev_parse_tests[] = {
    { (char *)"/rev_parse/existing_object", test_rev_parse_existing_object, fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
