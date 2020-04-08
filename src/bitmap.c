/**
 * @file bitmap.c
 * @brief Implementation of the bitmap functions.
 */

#include <assert.h>

#include <bitmap/bitmap.h>

#include "bitmap_common.h"

#include <string.h>

void bitmap_bitwise_raise1(
        bitmap_block_t * bitmap,
        size_t bits_num
)
{
    static_assert(
            sizeof(bitmap_block_t) * BITMAP_BITS_IN_BYTE() == BITMAP_BITS_IN_BLOCK_DEFINE,
            "sizeof(bitmap_block_t) * BITMAP_BITS_IN_BYTE() == BITMAP_BITS_IN_BLOCK_DEFINE"
    );

    memset(bitmap, -1, BITMAP_BITS_TO_BYTES_ALIGNED(bits_num));
}

void bitmap_bitwise_clear2(
        bitmap_block_t * bitmap,
        size_t bits_num
)
{
    memset(bitmap, 0, BITMAP_BITS_TO_BYTES_ALIGNED(bits_num));
}

void bitmap_bitwise_range_raise2(
        bitmap_block_t * bitmap,
        const bitmap_range_t * range
)
{
    size_t i;
    for(i = range->begin; i <= range->end; ++i)
    {
        bitmap_bit_raise2(bitmap, i);
    }
}

void bitmap_bitwise_range_clear2(
        bitmap_block_t * bitmap,
        const bitmap_range_t * range
)
{
    size_t i;
    for(i = range->begin; i <= range->end; ++i)
    {
        bitmap_bit_clear2(bitmap, i);
    }
}

void bitmap_bitwise_copy3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    memcpy(dest, src, BITMAP_BITS_TO_BYTES_ALIGNED(bits_num));
}

void bitmap_bitwise_not3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] = ~src[iblock];
    }
}

void bitmap_bitwise_or3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] |= src[iblock];
    }
}

void bitmap_bitwise_or4(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] = a[iblock] | b[iblock];
    }
}

void bitmap_bitwise_and3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] &= src[iblock];
    }
}

void bitmap_bitwise_and4(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] = a[iblock] & b[iblock];
    }
}

void bitmap_bitwise_clear3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] &= ~src[iblock];
    }
}

void bitmap_bitwise_clear4(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] = a[iblock] & ~b[iblock];
    }
}

void bitmap_bit_raise2(
        bitmap_block_t *bitmap,
        size_t bit_index
)
{

    bitmap_block_t *block = &bitmap[bit_index / BITMAP_BITS_IN_BLOCK()];
    bitmap_block_t bit = BITMAP_RAISED_BIT(bit_index % BITMAP_BITS_IN_BLOCK());
    *block |= bit;
}

void bitmap_bit_clear2(
        bitmap_block_t *bitmap,
        size_t bit_index
)
{
    bitmap_block_t *block = &bitmap[bit_index / BITMAP_BITS_IN_BLOCK()];
    bitmap_block_t bit = BITMAP_RAISED_BIT(bit_index % BITMAP_BITS_IN_BLOCK());
    *block &= ~bit;
}

bool bitmap_bit_get2(
        const bitmap_block_t *bitmap,
        size_t bit_index
)
{
    bitmap_block_t block = bitmap[bit_index / BITMAP_BITS_IN_BLOCK()];
    bitmap_block_t bit = BITMAP_RAISED_BIT(bit_index % BITMAP_BITS_IN_BLOCK());
    return ((block & bit) != 0);
}

void bitmap_bit_nearest_forward_raised_get4(
        const bitmap_block_t *bitmap,
        size_t bits_num,
        size_t bit_index_from,
        bitmap_bit_nearest_get_context_t * bit_nearest
)
{
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);
    size_t iblock;
    size_t bit_index_tmp;
    for(
            iblock = bit_index_from / BITMAP_BITS_IN_BLOCK(), bit_index_tmp = bit_index_from;
            iblock < blocks_num;
            ++iblock                                        , bit_index_tmp = iblock * BITMAP_BITS_IN_BLOCK()
    )
    {
        const bitmap_block_t block = bitmap[iblock];
        if(block == 0) /* skip zero block */
        {
            continue;
        }

        bool last = (iblock + 1 == blocks_num);
        size_t blockbits = last ?
                BITMAP_BITS_IN_TAILBLOCK(bits_num) :
                BITMAP_BITS_IN_BLOCK();

        uint8_t iblockbit;
        for(
                iblockbit = bit_index_tmp % BITMAP_BITS_IN_BLOCK();
                iblockbit < blockbits;
                ++iblockbit
        )
        {
            bitmap_block_t bit = BITMAP_RAISED_BIT(iblockbit);
            bool found = ((block & bit) != 0);
            if(found)
            {
                bit_nearest->index = (iblock * BITMAP_BITS_IN_BLOCK() + iblockbit);
                bit_nearest->exist = true;
                return;
            }
        }
    }

    bit_nearest->exist = false;
}
