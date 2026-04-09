#ifndef READ_H
#define READ_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 4096

typedef struct {
    uint8_t *data;
    size_t size;
} Blob;

Blob readData(FILE *stream);

#endif /* READ_H */
