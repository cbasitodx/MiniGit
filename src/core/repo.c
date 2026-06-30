#include "minigit/core/repo.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "minigit/minigit.h"
#include "minigit/platform/fs.h"

int mg_repo_open(mg_repo_t *repo, mg_error_t *err) {
    if (!mg_dir_exists(MINIGIT_INIT_DIR)) {
        return mgSetError(
            err,
            MG_ERR_DIR_OPEN_FAILED,
            "minigit repository was not initialized. Please initialize it with 'minigit init'"
        );
    }

    if (!mg_dir_exists(MINIGIT_OBJECTS_PATH)) {
        return mgSetError(
            err,
            MG_ERR_DIR_OPEN_FAILED,
            "minigit repository does not contains an objects directory."
        );
    }

    snprintf(repo->git_dir, sizeof(repo->git_dir), "%s", MINIGIT_INIT_DIR);
    snprintf(repo->objects_dir, sizeof(repo->objects_dir), "%s", MINIGIT_OBJECTS_PATH);
    return MG_SUCCESS;
}

int mg_repo_init(mg_error_t *err) {
    if (mg_dir_exists(MINIGIT_INIT_DIR)) {
        return mgSetError(err, MG_ERR_GENERIC, "This project already contains an initialized repo!");
    }

    if (mkdir(MINIGIT_INIT_DIR, 0777) != 0) {
        return mgSetError(err, MG_ERR_DIR_OPEN_FAILED, "Failed to create %s", MINIGIT_INIT_DIR);
    }

    const char *dirs[] = {MINIGIT_INIT_DIRS};
    for (int i = 0; i < MINIGIT_INIT_DIRS_COUNT; i++) {
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", MINIGIT_INIT_DIR, dirs[i]);

        if (mkdir(path, 0777) != 0) {
            rmdir(MINIGIT_INIT_DIR);
            return mgSetError(err, MG_ERR_DIR_OPEN_FAILED, "Failed to create %s", path);
        }
    }

    const char *files[] = {MINIGIT_INIT_FILES};
    for (int i = 0; i < MINIGIT_INIT_FILES_COUNT; i++) {
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", MINIGIT_INIT_DIR, files[i]);

        FILE *file = fopen(path, "w");
        if (file == NULL) {
            rmdir(MINIGIT_INIT_DIR);
            return mgSetError(err, MG_ERR_FILE_OPEN_FAILED, "Failed to create %s", path);
        }

        fclose(file);
    }

    char head_path[PATH_MAX];
    snprintf(head_path, sizeof(head_path), "%s/%s", MINIGIT_INIT_DIR, MINIGIT_HEAD_FILE);

    FILE *head = fopen(head_path, "w");
    if (head == NULL) {
        return mgSetError(err, MG_ERR_FILE_OPEN_FAILED, "Failed to write %s", head_path);
    }

    fputs(MINIGIT_HEAD_INITIAL_CONTENT, head);
    fclose(head);

    return MG_SUCCESS;
}
