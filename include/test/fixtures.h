#ifndef TEST_FIXTURES_H
#define TEST_FIXTURES_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "munit/munit.h"

#define FIXTURE_HELLO_CONTENT   "hello\n"
#define FIXTURE_WORLD_CONTENT   "world\n"
#define FIXTURE_NESTED_CONTENT  "nested file content\n"

typedef struct {
    char original_dir[PATH_MAX];
    char test_dir[PATH_MAX];
} TestFixture;

/* munit-compatible setup / teardown */
void *fixture_setup(const MunitParameter params[], void *user_data);
void  fixture_teardown(void *fixture);

/*
 * Create a valid blob object in .minigit/objects/ for the given file.
 * Returns a malloc'd hex SHA1 string (40 chars + NUL). Caller must free it.
 * The repo must already be initialised (initRepo called) before using this.
 */
char *fixture_create_blob(const char *filepath);

/* Capture what a function prints to stdout.
 * Saves stdout, calls fn(arg), restores stdout.
 * Returns a malloc'd string with the captured output. Caller must free it.
 */
typedef void (*CaptureFunc)(void *);
char *capture_stdout(CaptureFunc fn, void *arg);

#endif /* TEST_FIXTURES_H */
