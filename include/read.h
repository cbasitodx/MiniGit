#ifndef READ_H
#define READ_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 4096

size_t readData(char **data);

#endif /* READ_H */
