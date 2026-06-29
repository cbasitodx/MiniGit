#include <sys/stat.h>
#include <unistd.h>

#include "test/fixtures.h"
#include "../munit/munit.h"
#include "minigit.h"
#include "porcelain/init.h"

/* ── helpers ──────────────────────────────────────────────────────────────── */

static bool dir_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static bool file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

/* ── tests ────────────────────────────────────────────────────────────────── */

static MunitResult test_creates_minigit_dir(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    munit_assert_int(initRepo(), ==, 0);
    munit_assert_true(dir_exists(MINIGIT_INIT_DIR));
    return MUNIT_OK;
}

static MunitResult test_creates_subdirs(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();
    munit_assert_true(dir_exists(MINIGIT_INIT_DIR "/" MINIGIT_HOOKS_DIR));
    munit_assert_true(dir_exists(MINIGIT_INIT_DIR "/" MINIGIT_INFO_DIR));
    munit_assert_true(dir_exists(MINIGIT_INIT_DIR "/" MINIGIT_OBJECTS_DIR));
    munit_assert_true(dir_exists(MINIGIT_INIT_DIR "/" MINIGIT_REFS_DIR));
    return MUNIT_OK;
}

static MunitResult test_creates_files(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    initRepo();
    munit_assert_true(file_exists(MINIGIT_INIT_DIR "/" MINIGIT_CONFIG_FILE));
    munit_assert_true(file_exists(MINIGIT_INIT_DIR "/" MINIGIT_DESCRIPTION_FILE));
    munit_assert_true(file_exists(MINIGIT_INIT_DIR "/" MINIGIT_HEAD_FILE));
    return MUNIT_OK;
}

static MunitResult test_fails_if_already_initialized(const MunitParameter p[], void *fix) {
    (void)p; (void)fix;
    munit_assert_int(initRepo(), ==,  0);
    munit_assert_int(initRepo(), ==, -1);
    return MUNIT_OK;
}

/* ── exported tests (prefix = "/init") ───────────────────────────────────── */

MunitTest init_tests[] = {
    { (char *)"/init/creates_minigit_dir",      test_creates_minigit_dir,          fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/init/creates_subdirs",          test_creates_subdirs,              fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/init/creates_files",            test_creates_files,                fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { (char *)"/init/fails_if_already_inited",  test_fails_if_already_initialized, fixture_setup, fixture_teardown, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
