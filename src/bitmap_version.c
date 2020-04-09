/**
 * @file bitmap_version.c
 */

#include <bitmap/bitmap.h>

#include <assert.h>

#ifndef VERSION_HASH
#   define VERSION_HASH  "VERSION_HASH"
#endif

#ifndef VERSION_DATETIME
#   define VERSION_DATETIME  "VERSION_DATETIME"
#endif

#ifndef CFLAGS
#   define CFLAGS  ""
#endif

static const struct bitmap_version P_version =
{
        .hash      = VERSION_HASH,
        .date_time = VERSION_DATETIME,
        .cflags    = CFLAGS,
};

const struct bitmap_version * bitmap_version0(void)
{
    static_assert(
            BITMAP_BYTES_IN_BLOCK() * BITMAP_BITS_IN_BYTE() == BITMAP_BITS_IN_BLOCK_DEFINE,
            "BITMAP_BYTES_IN_BLOCK() * BITMAP_BITS_IN_BYTE() == BITMAP_BITS_IN_BLOCK_DEFINE"
    );

    return &P_version;
}
