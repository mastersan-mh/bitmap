/**
 * @file bitmap.h
 * @brief API to process the bitmaps
 * @description API to process the bitmaps: operations on a bitmaps and matching of a bitmaps
 */

#ifndef INCLUDE_LIBBITMAP_H_
#define INCLUDE_LIBBITMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#   define BITMAP_RESTRICT __restrict
#else
#   define BITMAP_RESTRICT restrict
#endif

typedef uint8_t bitmap_block_type_t;

/**
 * @brief The bitmaps relation
 */
typedef enum
{
    BITMAP_RELATION__EQUAL,        /**< Bitmaps are equal */
    BITMAP_RELATION__INCLUSION,    /**< B totally included in A */
    BITMAP_RELATION__INTERSECTION, /**< intersection B and A */
    BITMAP_RELATION__DIFFERENT     /**< Bitmaps are different */
} bitmap_relation_t;

/**
 * @brief Amount of bits in one block
 */
#define BITMAP_BLOCK_BITSNUM() (sizeof(bitmap_block_type_t) * 8)

/**
 * @brief Amount of significant bits on the tail. Internal use only!
 *
 * @param xbits_num    Amount of bits in bitmap
 * @return A value in range [0; BITMAP_BLOCK_BITSNUM() - 1]
 */
#define BITMAP_INTERNAL_RESTBITS(xbits_num) ((xbits_num) % BITMAP_BLOCK_BITSNUM())

/**
 * @brief Amount of significant bits in the last block
 *
 * @param xbits_num    Amount of bits in bitmap
 * @return A value in range [1; BITMAP_BLOCK_BITSNUM()]
 */
#define BITMAP_LASTBLOCK_BITSNUM(xbits_num) \
        ( { \
            size_t tmp = BITMAP_INTERNAL_RESTBITS(xbits_num); \
            (tmp == 0) ? BITMAP_BLOCK_BITSNUM() : tmp; \
        } )

/**
 * @brief Amount of blocks in bitmap
 *
 * @param xbits_num    Amount of bits in bitmap
 *
 * @return Amount of blocks
 */
#define BITMAP_BLOCKS_NUM(xbits_num) \
        ( ((xbits_num) / BITMAP_BLOCK_BITSNUM()) + ( (BITMAP_INTERNAL_RESTBITS(xbits_num) > 0) ? 1 : 0) )

/**
 * @brief Amount of bytes in bitmap
 *
 * @param xbits_num    Amount of bits in bitmap
 *
 * @return Amount of bytes
 */
#define BITMAP_BYTES_NUM(xbits_num) (BITMAP_BLOCKS_NUM(xbits_num) * sizeof(bitmap_block_type_t))

/**
 * @brief The main patter to declare the bitmap variable
 *
 * @param xvarname      The variable name
 * @param xbits_num     Amount of bits in bitmap
 */
#define BITMAP_VAR(xvarname, xbits_num) bitmap_block_type_t xvarname[BITMAP_BLOCKS_NUM(xbits_num)]

/**
 * @brief Fill entire bitmap by the value
 * @param bitmap      The bitmap
 * @param bitvalue    The value that will fill
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_set(
        bitmap_block_type_t * bitmap,
        bool bitvalue,
        size_t bits_num
);

/**
 * @brief Copy entire bitmap
 * @param dest        The destination bitmap
 * @param src         The source bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_copy(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
);

/**
 * @brief A bitwize negate
 * @note dest = ~src
 * @param dest        The destination bitmap
 * @param src         The source bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_not2(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
);

/**
 * @brief A bitwize OR, uses 2 arguments
 * @note dest = dest | src
 * @param dest        The destination bitmap
 * @param src         The source bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_or2(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
);

/**
 * @brief A bitwize OR, uses 3 arguments
 * @note dest = a | b
 * @param dest        The destination bitmap
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_or3(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
);

/**
 * @brief A bitwize AND, uses 2 arguments (intersection)
 * @note dest = dest & src
 * @param dest        The destination bitmap
 * @param src         The source bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_and2(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
);

/**
 * @brief A bitwize AND, uses 3 arguments (intersection)
 * @note dest = a & b (intersection of sets)
 * @param dest        The destination bitmap
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_and3(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
);

/**
 * @brief Produces a bitmap subtraction using destination bitmap and source bitmap as arguments and store the result into destination bitmap.
 * @note dest = dest & (~src)
 * @param dest        The destination bitmap
 * @param src         The subtractable Bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_clear2(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT src,
        size_t bits_num
);

/**
 * @brief Produces a bitmap subtraction using <a> bitmap and <b> bitmap as arguments and store the result into <dest> bitmap.
 * @note dest = a & (~b)
 * @param dest        The destination bitmap
 * @param a           The diminished Bitmap
 * @param b           The subtractable Bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_clear3(
        bitmap_block_type_t * BITMAP_RESTRICT dest,
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
);

/**
 * @brief Check, if all bits of bitmap is zero
 * @param bitmap      The bitmap
 * @param bits_num    Amount of bits
 * @param zero        The place to write the check result
 */
bool bitmap_bitwise_check_zero(
        const bitmap_block_type_t *bitmap,
        size_t bits_num
);

/**
 * @brief Check, if bitmaps are equal
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 * @return equal?
 */
bool bitmap_bitwise_check_equal(
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
);

/**
 * @brief Sets inclusion: Check that ALL <b> are inside <a>
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 * @return inclusion?
 */
bool bitmap_bitwise_check_inclusion(
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
);

/**
 * @brief Sets intersection: Check that at least one of <b> is present in <a>
 * @param a              The first bitmap
 * @param b              The second bitmap
 * @param bits_num       Amount of bits
 * @return intersection?
 */
bool bitmap_bitwise_check_intersection(
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
);

/**
 * @brief Check the bitmaps relation: equality, inclusion, intersection or difference
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 * @return relation    The place to write the check result
 */
bitmap_relation_t bitmap_bitwise_check_relation(
        const bitmap_block_type_t * BITMAP_RESTRICT a,
        const bitmap_block_type_t * BITMAP_RESTRICT b,
        size_t bits_num
);

/**
 * @brief Sets particular bit (to 1 or 0, as specified) in given bitmap.
 * @param bitmap      The changed bitmap.
 * @param bits_num    Amount of bits in bitmap.
 * @param bit_index   The bit index in bitmap.
 * @param bitvalue    The value to set the bit (FALSE - 0, TRUE - 1).
 */
void bitmap_bit_set(
        bitmap_block_type_t *bitmap,
        size_t bits_num,
        size_t bit_index,
        bool bitvalue
);

/**
 * @brief Gets particular bit (to 1 or 0, as specified) from given bitmap and stores it's value in placeholder given.
 * @param bitmap      The bitmap.
 * @param bits_num    Amount of bits in bitmap.
 * @param bit_index   The bit index in bitmap.
 * @return bitvalue    The pointer to value to store the bit.
 */
bool bitmap_bit_get(
        const bitmap_block_type_t *bitmap,
        size_t bits_num,
        size_t bit_index
);

typedef struct
{
    bool exist;     /**< Is the bit <bit_index> exist? */
    size_t index;   /**< The index of current bit */
} bitmap_bit_nearest_get_context_t;

/**
 * @brief Internal function to finding "forward" to bitmap iterator by bits, which has value TRUE in a bitmap.
 * @param bitmap            The bitmap.
 * @param bits_num          Amount of bits in bitmap.
 * @param bit_index_from    The bit numer, from which start searching.
 * @param bit_exists        The bit <bit_index> exists.
 * @param bit_index         The nearest current or next set bit.
 */
void bitmap_bit_nearest_forward_raised_get(
        const bitmap_block_type_t *bitmap,
        size_t bits_num,
        size_t bit_index_from,
        bitmap_bit_nearest_get_context_t * bit_nearest
);

/**
 * @brief Type to use in BITMAP_FOREACH_BIT_IN_BITMAP
 */
typedef struct
{
    bitmap_bit_nearest_get_context_t bit; /**< Context */
} bitmap_foreach_bit_context_t;

/**
 * @brief Iterator by bits, which has value TRUE in a bitmap.
 * @param xbit_index      Current bit, which has value TRUE (size_t *).
 * @param xbitmap         The bitmap (const bitmap_block_type_t *).
 * @param xbits_num       Amount of bits in xbitmap (size_t).
 * @param xcontext        Iterator context (bitmap_foreach_bit_context_t *)
 */
#define BITMAP_FOREACH_BIT_IN_BITMAP(xbit_index, xbitmap, xbits_num, xcontext) \
        for( \
                bitmap_bit_nearest_forward_raised_get((xbitmap), (xbits_num), 0                    , &(xcontext)->bit), (*xbit_index) = (xcontext)->bit.index, (xcontext)->bit.index += 1; \
                ((xcontext)->bit.exist); \
                bitmap_bit_nearest_forward_raised_get((xbitmap), (xbits_num), (xcontext)->bit.index, &(xcontext)->bit), (*xbit_index) = (xcontext)->bit.index, (xcontext)->bit.index += 1 \
        )

/**
 * @brief Print the bitmap by the ranges
 * @param dest          Destination string.
 * @param size          Size of destination string.
 * @param bitmap        The source bitmap.
 * @param bits_num      Amount of bits in source bitmap.
 * @param enum_marker   Marker of the enumeration: ", ".
 * @param range_marker  Marker of the range: " - ".
 */
int bitmap_snprintf_ranged(
        char * BITMAP_RESTRICT dest,
        size_t size,
        const bitmap_block_type_t * BITMAP_RESTRICT bitmap,
        size_t bits_num,
        const char * BITMAP_RESTRICT enum_marker,
        const char * BITMAP_RESTRICT range_marker
);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_LIBBITMAP_H_ */
