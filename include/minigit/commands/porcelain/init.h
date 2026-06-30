#ifndef INIT_H
#define INIT_H

#include "minigit/utils/errors.h"

int initRepo(mg_error_t *err);
int initCommand(int argc, char **argv, mg_error_t *err);

#endif
