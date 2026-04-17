#ifndef MINIGIT_H
#define MINIGIT_H

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MINIGIT_INIT_DIR ".minigit"

#define MINIGIT_INIT_DIRS_COUNT 4
#define MINIGIT_INIT_DIRS "hooks", "info", "objects", "refs"

#define MINIGIT_INIT_FILES_COUNT 3
#define MINIGIT_INIT_FILES "config", "description", "HEAD"

#endif /* MINIGIT_H */
