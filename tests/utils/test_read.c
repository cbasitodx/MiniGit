#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../munit/munit.h"
#include "minigit/utils/read.h"

static MunitResult test_read_data_reads_stream(const MunitParameter params[], void *fixture) {
    (void)params;
    (void)fixture;

    const char *content = "hello\n";
    FILE *stream = fmemopen((void *)content, strlen(content), "r");
    munit_assert_not_null(stream);

    Blob blob = readData(stream);
    fclose(stream);

    munit_assert_size(blob.size, ==, strlen(content));
    munit_assert_memory_equal(strlen(content), blob.data, content);

    free(blob.data);
    return MUNIT_OK;
}

MunitTest read_tests[] = {
    { (char *)"/read/reads_stream", test_read_data_reads_stream, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
