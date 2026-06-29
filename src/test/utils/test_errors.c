#include <string.h>

#include "test/fixtures.h"
#include "../munit/munit.h"
#include "utils/errors.h"

/* ── tests ────────────────────────────────────────────────────────────────── */

static MunitResult test_sets_code(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err = {0};
    mgSetError(&err, MG_ERR_NOT_ENOUGH_ARGS, "msg");
    munit_assert_int(err.code, ==, MG_ERR_NOT_ENOUGH_ARGS);
    return MUNIT_OK;
}

static MunitResult test_returns_code(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err = {0};
    int ret = mgSetError(&err, MG_ERR_FILE_OPEN_FAILED, "msg");
    munit_assert_int(ret, ==, MG_ERR_FILE_OPEN_FAILED);
    return MUNIT_OK;
}

static MunitResult test_formats_message(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err = {0};
    mgSetError(&err, MG_ERR_GENERIC, "val=%d name=%s", 42, "foo");
    munit_assert_not_null(strstr(err.message, "42"));
    munit_assert_not_null(strstr(err.message, "foo"));
    return MUNIT_OK;
}

static MunitResult test_no_buffer_overflow(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    mg_error_t err = {0};
    /* Build a format string whose output would exceed MG_ERR_MESSAGE_BUF_SIZE */
    char big[MG_ERR_MESSAGE_BUF_SIZE * 2 + 1];
    memset(big, 'x', sizeof(big) - 1);
    big[sizeof(big) - 1] = '\0';
    mgSetError(&err, MG_ERR_GENERIC, "%s", big);
    /* message must be null-terminated within its buffer */
    munit_assert_int(err.message[MG_ERR_MESSAGE_BUF_SIZE - 1], ==, '\0');
    return MUNIT_OK;
}

/* ── exported tests (prefix = "/errors") ─────────────────────────────────── */

MunitTest errors_tests[] = {
    { (char *)"/errors/sets_code",          test_sets_code,          NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/errors/returns_code",       test_returns_code,       NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/errors/formats_message",    test_formats_message,    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/errors/no_buffer_overflow", test_no_buffer_overflow, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
