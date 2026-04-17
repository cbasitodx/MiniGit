#include "porcelain/init.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minigit.h"

/**
 * Initializes a new minigit repository in the current directory.
 *
 * Creates the .minigit directory along with all required subdirectories
 * and files defined by MINIGIT_INIT_DIRS and MINIGIT_INIT_FILES.
 * If any step fails, it attempts to clean up by removing the root directory.
 *
 * @return 0 on success, -1 if the repo already exists or any creation step fails.
 */
int initRepo() {
    // Check if the minigit init directory is created
    if (opendir(MINIGIT_INIT_DIR)) {
        printf("This project already contains an initialized repo!\n");
        return -1;
    }

    // Create the directory and move to it
    if (mkdir(MINIGIT_INIT_DIR, 0777) && chdir(MINIGIT_INIT_DIR)) {
        return -1;
    }

    const char *dirsNames[] = {MINIGIT_INIT_DIRS};
    for (int i = 0; i < MINIGIT_INIT_DIRS_COUNT; i++) {
        size_t pathLength = strlen(MINIGIT_INIT_DIR) +
                            strlen(dirsNames[i]) +
                            2; // '/' + '\0'

        char *path = malloc(pathLength);
        if (!path) {
            return -1;
        }

        // Create the path string
        snprintf(path, pathLength, "%s/%s", MINIGIT_INIT_DIR, dirsNames[i]);
        if (mkdir(path, 0777)) {
            rmdir(MINIGIT_INIT_DIR); // Try to remove if something fails...
            free(path);
            return -1;
        }

        free(path);
    }

    const char *fileNames[] = {MINIGIT_INIT_FILES};
    for (int i = 0; i < MINIGIT_INIT_FILES_COUNT; i++) {
        size_t pathLength = strlen(MINIGIT_INIT_DIR) +
                            strlen(fileNames[i]) +
                            2; // '/' + '\0'

        char *path = malloc(pathLength);
        if (!path) {
            return -1;
        }

        // Create the path string
        snprintf(path, pathLength, "%s/%s", MINIGIT_INIT_DIR, fileNames[i]);
        if (fopen(path, "w") == NULL) {
            rmdir(MINIGIT_INIT_DIR); // Try to remove if something fails...
            free(path);
            return -1;
        }

        free(path);
    }

    return 0;
}
