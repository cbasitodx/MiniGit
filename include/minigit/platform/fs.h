#ifndef MINIGIT_PLATFORM_FS_H
#define MINIGIT_PLATFORM_FS_H

#include <stdbool.h>

bool mg_dir_exists(const char *path);
bool mg_file_exists(const char *path);

#endif
