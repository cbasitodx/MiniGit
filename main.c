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
        if (argc == 0) {
            free(args);
            continue;
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
        if(argc == 1) { printf("Run 'minigit --help' for help with the use of minigit\n"); continue; }
        if(strcmp(args[1], "init") == 0) { initRepo(); }

        // Always free when done 
        free(args);
    }

    return 0;
}
