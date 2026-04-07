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

/**
 * Reads data from the given stream and stores it in a dynamically allocated buffer.
 * The caller is responsible for freeing the allocated buffer.
 *
 * @param stream The input stream to read from.
 *
 * @return A Blob structure containing the read data and its size.
 */
Blob readData(FILE *stream);

#endif /* READ_H */
