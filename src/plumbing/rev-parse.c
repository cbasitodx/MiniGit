#include "plumbing/rev-parse.h"

#include "utils/errors.h"

#include <stdbool.h>
#include <string.h>

// TODO: This should agree with the one in hash-content.h,
// find an idiomatic solution without importing hash-content.h??? 
#define SHA1_HEX_LENGTH 40

// TODO: Add more features. Adding more features means adding a variable number of arguments
#define REV_PARSE_MAX_ARGS 1

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
    if (argc < REV_PARSE_MAX_ARGS + 2) {
        return mg_set_error(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "Not enough args [ERR MSG IN CONSTRUCTION]"
        );
    }

    if (argc > REV_PARSE_MAX_ARGS + 2) {
        return mg_set_error(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "Too many arguments [ERR MSG IN CONSTRUCTION]"
        );
    }

    args_out->rev_name = args_in[2];

    return MG_SUCCESS;
}

int revParse(RevParseArgs *args, mg_error_t *err) {
    if(isValidHash(args->rev_name)) {

    }
    
    return MG_SUCCESS;
}