#include "include/minigit.h"
#include "include/parser.h"

#define MINIGIT_NAME "minigit"

int main() {
    while(true) {
        char **args = NULL;
        int argc = parseMiniGitArgs(&args);
        if (argc == -1) {
            break;
        }

        // Check if we are in front of a minigit command...
        // If we are not, just echo and go to the next iteration
        if(strncmp(args[0], MINIGIT_NAME, strlen(MINIGIT_NAME)) != 0) {
            for(int i=0; i < argc; i++) { fprintf(stdout, "%s", args[i]); printf(" "); }
            printf("\n");
            free(args);
            continue;
        }

        // If we are, we now have to check which command we are running
        // TODO...

        // Always free when done 
        free(args);
    }

    return 0;
}
