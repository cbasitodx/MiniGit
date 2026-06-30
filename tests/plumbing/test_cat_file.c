#include <stdlib.h>
#include <string.h>

#include "fixtures.h"
#include "../munit/munit.h"
#include "minigit/commands/plumbing/cat-file.h"
#include "minigit/commands/porcelain/init.h"
#include "minigit/utils/errors.h"

typedef struct {
    CatFileArgs *args;
    mg_error_t *err;
} CatFileCall;

static void run_cat_file(void *arg) {
    CatFileCall *call = arg;
    catFile(call->args, call->err);
}

static MunitResult test_cat_file_prints_blob_content(const MunitParameter params[], void *fixture) {
    (void)params;
    (void)fixture;

    mg_error_t err = {0};
    munit_assert_int(initRepo(&err), ==, MG_SUCCESS);

    char *sha1 = fixture_create_blob("hello.txt");
    CatFileArgs args = {
        .opt_print = true,
        .object_name = sha1,
    };
    CatFileCall call = { .args = &args, .err = &err };

    char *out = capture_stdout(run_cat_file, &call);
    munit_assert_not_null(strstr(out, "hello"));

    free(out);
    free(sha1);
    return MUNIT_OK;
}

MunitTest cat_file_tests[] = {
    { (char *)"/cat_file/prints_blob_content", test_cat_file_prints_blob_content, fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
