#ifndef READ_H
#define READ_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 4096

/**
 * Reads data from the given stream and stores it in a dynamically allocated buffer.
 * The caller is responsible for freeing the allocated buffer.
 *
 * @param stream The input stream to read from.
 * @param data A pointer to a char pointer where the allocated buffer will be stored.
 *
 * @return The total number of bytes read from the stream, or 0 if an error occurs.
 */
size_t readData(FILE *stream, char **data);

#endif /* READ_H */
