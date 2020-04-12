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

#define BITMAP_VISIBILITY_DEFAULT   __attribute__((visibility ("default")))
#define BITMAP_VISIBILITY_HIDDEN    __attribute__((visibility ("hidden")))
#define BITMAP_VISIBILITY_INTERNAL  __attribute__((visibility ("internal")))
#define BITMAP_VISIBILITY_PROTECTED __attribute__((visibility ("protected")))

/** @brief BITMAP_BUILDING defined in Makefile, and mean that we build the library. */
#ifdef BITMAP_BUILDING
#   define BITMAP_PUBLIC  BITMAP_VISIBILITY_DEFAULT
#else
#   define BITMAP_PUBLIC  BITMAP_VISIBILITY_DEFAULT
#endif

/** @brief Aliasing optimization */
#ifdef __cplusplus
#   define BITMAP_RESTRICT __restrict
#else
#   define BITMAP_RESTRICT restrict
#endif

/** @brief Internal use: Type of bitmap block, helper, Not to direct using */
#define BITMAP_BLOCK_TYPE_HELPER(xbits)  uint  ## xbits ## _t
/** @brief Internal use: Type of bitmap block */
#define BITMAP_BLOCK_TYPE(xbits)  BITMAP_BLOCK_TYPE_HELPER(xbits)

/** @brief Internal use: Amount of bits in single block */
#define BITMAP_BITS_IN_BLOCK_DEFINE  64

/** @brief Amount of bits in one byte */
#define BITMAP_BITS_IN_BYTE()  (8)

/** @brief Internal use: Size of block */
#define BITMAP_BLOCK_SIZEOF()  (BITMAP_BITS_IN_BLOCK_DEFINE / BITMAP_BITS_IN_BYTE() /* bytes */)

/** @brief The bitmap block */
typedef BITMAP_BLOCK_TYPE(BITMAP_BITS_IN_BLOCK_DEFINE) bitmap_block_t;

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

typedef struct
{
    size_t begin;
    size_t end;
} bitmap_range_t;

/** @brief Amount of bytes in one block */
#define BITMAP_BYTES_IN_BLOCK()  (sizeof(bitmap_block_t))

/** @brief Amount of bits in one block */
#define BITMAP_BITS_IN_BLOCK()  (BITMAP_BYTES_IN_BLOCK() * BITMAP_BITS_IN_BYTE())

/**
 * @brief Amount of significant bits on the tail. Internal use only!
 *
 * @param xbits_num    Amount of bits in bitmap
 * @return A value in range [0; BITMAP_BITS_IN_BLOCK() - 1]
 */
#define BITMAP_BITS_IN_LASTBLOCK(xbits_num)  ((xbits_num) % BITMAP_BITS_IN_BLOCK())

/**
 * @brief Amount of significant bits in the last block
 *
 * @param xbits_num    Amount of bits in bitmap
 * @return A value in range [1; BITMAP_BITS_IN_BLOCK()]
 */
#define BITMAP_BITS_IN_TAILBLOCK(xbits_num) \
        (\
            (xbits_num) == 0 ? 0 : \
            ( { \
                size_t tmp = BITMAP_BITS_IN_LASTBLOCK(xbits_num); \
                (tmp == 0) ? BITMAP_BITS_IN_BLOCK() : tmp; \
            } ) \
        )

/**
 * @brief Amount of blocks in bitmap
 *
 * @param xbits_num    Amount of bits in bitmap
 *
 * @return Amount of blocks, aligned to blocksize
 */
#define BITMAP_BITS_TO_BLOCKS_ALIGNED(xbits_num) \
        ( \
                (xbits_num) == 0 ? 0 : \
                        ( (xbits_num) / BITMAP_BITS_IN_BLOCK() ) + ( BITMAP_BITS_IN_LASTBLOCK(xbits_num) == 0 ? 0 : 1 ) \
        )

/**
 * @brief Amount of bytes in bitmap aligned to blocksize
 * @param xbits_num    Amount of bits in bitmap
 * @return Amount of bytes
 */
#define BITMAP_BITS_TO_BYTES_ALIGNED(xbits_num) \
        ( BITMAP_BITS_TO_BLOCKS_ALIGNED(xbits_num) * BITMAP_BYTES_IN_BLOCK() )

/**
 * @brief Amount of bits in bitmap aligned to blocksize
 * @param xbits_num    Amount of blocks in bitmap
 * @return Amount of bits
 */
#define BITMAP_BLOCKS_TO_BITS_ALIGNED(xblocks_num) \
        ((xblocks_num) * BITMAP_BITS_IN_BLOCK())

/**
 * @brief Amount of bits in bytes, aligned to byte
 * @return Amount of bits
 */
#define BITMAP_BYTES_TO_BITS(xbytes_num) \
        ((xbytes_num) * BITMAP_BITS_IN_BYTE())

/**
 * @brief The main patter to define the static or auto bitmap variable
 * @param xvarname      The variable name
 * @param xbits_num     Amount of bits in bitmap
 */
#define BITMAP_VAR(xvarname, xbits_num) \
    bitmap_block_t xvarname[BITMAP_BITS_TO_BLOCKS_ALIGNED(xbits_num)]

struct bitmap_version
{
    const char * hash;
    const char * date_time;
    const char * cflags;
};

/**
 * @brief Get library version
 * @param return hash and date-time of compilation
 */
const struct bitmap_version * bitmap_version0(void) BITMAP_PUBLIC;

/**
 * @brief Fill entire bitmap by the value 1
 * @param bitmap      The bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_raise1(
        bitmap_block_t * bitmap,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief Fill entire bitmap by the value 0
 * @param bitmap      The bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_clear2(
        bitmap_block_t * bitmap,
        size_t bits_num
) BITMAP_PUBLIC;

void bitmap_bitwise_range_raise2(
        bitmap_block_t * bitmap,
        const bitmap_range_t * range
) BITMAP_PUBLIC;

void bitmap_bitwise_range_clear2(
        bitmap_block_t * bitmap,
        const bitmap_range_t * range
) BITMAP_PUBLIC;

/**
 * @brief Copy entire bitmap
 * @param dest        The destination bitmap
 * @param src         The source bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_copy3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief A bitwize negate
 * @note dest = ~src
 * @param dest        The destination bitmap
 * @param src         The source bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_not3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief A bitwize OR, uses 2 arguments
 * @note dest = dest | src
 * @param dest        The destination bitmap
 * @param src         The source bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_or3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief A bitwize OR, uses 3 arguments
 * @note dest = a | b
 * @param dest        The destination bitmap
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_or4(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief A bitwize AND, uses 2 arguments (intersection)
 * @note dest = dest & src
 * @param dest        The destination bitmap
 * @param src         The source bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_and3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief A bitwize AND, uses 4 arguments (intersection)
 * @note dest = a & b (intersection of sets)
 * @param dest        The destination bitmap
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_and4(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief Produces a bitmap subtraction using destination bitmap and source bitmap as arguments and store the result into destination bitmap.
 * @note dest = dest & (~src)
 * @param dest        The destination bitmap
 * @param src         The subtractable Bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_clear3(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief Produces a bitmap subtraction using `<a>` bitmap
 *        and `<b>` bitmap as arguments and store the result into `<dest>` bitmap.
 * @note dest = a & (~b)
 * @param dest        The destination bitmap
 * @param a           The diminished Bitmap
 * @param b           The subtractable Bitmap
 * @param bits_num    Amount of bits
 */
void bitmap_bitwise_clear4(
        bitmap_block_t * BITMAP_RESTRICT dest,
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief Power of bitmap (amount of raised bits)
 */
size_t bitmap_bitwise_power2(
        const bitmap_block_t * BITMAP_RESTRICT src,
        size_t size
) BITMAP_PUBLIC;

/**
 * @brief Power of union and intersection of srcA and srcB
 */
void bitmap_bitwise_power6(
        const bitmap_block_t * BITMAP_RESTRICT srcA,
        size_t sizeA,
        const bitmap_block_t * BITMAP_RESTRICT srcB,
        size_t sizeB,
        size_t * BITMAP_RESTRICT power_intersection,
        size_t * BITMAP_RESTRICT power_union
) BITMAP_PUBLIC;

/**
 * @brief Check, if all bits of bitmap is zero
 * @param bitmap      The bitmap
 * @param bits_num    Amount of bits
 * @param zero        The place to write the check result
 */
bool bitmap_bitwise_check_zero2(
        const bitmap_block_t * bitmap,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief Check, if bitmaps are equal
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 * @return equal?
 */
bool bitmap_bitwise_check_equal3(
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief Sets inclusion: Check that ALL <b> are inside <a>
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 * @return inclusion?
 */
bool bitmap_bitwise_check_inclusion3(
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief Sets intersection: Check that at least one of <b> is present in <a>
 * @param a              The first bitmap
 * @param b              The second bitmap
 * @param bits_num       Amount of bits
 * @return intersection?
 */
bool bitmap_bitwise_check_intersection3(
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief Check the bitmaps relation: equality, inclusion, intersection or difference
 * @param a           The first bitmap
 * @param b           The second bitmap
 * @param bits_num    Amount of bits
 * @return relation    The place to write the check result
 */
bitmap_relation_t bitmap_bitwise_check_relation3(
        const bitmap_block_t * BITMAP_RESTRICT a,
        const bitmap_block_t * BITMAP_RESTRICT b,
        size_t bits_num
) BITMAP_PUBLIC;

/**
 * @brief Sets particular bit to 1 in given bitmap.
 * @param bitmap      The changed bitmap.
 * @param bit_index   The bit index in bitmap.
 */
void bitmap_bit_raise2(
        bitmap_block_t *bitmap,
        size_t bit_index
) BITMAP_PUBLIC;

/**
 * @brief Sets particular bit to 0 in given bitmap.
 * @param bitmap      The changed bitmap.
 * @param bit_index   The bit index in bitmap.
 */
void bitmap_bit_clear2(
        bitmap_block_t *bitmap,
        size_t bit_index
) BITMAP_PUBLIC;

/**
 * @brief Gets particular bit (to 1 or 0, as specified) from given bitmap and stores it's value in placeholder given.
 * @param bitmap      The bitmap.
 * @param bits_num    Amount of bits in bitmap.
 * @param bit_index   The bit index in bitmap.
 * @return bitvalue    The pointer to value to store the bit.
 */
bool bitmap_bit_get2(
        const bitmap_block_t *bitmap,
        size_t bit_index
) BITMAP_PUBLIC;

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
void bitmap_bit_nearest_forward_raised_get4(
        const bitmap_block_t *bitmap,
        size_t bits_num,
        size_t bit_index_from,
        bitmap_bit_nearest_get_context_t * bit_nearest
) BITMAP_PUBLIC;

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
 * @param xbitmap         The bitmap (const bitmap_block_t *).
 * @param xbits_num       Amount of bits in xbitmap (size_t).
 * @param xcontext        Iterator context (bitmap_foreach_bit_context_t *)
 */
#define BITMAP_FOREACH_BIT_IN_BITMAP(xbit_index, xbitmap, xbits_num, xcontext) \
        for( \
                bitmap_bit_nearest_forward_raised_get4((xbitmap), (xbits_num), 0                    , &(xcontext)->bit), (*xbit_index) = (xcontext)->bit.index, (xcontext)->bit.index += 1; \
                ((xcontext)->bit.exist); \
                bitmap_bit_nearest_forward_raised_get4((xbitmap), (xbits_num), (xcontext)->bit.index, &(xcontext)->bit), (*xbit_index) = (xcontext)->bit.index, (xcontext)->bit.index += 1 \
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
int bitmap_snprintf_ranged6(
        char * BITMAP_RESTRICT dest,
        size_t size,
        const bitmap_block_t * BITMAP_RESTRICT bitmap,
        size_t bits_num,
        const char * BITMAP_RESTRICT enum_marker,
        const char * BITMAP_RESTRICT range_marker
) BITMAP_PUBLIC;

/**
 * @brief Scan the string of ranges and append raised bits it to the bitmap
 * @param bitmap        The source bitmap.
 * @param bits_num      Amount of bits in source bitmap.
 * @param enum_marker   Marker of the enumeration: ",".
 * @param range_marker  Marker of the range: "-".
 * @param src           Source string.
 * @note Example: src = "0 - 5, 7"
 */
int bitmap_sscanf_append_ranged5(
        bitmap_block_t * BITMAP_RESTRICT bitmap,
        size_t bits_num,
        char enum_marker,
        char range_marker,
        const char * BITMAP_RESTRICT src
) BITMAP_PUBLIC;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_LIBBITMAP_H_ */
