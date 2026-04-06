#include "read.h"

void growBuffer(char **data, size_t *capacity) {
    (*capacity) += CHUNK_SIZE;
    char *temp = (char *)realloc(*data, (*capacity) * sizeof(char));

    if (temp == NULL) {
        free(*data);
        fprintf(stderr, "Memory allocation failed\n");
        exit(-1);
    }

    *data = temp;
    return;
}

size_t initializeBuffer(char **data) {
    *data = NULL;
    size_t capacity = 0;
    growBuffer(data, &capacity);
    return capacity;
}

size_t readData(char **data) {
    size_t capacity = initializeBuffer(data);

    int chr;
    size_t idx = 0;

    while ((chr = getchar()) != EOF) {
        if (idx >= capacity) {
            growBuffer(data, &capacity);
        }

        (*data)[idx] = (char)chr;
        idx++;
    }

    return idx;
}
