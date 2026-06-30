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

void *fixture_setup(const MunitParameter params[], void *user_data);
void  fixture_teardown(void *fixture);

char *fixture_create_blob(const char *filepath);

typedef void (*CaptureFunc)(void *);
char *capture_stdout(CaptureFunc fn, void *arg);

#endif
