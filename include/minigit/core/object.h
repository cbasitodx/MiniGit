#ifndef MINIGIT_CORE_OBJECT_H
#define MINIGIT_CORE_OBJECT_H

#include <stddef.h>
#include <stdint.h>

#define MG_OBJECT_TYPE_MAX 16
#define MG_OBJECT_HEADER_INITIAL_SIZE 64

typedef struct {
    char type[MG_OBJECT_TYPE_MAX];
    size_t size;
    uint8_t *data;
} mg_object_t;

void mg_object_free(mg_object_t *object);

#endif
