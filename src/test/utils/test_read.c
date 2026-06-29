#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test/fixtures.h"
#include "../munit/munit.h"
#include "utils/read.h"

/* ── tests ────────────────────────────────────────────────────────────────── */

static MunitResult test_reads_content(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    const char *content = "hello\nworld\n";
    size_t len = strlen(content);
    FILE *stream = fmemopen((void *)content, len, "r");
    munit_assert_not_null(stream);

    Blob blob = readData(stream);
    fclose(stream);

    munit_assert_size(blob.size, ==, len);
    munit_assert_memory_equal(len, blob.data, content);

    free(blob.data);
    return MUNIT_OK;
}

static MunitResult test_reads_empty_file(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    FILE *stream = tmpfile();
    munit_assert_not_null(stream);

    Blob blob = readData(stream);
    fclose(stream);

    munit_assert_size(blob.size, ==, 0);

    free(blob.data);
    return MUNIT_OK;
}

static MunitResult test_reads_large_file(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    /* Use more than one 4096-byte chunk to exercise buffer growth */
    const size_t len = 4096 * 3 + 7;
    char *content = malloc(len);
    munit_assert_not_null(content);
    memset(content, 'a', len);

    FILE *stream = fmemopen(content, len, "r");
    munit_assert_not_null(stream);

    Blob blob = readData(stream);
    fclose(stream);

    munit_assert_size(blob.size, ==, len);
    munit_assert_memory_equal(len, blob.data, content);

    free(blob.data);
    free(content);
    return MUNIT_OK;
}

static MunitResult test_reads_binary_data(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    /* Ensure null bytes inside the stream don't terminate reading early */
    unsigned char content[] = { 0x00, 0x01, 0x02, 0xFF, 0x00 };
    size_t len = sizeof(content);
    FILE *stream = fmemopen(content, len, "r");
    munit_assert_not_null(stream);

    Blob blob = readData(stream);
    fclose(stream);

    munit_assert_size(blob.size, ==, len);
    munit_assert_memory_equal(len, blob.data, content);

    free(blob.data);
    return MUNIT_OK;
}

/* ── exported tests (prefix = "/read") ───────────────────────────────────── */

MunitTest read_tests[] = {
    { (char *)"/read/reads_content",    test_reads_content,    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/read/reads_empty_file", test_reads_empty_file, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/read/reads_large_file", test_reads_large_file, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/read/reads_binary_data", test_reads_binary_data, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
