#include "utils/read.h"

void growBuffer(uint8_t **data, size_t *capacity) {
    (*capacity) += CHUNK_SIZE;
    uint8_t *temp = (uint8_t *)realloc(*data, (*capacity) * sizeof(uint8_t));

    if (temp == NULL) {
        free(*data);
        fprintf(stderr, "Memory allocation failed\n");
        exit(-1);
    }

    *data = temp;
    return;
}

size_t initializeBuffer(uint8_t **data) {
    *data = NULL;
    size_t capacity = 0;
    growBuffer(data, &capacity);
    return capacity;
}

/**
 * Reads data from the given stream and stores it in a dynamically allocated buffer.
 * The caller is responsible for freeing the allocated buffer.
 *
 * @param stream The input stream to read from.
 *
 * @return A Blob structure containing the read data and its size.
 */
Blob readData(FILE *stream) {
    Blob blob = {0};
    size_t capacity = initializeBuffer(&(blob.data));

    int chr;
    while ((chr = fgetc(stream)) != EOF) {
        if (blob.size >= capacity) {
            growBuffer(&(blob.data), &capacity);
        }

        blob.data[blob.size] = (uint8_t)chr;
        blob.size++;
    }

    return blob;
}
