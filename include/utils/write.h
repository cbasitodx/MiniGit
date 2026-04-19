#ifndef WRITE_H
#define WRITE_H

#include <stdbool.h>
#include <sys/types.h>

#include "utils/errors.h"

int createDir(const char *path, mode_t mode, bool allowExist, mg_error_t *err);

#endif /* WRITE_H */
