#include "minigit/platform/compress.h"

#include <zlib.h>

int mg_inflate_buffer(
    const uint8_t *compressed,
    size_t compressed_size,
    uint8_t *out,
    size_t out_size,
    mg_error_t *err
) {
    z_stream stream = {0};
    stream.next_in = (Bytef *)compressed;
    stream.avail_in = (uInt)compressed_size;
    stream.next_out = out;
    stream.avail_out = (uInt)out_size;

    int zret = inflateInit(&stream);
    if (zret != Z_OK) {
        return mgSetError(err, MG_ERR_GENERIC, "Failed to initialize zlib inflate");
    }

    zret = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (zret != Z_STREAM_END) {
        return mgSetError(err, MG_ERR_GENERIC, "Failed to decompress object data");
    }

    return MG_SUCCESS;
}
