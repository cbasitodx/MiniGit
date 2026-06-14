#ifndef READ_H
#define READ_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t *data;
    size_t size;
} Blob;

Blob readData(FILE *stream);

#endif /* READ_H */
