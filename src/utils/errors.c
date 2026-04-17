#include "utils/errors.h"

#include <stdarg.h>
#include <stdio.h>

int mgSetError(mg_error_t *err, int code, const char *fmt, ...) {
    err->code = code;

    va_list args;
    va_start(args, fmt);
    vsnprintf(err->message, sizeof(err->message), fmt, args);
    va_end(args);

    return code;
}
