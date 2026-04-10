#ifndef ERRORS_H
#define ERRORS_H

#include <stdarg.h>

#define MG_SUCCESS 0
#define MG_ERR_NOT_ENOUGH_ARGS 1
#define MG_ERR_TOO_MANY_ARGS 2
#define MG_ERR_UNKNOWN_OPTION 3
#define MG_ERR_ALLOCATION_FAILED 4
#define MG_ERR_FILE_OPEN_FAILED 5

#define MG_ERR_MESSAGE_BUF_SIZE 512
typedef struct {
    int code;
    char message[MG_ERR_MESSAGE_BUF_SIZE];
} mg_error_t;

int mg_set_error(mg_error_t *err, int code, const char *fmt, ...);

#endif /* ERRORS_H */
