/*
 * @file bitmap4096.h
 * @brief Implementation of the functions of bitmap size of 4096 bits.
 */

#ifndef INCLUDE_BITMAP4096_H_
#define INCLUDE_BITMAP4096_H_

#include <bitmap/bitmap.h>

#ifdef __cplusplus
extern "C" {
#endif


#define BITMAP4096_BITS_NUM 4096
typedef struct
{
    BITMAP_VAR(data, BITMAP4096_BITS_NUM);
} bitmap4096_t;

static inline void bitmap4096_raise1(bitmap4096_t * bitmap)
{
    bitmap_bitwise_raise1(bitmap->data, BITMAP4096_BITS_NUM);
}

static inline void bitmap4096_clear1(bitmap4096_t * bitmap)
{
    bitmap_bitwise_clear2(bitmap->data, BITMAP4096_BITS_NUM);
}

static inline void bitmap4096_copy2(
        bitmap4096_t * BITMAP_RESTRICT bitmap_dest,
        const bitmap4096_t * BITMAP_RESTRICT bitmap_src
)
{
    bitmap_bitwise_copy3(bitmap_dest->data, bitmap_src->data, BITMAP4096_BITS_NUM);
}

static inline void bitmap4096_bitwise_or2(
        bitmap4096_t * BITMAP_RESTRICT bitmap_dest,
        const bitmap4096_t * BITMAP_RESTRICT bitmap_src
)
{
    bitmap_bitwise_or3(bitmap_dest->data, bitmap_src->data, BITMAP4096_BITS_NUM);
}

static inline void bitmap4096_bitwise_or3(
        bitmap4096_t * BITMAP_RESTRICT bitmap_dest,
        const bitmap4096_t * BITMAP_RESTRICT bitmap_a,
        const bitmap4096_t * BITMAP_RESTRICT bitmap_b
)
{
    bitmap_bitwise_or4(
            bitmap_dest->data,
            bitmap_a->data,
            bitmap_b->data,
            BITMAP4096_BITS_NUM
    );
}

static inline void bitmap4096_bitwise_clear2(
        bitmap4096_t * BITMAP_RESTRICT bitmap_dest,
        const bitmap4096_t * BITMAP_RESTRICT bitmap_src
)
{
    bitmap_bitwise_clear3(bitmap_dest->data, bitmap_src->data, BITMAP4096_BITS_NUM);
}

static inline void bitmap4096_bitwise_clear3(
        bitmap4096_t * BITMAP_RESTRICT bitmap_dest,
        const bitmap4096_t * BITMAP_RESTRICT bitmap_a,
        const bitmap4096_t * BITMAP_RESTRICT bitmap_b
)
{
    bitmap_bitwise_clear4(
            bitmap_dest->data,
            bitmap_a->data,
            bitmap_b->data,
            BITMAP4096_BITS_NUM
    );
}

/**
 * @brief Intersection
 */
static inline void bitmap4096_bitwise_and3(
        bitmap4096_t * BITMAP_RESTRICT dest,
        const bitmap4096_t * BITMAP_RESTRICT a,
        const bitmap4096_t * BITMAP_RESTRICT b
)
{
    bitmap_bitwise_and4(dest->data, a->data, b->data, BITMAP4096_BITS_NUM);
}

static inline bool bitmap4096_bitwise_check_zero1(
        const bitmap4096_t * bitmap
)
{
    return bitmap_bitwise_check_zero2(bitmap->data, BITMAP4096_BITS_NUM);
}

static inline bool bitmap4096_bitwise_check_equal2(
        const bitmap4096_t * BITMAP_RESTRICT bitmap1,
        const bitmap4096_t * BITMAP_RESTRICT bitmap2
)
{
    return bitmap_bitwise_check_equal3(bitmap1->data, bitmap2->data, BITMAP4096_BITS_NUM);
}

static inline bool bitmap4096_bitwise_check_inclusion2(
        const bitmap4096_t * BITMAP_RESTRICT bitmap1,
        const bitmap4096_t * BITMAP_RESTRICT bitmap2
)
{
    return bitmap_bitwise_check_inclusion3(bitmap1->data, bitmap2->data, BITMAP4096_BITS_NUM);
}

static inline bool bitmap4096_check_intersection2(
        const bitmap4096_t * BITMAP_RESTRICT bitmap1,
        const bitmap4096_t * BITMAP_RESTRICT bitmap2
)
{
    return bitmap_bitwise_check_intersection3(bitmap1->data, bitmap2->data, BITMAP4096_BITS_NUM);
}

static inline bitmap_relation_t bitmap4096_bitwise_check_relation2(
        const bitmap4096_t * BITMAP_RESTRICT bitmap1,
        const bitmap4096_t * BITMAP_RESTRICT bitmap2
)
{
    return bitmap_bitwise_check_relation3(bitmap1->data, bitmap2->data, BITMAP4096_BITS_NUM);
}

static inline void bitmap4096_bit_raise2(
        bitmap4096_t * bitmap,
        size_t bit_index
)
{
    bitmap_bit_raise2(bitmap->data, bit_index);
}

static inline void bitmap4096_bit_clear2(
        bitmap4096_t * bitmap,
        size_t bit_index
)
{
    bitmap_bit_clear2(bitmap->data, bit_index);
}

static inline bool bitmap4096_bit_get2(
        const bitmap4096_t * bitmap,
        size_t bit_index
)
{
    return bitmap_bit_get2(bitmap->data, bit_index);
}

static inline int bitmap4096_snprintf_ranged5(
        char * BITMAP_RESTRICT dest,
        size_t size,
        const bitmap4096_t * BITMAP_RESTRICT bitmap,
        const char * BITMAP_RESTRICT enum_marker,
        const char * BITMAP_RESTRICT range_marker
)
{
    return bitmap_snprintf_ranged6(
            dest,
            size,
            bitmap->data,
            BITMAP4096_BITS_NUM,
            enum_marker,
            range_marker
    );
}

/**
 * @brief Type to use in BITMAP_FOREACH_BIT_IN_BITMAP
 */
typedef struct
{
    bitmap_foreach_bit_context_t bit; /**< to more strict type */
} bitma4096p_foreach_bit_context_t;

/**
 * @brief Iterator by bits, which has value TRUE in a bitmap.
 *
 * @param xbit_index      Current bit, which has value TRUE (size_t *).
 * @param xbitmap         The bitmap (const bitmap_block_t *).
 * @param xcontext        Iterator context (bitmap_foreach_bit_context_t *)
 */
#define BITMAP4096_FOREACH_BIT_IN_BITMAP(xbit_index, xbitmap, xcontext) \
         BITMAP_FOREACH_BIT_IN_BITMAP(xbit_index, xbitmap, BITMAP4096_BITS_NUM, &((xcontext)->bit))

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_BITMAP4096_H_ */
