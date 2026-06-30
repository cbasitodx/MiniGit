#include <stdbool.h>
#include <sys/stat.h>

#include "fixtures.h"
#include "../munit/munit.h"
#include "minigit/minigit.h"
#include "minigit/commands/porcelain/init.h"
#include "minigit/utils/errors.h"

static bool dir_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static bool file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static MunitResult test_init_creates_repo(const MunitParameter params[], void *fixture) {
    (void)params;
    (void)fixture;

    mg_error_t err = {0};

    munit_assert_int(initRepo(&err), ==, MG_SUCCESS);
    munit_assert_true(dir_exists(MINIGIT_INIT_DIR));
    munit_assert_true(dir_exists(MINIGIT_OBJECTS_PATH));
    munit_assert_true(file_exists(MINIGIT_INIT_DIR "/" MINIGIT_HEAD_FILE));

    return MUNIT_OK;
}

MunitTest init_tests[] = {
    { (char *)"/init/creates_repo", test_init_creates_repo, fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
