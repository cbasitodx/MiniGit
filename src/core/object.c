#include "minigit/core/object.h"

#include <stdlib.h>
#include <string.h>

void mg_object_free(mg_object_t *object) {
    if (object == NULL) {
        return;
    }

    free(object->data);
    memset(object, 0, sizeof(*object));
}
