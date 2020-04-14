/**
 * @file bitmap_bitwise_check.c
 */

#include <bitmap/bitmap.h>

#include "bitmap_common.h"

bool bitmap_bitwise_check_zero2(
        const bitmap_block_t *bitmap,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);

    BITMAP_FOREACH_BLOCK_EXTENDED_BEGIN(iblock, blocks_num)
    {
        bitmap_block_t block;
        block = bitmap[iblock];
        if(block != 0)
        {
            return false;
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_LASTBLOCK(iblock, blocks_num)
    {
        bitmap_block_t block;
        block = bitmap[iblock];
        bitmap_block_t tailmask = bitmap_P_tailblock_mask(bits_num);
        block &= tailmask;
        if(block != 0)
        {
            return false;
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_END();

    return true;
}

bool bitmap_bitwise_check_equal3(
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);

    BITMAP_FOREACH_BLOCK_EXTENDED_BEGIN(iblock, blocks_num)
    {
        bitmap_block_t block_a = a[iblock];
        bitmap_block_t block_b = b[iblock];
        if(block_a != block_b)
        {
            return false;
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_LASTBLOCK(iblock, blocks_num)
    {
        bitmap_block_t block_a = a[iblock];
        bitmap_block_t block_b = b[iblock];
        bitmap_block_t tailmask = bitmap_P_tailblock_mask(bits_num);
        block_a &= tailmask;
        block_b &= tailmask;
        if(block_a != block_b)
        {
            return false;
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_END();

    return true;
}

bool bitmap_bitwise_check_inclusion3(
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);

    BITMAP_FOREACH_BLOCK_EXTENDED_BEGIN(iblock, blocks_num)
    {
        bitmap_block_t block_a = a[iblock];
        bitmap_block_t block_b = b[iblock];
        if((block_a | block_b) != block_a)
        {
            return false;
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_LASTBLOCK(iblock, blocks_num)
    {
        bitmap_block_t block_a = a[iblock];
        bitmap_block_t block_b = b[iblock];
        bitmap_block_t tailmask = bitmap_P_tailblock_mask(bits_num);
        block_a &= tailmask;
        block_b &= tailmask;
        if((block_a | block_b) != block_a)
        {
            return false;
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_END();

    return true;
}

bool bitmap_bitwise_check_intersection3(
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);

    BITMAP_FOREACH_BLOCK_EXTENDED_BEGIN(iblock, blocks_num)
    {
        bitmap_block_t block_a = a[iblock];
        bitmap_block_t block_b = b[iblock];
        if((block_a & block_b) != 0)
        {
            return true;
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_LASTBLOCK(iblock, blocks_num)
    {
        bitmap_block_t block_a = a[iblock];
        bitmap_block_t block_b = b[iblock];
        bitmap_block_t tailmask = bitmap_P_tailblock_mask(bits_num);
        block_a &= tailmask;
        block_b &= tailmask;
        if((block_a & block_b) != 0)
        {
            return true;
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_END();

    return false;
}

enum bitmap_relation bitmap_bitwise_check_relation3(
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(bits_num);
    bool equal = true; /* B == A */
    bool inclusion = true; /* B totally included in A */
    bool intersection = false; /* intersection of B and A */

    BITMAP_FOREACH_BLOCK_EXTENDED_BEGIN(iblock, blocks_num)
    {
        bitmap_block_t block_a = a[iblock];
        bitmap_block_t block_b = b[iblock];
        if(block_a != block_b)
        {
            equal = false;
        }

        if((block_a | block_b) != block_a)
        {
            inclusion = false;
        }

        if((block_a & block_b) != 0)
        {
            intersection = true;
        }

        /* The result is known, do not check next */
        if(
                equal == false &&
                inclusion == false &&
                intersection == true
        )
        {
            BITMAP_FOREACH_BLOCK_EXTENDED_BREAK();
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_LASTBLOCK(iblock, blocks_num)
    {
        bitmap_block_t block_a = a[iblock];
        bitmap_block_t block_b = b[iblock];
        bitmap_block_t tailmask = bitmap_P_tailblock_mask(bits_num);
        block_a &= tailmask;
        block_b &= tailmask;

        if(block_a != block_b)
        {
            equal = false;
        }

        if((block_a | block_b) != block_a)
        {
            inclusion = false;
        }

        if((block_a & block_b) != 0)
        {
            intersection = true;
        }
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_END();

    enum bitmap_relation relation =
            equal ? BITMAP_RELATION__EQUAL :
                    inclusion ? BITMAP_RELATION__INCLUSION :
                            intersection ? BITMAP_RELATION__INTERSECTION :
                                    BITMAP_RELATION__DIFFERENT;

    return relation;
}
