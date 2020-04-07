/*
 * bitmap.c
 *
 */

#include <bitmap/bitmap.h>

#include "bitmap_common.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/**
 * @brief Выбор наиболее оптимальной функции
 */
#if BITMAP_BLOCK_SIZEOF() == __SIZEOF_SHORT__
#   define POPCOUNT(x)  __builtin_popcount(/* unsigned int */ x)
#elif BITMAP_BLOCK_SIZEOF() == __SIZEOF_INT__
#   define POPCOUNT(x)  __builtin_popcount(/* unsigned int */ x)
#elif BITMAP_BLOCK_SIZEOF() == __SIZEOF_LONG__
#   define POPCOUNT(x)  __builtin_popcountl(/* unsigned long */ x)
#elif BITMAP_BLOCK_SIZEOF() == __SIZEOF_LONG_LONG__
#   define POPCOUNT(x)  __builtin_popcountll(/* unsigned long long */ x)
#else /* BITMAP_BLOCK_SIZEOF() == 1 byte */
#   define POPCOUNT(x)  __builtin_popcount(/* unsigned int */ x)
#endif

size_t bitmap_bitwise_power2(
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t size
)
{
    size_t blocks_num = BITMAP_BITS_TO_BLOCKS_ALIGNED(size);
    size_t iblock;
    size_t power = 0;
    bitmap_block_t block;

    BITMAP_FOREACH_BLOCK_EXTENDED_BEGIN(iblock, blocks_num)
    {
        block = src[iblock];
        power += POPCOUNT(block);
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_LASTBLOCK(iblock, blocks_num)
    {
        block = src[iblock];
        bitmap_block_t mask = bitmap_P_tailblock_mask(size);
        block &= mask;
        power += POPCOUNT(block);
    }
    BITMAP_FOREACH_BLOCK_EXTENDED_END();

    return power;
}

void bitmap_bitwise_power6(
        const bitmap_block_t * BITMAP_RESTRICT srcA,
        size_t sizeA,
        const bitmap_block_t * BITMAP_RESTRICT srcB,
        size_t sizeB,
        size_t * BITMAP_RESTRICT power_intersection,
        size_t * BITMAP_RESTRICT power_union
)
{
    size_t power_isect_tmp = 0;
    size_t power_union_tmp = 0;

    struct map
    {
        const bitmap_block_t * bitmap;
        size_t size_bits;
        size_t size_blocks;
    };

    struct map mapS; /**< Short bitmap */
    struct map mapL; /**< Long bitmap */

    if(sizeA < sizeB)
    {
        mapS.bitmap = srcA;
        mapS.size_bits = sizeA;
        mapS.size_blocks = BITMAP_BITS_TO_BLOCKS_ALIGNED(sizeA);
        mapL.bitmap = srcB;
        mapL.size_bits = sizeB;
        mapL.size_blocks = BITMAP_BITS_TO_BLOCKS_ALIGNED(sizeB);
    }
    else
    {
        mapS.bitmap = srcB;
        mapS.size_bits = sizeB;
        mapS.size_blocks = BITMAP_BITS_TO_BLOCKS_ALIGNED(sizeB);
        mapL.bitmap = srcA;
        mapL.size_bits = sizeA;
        mapL.size_blocks = BITMAP_BITS_TO_BLOCKS_ALIGNED(sizeA);
    }

    bitmap_block_t blockS;
    bitmap_block_t blockL;
    bitmap_block_t bisect;
    bitmap_block_t bunion;
    bitmap_block_t mask;
    size_t iblock = 0;

    /* begin, common part of S and L, no tail block */
    if(mapS.size_blocks > 0)
    {
        size_t S_size_blocks = mapS.size_blocks - 1;
        for(; iblock < S_size_blocks; ++iblock)
        {
            blockS = mapS.bitmap[iblock];
            blockL = mapL.bitmap[iblock];

            bisect = blockS & blockL;
            bunion = blockS | blockL;

            power_isect_tmp += POPCOUNT(bisect);
            power_union_tmp += POPCOUNT(bunion);
        }

        /* tail blocks of S and L */
        if(iblock < mapS.size_blocks)
        {
            blockS = mapS.bitmap[iblock];
            blockL = mapL.bitmap[iblock];

            mask = bitmap_P_tailblock_mask(mapS.size_bits);
            blockS &= mask;

            /* Если количество блоков в обеих картах одинаковое,
             * маскируем так же и блок карты L, потому что это последний блок в L
             */
            if(mapL.size_blocks == mapS.size_blocks)
            {
                mask = bitmap_P_tailblock_mask(mapL.size_bits);
                blockL &= mask;
            }

            bisect = blockS & blockL;
            bunion = blockS | blockL;

            power_isect_tmp += POPCOUNT(bisect);
            power_union_tmp += POPCOUNT(bunion);
        }
        ++iblock;
    }

    /* rest */
    if(mapL.size_blocks > 0)
    {
        size_t L_size_blocks = mapL.size_blocks - 1;
        for(; iblock < L_size_blocks; ++iblock)
        {
            blockL = mapL.bitmap[iblock];

            bunion = blockL;

            power_union_tmp += POPCOUNT(bunion);
        }

        /* tail block of L */
        if(iblock < mapL.size_blocks)
        {
            blockL = mapL.bitmap[iblock];

            bitmap_block_t mask;
            mask = bitmap_P_tailblock_mask(mapL.size_bits);
            blockL &= mask;

            bunion = blockL;

            power_union_tmp += POPCOUNT(bunion);
        }
    }

    (*power_intersection) = power_isect_tmp;
    (*power_union) = power_union_tmp;
}
