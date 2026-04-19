#include "utils/compress.h"

#include <stdlib.h>
#include <zlib.h>

#include "utils/read.h"

/**
 * Allocates memory for blob_out's data field based on the size of blob_in.
 *
 * @param blob_in The input blob to be compressed.
 * @param blob_out The output blob that will hold the compressed data.
 *
 * @return 0 if successful, 1 if memory allocation fails.
 */
int prepare_compress_blob(Blob *blob_in, Blob *blob_out) {
    blob_out->size = compressBound(blob_in->size);
    blob_out->data = (uint8_t *)malloc(blob_out->size);

    if (blob_out->data == NULL) {
        return 1;
    }

    return 0;
}

/**
 * Compresses the data in blob_in and stores the result in blob_out.
 * The caller is responsible for ensuring that blob_out has enough allocated memory to hold the compressed data.
 *
 * @param blob_in The input blob containing the data to be compressed.
 * @param blob_out The output blob that will hold the compressed data. Its size field should
 *
 * @return 0 if successful, 1 if compression fails (e.g., if the output buffer is too small).
 */
int compress_blob(Blob *blob_in, Blob *blob_out) {
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if (deflateInit(&strm, Z_DEFAULT_COMPRESSION) != Z_OK) {
        return 1;
    }

    strm.next_in = blob_in->data;
    strm.avail_in = blob_in->size;
    strm.next_out = blob_out->data;
    strm.avail_out = blob_out->size;

    int ret = deflate(&strm, Z_FINISH);

    if (ret != Z_STREAM_END) {
        deflateEnd(&strm);
        return 1; // Buffer probably too small
    }

    blob_out->size = strm.total_out;
    deflateEnd(&strm);
    return 0;
}
