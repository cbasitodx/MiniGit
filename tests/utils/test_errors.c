#include <string.h>

#include "../munit/munit.h"
#include "minigit/utils/errors.h"

static MunitResult test_set_error_formats_message(const MunitParameter params[], void *fixture) {
    (void)params;
    (void)fixture;

    mg_error_t err = {0};
    int ret = mgSetError(&err, MG_ERR_GENERIC, "hello %s", "world");

    munit_assert_int(ret, ==, MG_ERR_GENERIC);
    munit_assert_not_null(strstr(err.message, "hello world"));
    return MUNIT_OK;
}

MunitTest errors_tests[] = {
    { (char *)"/errors/formats_message", test_set_error_formats_message, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
