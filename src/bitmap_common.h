/**
 * @file bitmap_common.h
 */

#ifndef SRC_BITMAP_COMMON_H_
#define SRC_BITMAP_COMMON_H_

#include <bitmap/bitmap.h>

#ifndef likely
#   define likely(x)   __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#   define unlikely(x) __builtin_expect(!!(x), 0)
#endif

/**
 * @brief Iterate over bitmap blocks
 * @param xiblock         Block index
 * @param xblocks_num     Amount of blocks
 */
#define BITMAP_FOREACH_BLOCK(xiblock, xblocks_num) \
        for( \
                (xiblock) = 0; \
                (xiblock) < (xblocks_num); \
                ++(xiblock) \
        )

/**
 * @brief Iterate over bitmap blocks, extended version to determine the last block
 * @param xiblock         Block index
 * @param xblocks_num     Amount of blocks
 */
#define BITMAP_FOREACH_BLOCK_EXTENDED_BEGIN(xiblock, xblocks_num) \
        do { \
            if(xblocks_num > 0) \
            { \
                size_t xxx_blocks_num_1_xxx = xblocks_num - 1; \
                for(xiblock = 0; xiblock < xxx_blocks_num_1_xxx; ++xiblock) \
                do

/**
 * @brief Processing of last block of bitmap
 * @param xiblock         Block index
 * @param xblocks_num     Amount of blocks
 */
#define BITMAP_FOREACH_BLOCK_EXTENDED_LASTBLOCK(xiblock, xblocks_num) \
                while(0); \
                if(xiblock < xblocks_num) \
                do

/**
 * @brief End of bitmap processing
 */
#define BITMAP_FOREACH_BLOCK_EXTENDED_END() \
                while(0); \
            } \
            goto bitmap_foreach_block_extended_exit; /* unused */ \
            bitmap_foreach_block_extended_exit: ; \
        } while(0)

/**
 * @brief Break from BITMAP_FOREACH_BLOCK_EXTENDED_BEGIN() loop
 */
#define BITMAP_FOREACH_BLOCK_EXTENDED_BREAK() \
        do { \
            goto bitmap_foreach_block_extended_exit; \
        } while(0)

/**
 * @brief Get bit, raised in position <xibit>
 */
#define BITMAP_RAISED_BIT(xibit) \
    ((bitmap_block_t)1 << (xibit))

/**
 * @brief Get significant bits mask of tail block
 * @param bits_num        Amount of bits in bitmap
 * @return Bitmap block mask
 */
static inline bitmap_block_t bitmap_P_tailblock_mask(size_t bits_num)
{
    size_t significant_bits = BITMAP_BITS_IN_TAILBLOCK(bits_num);
    /* build the bitmask */
    return (significant_bits == BITMAP_BITS_IN_BLOCK()) ?
            ( ~(bitmap_block_t)0 ) : /* all set to 1 */
            ( ((bitmap_block_t)1 << significant_bits) - 1 );
}

#endif /* SRC_BITMAP_COMMON_H_ */
