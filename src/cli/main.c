#include <stdio.h>

#include "minigit/cli/dispatch.h"
#include "minigit/utils/errors.h"

int main(int argc, char *argv[]) {
    mg_error_t error = {0};
    int ret = mg_cli_dispatch(argc, argv, &error);
    if (ret != MG_SUCCESS && error.message[0] != '\0') {
        fprintf(stderr, "%s\n", error.message);
    }

    return ret;
}
