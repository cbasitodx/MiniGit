#ifndef COMPRESS_H
#define COMPRESS_H

#include "utils/read.h"

int compress_blob(Blob *blob_in, Blob *blob_out);
int prepare_compress_blob(Blob *blob_in, Blob *blob_out);

#endif /* COMPRESS_H */
