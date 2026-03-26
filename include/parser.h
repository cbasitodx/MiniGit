#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Reads one line from stdin, normalizes whitespace, and splits it into tokens.
// On success, sets *args_out to a heap-allocated array of token pointers 
// and returns the number of tokens (argc).
// Returns -1 on EOF or read error; *args_out is NULL in that case.
// The caller is responsible for freeing *args_out
int parseMiniGitArgs(char ***args_out);

#endif /* PARSER_H */
