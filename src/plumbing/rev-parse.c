#include "plumbing/rev-parse.h"

#include <stdbool.h>
#include <string.h>

// TODO: This should agree with the one in hash-content.h,
// find an idiomatic solution without importing hash-content.h??? 
#define SHA1_HEX_LENGTH 40

bool isValidHash(const char *str) {                                                    
    if (strlen(str) != SHA1_HEX_LENGTH) return false;                                               
    for (int i = 0; i < SHA1_HEX_LENGTH; i++) {
        if (!((str[i] >= '0' && str[i] <= '9') ||                                      
            (str[i] >= 'a' && str[i] <= 'f')))
            return false;                                                              
    }           
    return true;                                                                       
}       

void revParse(RevParseArgs *args) {

}

bool handleRevParseArgsFromCLI(int argc, char **args_in, RevParseArgs *args_out) {
    return false;
}