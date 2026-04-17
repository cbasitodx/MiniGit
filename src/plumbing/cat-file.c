#include "plumbing/cat-file.h"

#include "utils/errors.h"
#include "plumbing/rev-parse.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h> // TODO: Take this out of here

// TODO: Add more features. For now, we'll work exclusively with these 4 flags
// Adding more features means adding a variable number of arguments
#define CAT_FILE_MAX_ARGS 2

#define OPTION_TYPE "-t"
#define OPTION_SIZE "-s"
#define OPTION_EXISTS "-e"
#define OPTION_PRINT "-p"

// TODO: Add some error handling to this function
char* readTypeFromHeader(CatFileArgs *args, mg_error_t *err) {
    char* obj_type = calloc(SOME_BIG_NUMBER, sizeof(char)); // TODO: change this crap
    sscanf(args->object_header, "%s %*zu", obj_type);
    
    return obj_type;
}

// TODO: Add some error handling to this function
size_t readSizeFromHeader(CatFileArgs *args, mg_error_t *err) {
    size_t obj_size;
    sscanf(args->object_header, "%*s %zu", &obj_size);
    
    return obj_size;
}

int handleCatFileArgsFromCLI(int argc, char **args_in, CatFileArgs *args_out, mg_error_t *err) {
    if (argc < CAT_FILE_MAX_ARGS + 2) {
        return mgSetError(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "Not enough args [ERR MSG IN CONSTRUCTION]"
        );
    }

    if (argc > CAT_FILE_MAX_ARGS + 2) {
        return mgSetError(
            err,
            MG_ERR_NOT_ENOUGH_ARGS,
            "Too many arguments [ERR MSG IN CONSTRUCTION]"
        );
    }

    args_out->opt_type = false;
    args_out->opt_size = false;
    args_out->opt_exists = false;
    args_out->opt_print = false;
    args_out->object_name = NULL;
    args_out->object_file = NULL;
    args_out->object_hash = NULL;
    args_out->object_exists = false;
    args_out->object_header = NULL;

    // Start at 2 to skip the command name and subcommand
    if (strncmp(args_in[2], OPTION_TYPE, strlen(OPTION_TYPE)) == 0) {
        args_out->opt_type = true;
    } else if (strncmp(args_in[2], OPTION_SIZE, strlen(OPTION_SIZE)) == 0) {
        args_out->opt_size = true;
    } else if (strncmp(args_in[2], OPTION_EXISTS, strlen(OPTION_EXISTS)) == 0) {
        args_out->opt_exists = true;
    } else if (strncmp(args_in[2], OPTION_PRINT, strlen(OPTION_PRINT)) == 0) {
        args_out->opt_print = true;
    }

    args_out->object_name = args_in[3];
    
    // Let's try to fill args_out->object_file
    RevParseArgs revParseArgs = {0};
    char *argv_rev_pase[] = {"minigit", "rev-parse", args_out->object_name};
    if (handleRevParseArgsFromCLI(3, argv_rev_pase, &revParseArgs, err) != 0) {
        return err->code;
    }
    revParse(&revParseArgs, err);
    if(revParseArgs.file_ptr != NULL && revParseArgs.rev_hash != NULL && revParseArgs.rev_header != NULL) { 
        args_out->object_file = revParseArgs.file_ptr; 
        args_out->object_hash = revParseArgs.rev_hash;
        args_out->object_header = revParseArgs.rev_header;
        args_out->object_exists = true;
    }
    
    return MG_SUCCESS;
}

int catFile(CatFileArgs *args, mg_error_t *err) {
    if (args->opt_type) {
        if(!args->object_exists) {
            return mgSetError(
                err,
                MG_ERR_GENERIC,
                "Object does not exists"
            );
        }
        char* object_type = readTypeFromHeader(args, err);
        printf("The type is %s\n", object_type);
        free(object_type);
    }

    else if (args->opt_size) {
        if(!args->object_exists) {
            return mgSetError(
                err,
                MG_ERR_GENERIC,
                "Object does not exists"
            );
        }
        size_t object_size = readSizeFromHeader(args, err);
        printf("The size is %zu\n", object_size);
    }

    else if (args->opt_exists) {
        if(args->object_exists) { printf("Object exists\n"); }
        else { printf("Object does not exists\n"); }
    }

    else if (args->opt_print) {
        if(!args->object_exists) {
            return mgSetError(
                err,
                MG_ERR_GENERIC,
                "Object does not exists"
            );
        }
        
        // TODO: Take this out of the function
        // @joaquin pleeeeease make a decompressing function :(

        // file_ptr is already positioned after the header \0 — read only the compressed part
        long current_pos = ftell(args->object_file);
        fseek(args->object_file, 0, SEEK_END);
        long compressed_size = ftell(args->object_file) - current_pos;
        fseek(args->object_file, current_pos, SEEK_SET);

        unsigned char *compressed = malloc(compressed_size);
        fread(compressed, 1, compressed_size, args->object_file);

        size_t decompressed_size = readSizeFromHeader(args, err);
        unsigned char *decompressed = malloc(decompressed_size);

        z_stream s = {0};
        s.next_in   = compressed;
        s.avail_in  = compressed_size;
        s.next_out  = decompressed;
        s.avail_out = decompressed_size;

        inflateInit(&s);
        inflate(&s, Z_FINISH);
        inflateEnd(&s);

        free(compressed);

        fwrite(decompressed, 1, decompressed_size, stdout);
        free(decompressed);

        // char line[1024];                                                                                                                                                                                   
        // while (fgets(line, sizeof(line), args->object_file))
        // fputs(line, stdout); 
    }

    // Clean up shit
    if(args->object_file != NULL) { fclose(args->object_file); } 

    return MG_SUCCESS;
}
