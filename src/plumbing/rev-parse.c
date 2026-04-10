#include "plumbing/rev-parse.h"

#include "utils/errors.h"

#include <stdbool.h>
#include <string.h>

// TODO: This should agree with the one in hash-content.h,
// find an idiomatic solution without importing hash-content.h??? 
#define SHA1_HEX_LENGTH 40

// TODO: Add more features. For now, we'll work exclusively with these 4 flags
// Adding more features means adding a variable number of arguments
#define CAT_FILE_MAX_ARGS 2

bool isValidHash(const char *str) {                                                    
    if (strlen(str) != SHA1_HEX_LENGTH) return false;                                               
    for (int i = 0; i < SHA1_HEX_LENGTH; i++) {
        if (!((str[i] >= '0' && str[i] <= '9') ||                                      
            (str[i] >= 'a' && str[i] <= 'f')))
            return false;                                                              
    }           
    return true;                                                                       
}       

int handleRevParseArgsFromCLI(int argc, char **args_in, RevParseArgs *args_out, mg_error_t *err) {

}

int revParse(RevParseArgs *args, mg_error_t *err) {

}