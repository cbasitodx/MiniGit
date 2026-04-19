#include "utils/write.h"

#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "utils/errors.h"

/**
 * Helper function for creating directories with error handling.
 * Recursivity is NOT implemented, so the parent directory must already exist.
 *
 * @param path The path of the directory to create.
 * @param mode The permissions to use when creating the directory (e.g., 0755).
 * @param exist_ok If true, the function will not return an error if the directory already exists.
 * @param err The error struct to populate in case of an error.
 *
 * @return 0 if successful, non-zero if there's an error.
 */
int createDir(const char *path, mode_t mode, bool exist_ok, mg_error_t *err) {
    if (mkdir(path, mode) != 0) {
        if (exist_ok && errno == EEXIST) {
            return MG_SUCCESS;
        }

        return errno;
    }
    return MG_SUCCESS;
}
