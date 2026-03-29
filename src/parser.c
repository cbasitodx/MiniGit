#include "parser.h"

int parseMiniGitArgs(char ***args_out) {
    char *line = NULL;
    size_t len = 0; // capacity of the buffer (getline manages this)
    ssize_t nread = getline(&line, &len, stdin);
    if (nread == -1) {
        // EOF or error
        *args_out = NULL;
        return -1;
    }

    // Strip trailing spaces and newline
    int end = strlen(line) - 1;
    while (end >= 0 && (line[end] == ' ' || line[end] == '\n' || line[end] == '\r')) {
        line[end--] = '\0';
    }
    // Skip leading spaces
    int start = 0;
    while (line[start] == ' ') {
        start++;
    }

    if (start > 0) {
        memmove(line, line + start, strlen(line) - start + 1);
    }

    // Collapse internal runs of spaces to a single space
    int read = 0, write = 0;
    bool in_space = false;
    while (line[read] != '\0') {
        if (line[read] == ' ') {
            if (!in_space) {
                line[write++] = ' ';
                in_space = true;
            }
        } else {
            line[write++] = line[read];
            in_space = false;
        }
        read++;
    }
    line[write] = '\0';

    // Line should now be a word or a series of words separated by one space
    // Split into args array
    int argc = 0;
    char **args = NULL;
    char *token = strtok(line, " ");
    while (token != NULL) {
        args = realloc(args, (argc + 1) * sizeof(char *));
        args[argc++] = strdup(token);
        token = strtok(NULL, " ");
    }

    *args_out = args;
    free(line);
    return argc;
}
