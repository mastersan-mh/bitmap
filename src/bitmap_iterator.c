/**
 * @file bitmap_iterator.c
 */

#include <bitmap/bitmap.h>

#include "bitmap_common.h"

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

