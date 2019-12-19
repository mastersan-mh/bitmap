/**
 * @file bitmap.c
 * @brief Implementation of the bitmap functions.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include <bitmap/bitmap.h>

#include <string.h>

#define CHECK_VALID(xexpr, xres) do{ if(!(xexpr)) return xres; }while(0)

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
 * @param xislast         Is the block is last?
 * @param xblocks_num     Amount of blocks
 */
#define BITMAPS_FOREACH_BLOCK_EXTENDED(xiblock, xislast, xblocks_num) \
        for( \
                (xiblock) = 0, (xislast) = (1 == (xblocks_num)); \
                (xiblock) < (xblocks_num); \
                ++(xiblock), (xislast) = ((xiblock) + 1 == (xblocks_num)) \
        )

/**
 * @brief Get significant bits mask
 * @param bits_num        Amount of bits in bitmap
 * @return Bitmap block mask
 */
static inline bitmap_block_type_t P_significant_bits_mask_get(size_t bits_num)
{
    size_t significant_bits = BITMAP_LASTBLOCK_BITSNUM(bits_num);
    /* build the bitmask */
    return (significant_bits == BITMAP_BLOCK_BITSNUM()) ?
            ( ~(bitmap_block_type_t)0 ) : /* all set to 1 */
            ( ((bitmap_block_type_t)1 << significant_bits) - 1 );
}

void bitmap_bitwise_set(
        bitmap_block_type_t *bitmap,
        bool bitvalue,
        size_t bits_num
)
{
    memset(bitmap, (bitvalue ? -1 : 0), BITMAP_BYTES_NUM(bits_num));
}

void bitmap_bitwise_copy(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    memcpy(dest, src, BITMAP_BYTES_NUM(bits_num));
}

void bitmap_bitwise_not2(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] = ~src[iblock];
    }
}

void bitmap_bitwise_or2(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] |= src[iblock];
    }
}

void bitmap_bitwise_or3(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] = a[iblock] | b[iblock];
    }
}

void bitmap_bitwise_and2(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] &= src[iblock];
    }
}

void bitmap_bitwise_and3(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] = a[iblock] & b[iblock];
    }
}

void bitmap_bitwise_clear2(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] &= ~src[iblock];
    }
}

void bitmap_bitwise_clear3(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAP_FOREACH_BLOCK(iblock, blocks_num)
    {
        dest[iblock] = a[iblock] & ~b[iblock];
    }
}

bool bitmap_bitwise_check_zero(
        const bitmap_block_type_t *bitmap,
        size_t bits_num
)
{
    size_t iblock;
    bool isLast;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAPS_FOREACH_BLOCK_EXTENDED(iblock, isLast, blocks_num)
    {
        bitmap_block_type_t item;
        item = bitmap[iblock];
        if(isLast)
        {
            bitmap_block_type_t tailmask = P_significant_bits_mask_get(bits_num);
            item &= tailmask;
        }

        if(item != 0)
        {
            return false;
        }
    }

    return true;
}

bool bitmap_bitwise_check_equal(
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    bool isLast;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAPS_FOREACH_BLOCK_EXTENDED(iblock, isLast, blocks_num)
    {
        bitmap_block_type_t item_a = a[iblock];
        bitmap_block_type_t item_b = b[iblock];

        if(isLast)
        {
            bitmap_block_type_t tailmask = P_significant_bits_mask_get(bits_num);
            item_a &= tailmask;
            item_b &= tailmask;
        }

        if(item_a != item_b)
        {
            return false;
        }
    }

    return true;
}

bool bitmap_bitwise_check_inclusion(
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    bool isLast;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAPS_FOREACH_BLOCK_EXTENDED(iblock, isLast, blocks_num)
    {
        bitmap_block_type_t item_a = a[iblock];
        bitmap_block_type_t item_b = b[iblock];
        if(isLast)
        {
            bitmap_block_type_t tailmask = P_significant_bits_mask_get(bits_num);
            item_a &= tailmask;
            item_b &= tailmask;
        }

        if((item_a | item_b) != item_a)
        {
            return false;
        }

    }
    return true;
}

bool bitmap_bitwise_check_intersection(
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    bool isLast;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAPS_FOREACH_BLOCK_EXTENDED(iblock, isLast, blocks_num)
    {
        bitmap_block_type_t item_a = a[iblock];
        bitmap_block_type_t item_b = b[iblock];
        if(isLast)
        {
            bitmap_block_type_t tailmask = P_significant_bits_mask_get(bits_num);
            item_a &= tailmask;
            item_b &= tailmask;
        }

        if((item_a & item_b) != 0)
        {
            return true;
        }
    }

    return false;
}

bitmap_relation_t bitmap_bitwise_check_relation(
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
)
{
    size_t iblock;
    bool equal = true; /* B == A */
    bool inclusion = true; /* B totally included in A */
    bool intersection = false; /* intersection B and A */
    bool isLast;
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    BITMAPS_FOREACH_BLOCK_EXTENDED(iblock, isLast, blocks_num)
    {
        bitmap_block_type_t item_a = a[iblock];
        bitmap_block_type_t item_b = b[iblock];
        if(isLast)
        {
            bitmap_block_type_t tailmask = P_significant_bits_mask_get(bits_num);
            item_a &= tailmask;
            item_b &= tailmask;
        }

        if(item_a != item_b)
        {
            equal = false;
        }

        if((item_a | item_b) != item_a)
        {
            inclusion = false;
        }

        if((item_a & item_b) != 0)
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
            break;
        }
    }

    bitmap_relation_t relation =
            equal ? BITMAP_RELATION__EQUAL :
                    inclusion ? BITMAP_RELATION__INCLUSION :
                            intersection ? BITMAP_RELATION__INTERSECTION :
                                    BITMAP_RELATION__DIFFERENT;

    return relation;
}

void bitmap_bit_set(
        bitmap_block_type_t *bitmap,
        size_t bits_num,
        size_t bit_index,
        bool bitvalue
)
{
    CHECK_VALID(bit_index < bits_num, );

    bitmap_block_type_t *block = &bitmap[bit_index / BITMAP_BLOCK_BITSNUM()];
    bitmap_block_type_t bit = 1 << (bit_index % BITMAP_BLOCK_BITSNUM());
    if(bitvalue)
    {
        *block |= bit;
    }
    else
    {
        *block &= ~bit;
    }
}

bool bitmap_bit_get(
        const bitmap_block_type_t *bitmap,
        size_t bits_num,
        size_t bit_index
)
{
    CHECK_VALID(bit_index < bits_num, false);

    bitmap_block_type_t block = bitmap[bit_index / BITMAP_BLOCK_BITSNUM()];
    bitmap_block_type_t bit = 1 << (bit_index % BITMAP_BLOCK_BITSNUM());
    return ((block & bit) != 0);
}

/**
 * @brief Добавление форматной строки с аргументами к строке с проверкой размера
 * @param[i/o] str_ptr		Указатель на строку, к которой происходит добавление. Модифицируется! (char *)
 * @param[i/o] str_rest		Сколько свобоных символов осталось в str_ptr. Модифицируется! (ssize_t)
 * @return len				>= 0 - Записаная длина; < 0 - согласно спецификации snprintf (сколько не хватило для записи?) (ssize_t)
 */
#define STR_SNPRINTF_PUSH(str_ptr, str_rest, format, ...) \
        ({ \
            ssize_t len = snprintf((str_ptr), (str_rest), format, ##__VA_ARGS__); \
            if(len > 0) \
            { \
                (str_ptr) += len; \
                (str_rest) -= len; \
            } \
            (str_rest) < 0 ? (str_rest) : (len); \
        })

typedef struct
{
    ssize_t begin;
    ssize_t end;
} bitmap_range_t;

/**
 * @breif Print the single range
 * @param first         Is the first range?
 * @param dest          Destination string.
 * @param rest          Rest of the string.
 * @param range         Range.
 * @param enum_marker   Marker of the enumeration: ", ".
 * @param range_marker  Marker of the range: " - ".
 */
static int P_snprintf_range(
        bool first,
        char ** BITMAP_RESTRICT dest,
        ssize_t * BITMAP_RESTRICT rest,
        const bitmap_range_t * BITMAP_RESTRICT range,
        const char * BITMAP_RESTRICT enum_marker,
        const char * BITMAP_RESTRICT range_marker
)
{
    if(!first)
    {
        STR_SNPRINTF_PUSH(*dest, *rest, "%s", enum_marker);
        if((*rest) <= 0)
        {
            return -1;
        }
    }

    if(range->begin == range->end)
    {
        STR_SNPRINTF_PUSH(*dest, *rest, "%d", (int)range->begin);
    }
    else
    {
        STR_SNPRINTF_PUSH(
                *dest,
                *rest,
                "%d%s%d",
                (int)range->begin,
                ((range->begin + 1 == range->end) ? enum_marker : range_marker),
                (int)range->end
        );
    }
    if((*rest) <= 0)
    {
        return -1;
    }
    return 0;
}

void bitmap_bit_nearest_forward_raised_get(
        const bitmap_block_type_t *bitmap,
        size_t bits_num,
        size_t bit_index_from,
        bitmap_bit_nearest_get_context_t * bit_nearest
)
{
    size_t blocks_num = BITMAP_BLOCKS_NUM(bits_num);
    size_t iblock;
    size_t bit_index_tmp;
    for(
            iblock = bit_index_from / BITMAP_BLOCK_BITSNUM(), bit_index_tmp = bit_index_from;
            iblock < blocks_num;
            ++iblock                                        , bit_index_tmp = iblock * BITMAP_BLOCK_BITSNUM()
    )
    {
        const bitmap_block_type_t block = bitmap[iblock];
        if(block == 0) /* skip zero block */
        {
            continue;
        }

        bool last = (iblock + 1 == blocks_num);
        size_t blockbits = last ?
                BITMAP_LASTBLOCK_BITSNUM(bits_num) :
                BITMAP_BLOCK_BITSNUM();

        uint8_t iblockbit;
        for(
                iblockbit = bit_index_tmp % BITMAP_BLOCK_BITSNUM();
                iblockbit < blockbits;
                ++iblockbit
        )
        {
            bitmap_block_type_t bit = 1 << iblockbit;
            bool found = ((block & bit) != 0);
            if(found)
            {
                bit_nearest->index = (iblock * BITMAP_BLOCK_BITSNUM() + iblockbit);
                bit_nearest->exist = true;
                return;
            }
        }
    }

    bit_nearest->exist = false;
}

int bitmap_snprintf_ranged(
        char * BITMAP_RESTRICT dest,
        size_t size,
        const bitmap_block_type_t * BITMAP_RESTRICT bitmap,
        size_t bits_num,
        const char * BITMAP_RESTRICT enum_marker,
        const char * BITMAP_RESTRICT range_marker
)
{
    int res = 0;
    size_t bits_count;

    char * bits_str_ptr = dest;
    ssize_t rest = size;

    /* protect */
    if(size == 0)
    {
        return 0;
    }
    bits_str_ptr[0] = '\0';

    bitmap_range_t range = {-1, -1};
    bool first = true;
    bool range_interrupted = false;

    ssize_t bit_prev = -1;
    size_t bit_index;
    bitmap_foreach_bit_context_t ctx;
    BITMAP_FOREACH_BIT_IN_BITMAP(&bit_index, bitmap, bits_num, &ctx)
    {

        if(bit_prev == -1)
        {
            range.begin = bit_index;
        }
        else
        {
            range_interrupted = (bit_prev + 1 != (ssize_t)bit_index);
        }

        if(!range_interrupted)
        {
            range.end = bit_index;
        }
        else
        {
            res = P_snprintf_range(first, &bits_str_ptr, &rest, &range, enum_marker, range_marker);
            if(res) goto end;

            range.begin = bit_index;
            range.end = bit_index;
            first = false;
        }

        bit_prev = bit_index;
        ++bits_count;
    }

    if(range.begin >= 0 && range.end >= 0)
    {
        res = P_snprintf_range(first, &bits_str_ptr, &rest, &range, enum_marker, range_marker);
        if(res) goto end;
    }

    end:

    if(size > 0)
    {
        dest[size - 1] = '\0';
    }

    return res;
}
