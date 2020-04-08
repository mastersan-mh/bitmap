/**
 * @file test_bitmap.cpp
 *
 */

#include <bitmap/bitmap.h>
#include <bitmap/bitmap4096.h>

#include <catch/catch.hpp>

#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define BITMAP_SIZE3 (3)
#define BITMAP_SIZE128 (64 + 64)
#define BITMAP_SIZE133 (64 + 64 + 5)
#define BITMAP_SIZE64 (64)
#define BITMAP_SIZE67 (64 + 3)

static void P_prepare_fill_0(
        bitmap_block_t *bitmap,
        size_t bits_num
)
{
    bitmap_bitwise_clear2(bitmap, bits_num);
}

static void P_prepare_fill_0_trashed(
        bitmap_block_t *bitmap,
        size_t bits_num
)
{
    /* Сделаем так, чтобы хвостовые биты були установлены в 1, но они не должны учитываться при сравнении */
    bitmap_bitwise_raise1(bitmap, bits_num);

    size_t i;
    for(i = 0; i < bits_num; ++i)
    {
        bitmap_bit_clear2(bitmap, i);
    }
}

/**
 * @brief Make pattern 1010101010...
 */
static void P_prepare_fill_55(
        bitmap_block_t *bitmap,
        size_t bits_num
)
{
    bitmap_bitwise_clear2(bitmap, bits_num);
    size_t i;
    for(i = 0; i < bits_num; i += 2)
    {
        bitmap_bit_raise2(bitmap, i);
    }
}

/**
 * @brief Make pattern 01010101010...
 */
static void P_prepare_fill_AA(
        bitmap_block_t *bitmap,
        size_t bits_num
)
{
    bitmap_bitwise_clear2(bitmap, bits_num);
    size_t i;
    for(i = 1; i < bits_num; i += 2)
    {
        bitmap_bit_raise2(bitmap, i);
    }
}

/**
 * @brief Make pattern 111000...
 */
static void P_prepare_fill_111000(
        bitmap_block_t *bitmap,
        size_t bits_num
)
{
#undef HALF_PERIOD
#define HALF_PERIOD  (3)
    if(bits_num == 0) return;

    bitmap_range_t range = {0, HALF_PERIOD};
    bool quit = false;
    for(;;)
    {
        if(range.end >= bits_num)
        {
            range.end = bits_num - 1;
            quit = true;
        }
        bitmap_bitwise_range_raise2(bitmap, &range);
        if(quit) break;
        range.begin += HALF_PERIOD;
        range.end += HALF_PERIOD;

        if(range.end >= bits_num)
        {
            range.end = bits_num - 1;
            quit = true;
        }
        bitmap_bitwise_range_clear2(bitmap, &range);
        if(quit) break;
        range.begin += HALF_PERIOD;
        range.end += HALF_PERIOD;
    }
}

/**
 * @brief Make pattern 000111...
 */
static void P_prepare_fill_000111(
        bitmap_block_t *bitmap,
        size_t bits_num
)
{
#undef HALF_PERIOD
#define HALF_PERIOD  (3)
    if(bits_num == 0) return;

    bitmap_range_t range = {0, HALF_PERIOD};
    bool quit = false;
    for(;;)
    {
        if(range.end >= bits_num)
        {
            range.end = bits_num - 1;
            quit = true;
        }
        bitmap_bitwise_range_clear2(bitmap, &range);
        if(quit) break;
        range.begin += HALF_PERIOD;
        range.end += HALF_PERIOD;

        if(range.end >= bits_num)
        {
            range.end = bits_num - 1;
            quit = true;
        }
        bitmap_bitwise_range_raise2(bitmap, &range);
        if(quit) break;
        range.begin += HALF_PERIOD;
        range.end += HALF_PERIOD;
    }
}

static void P_prepare_bitmap(
        const size_t * indexes,
        size_t indexes_size,
        bitmap_block_t * bitmap,
        size_t bits_num
)
{
    /* prepare */
    P_prepare_fill_0_trashed(bitmap, bits_num);

    size_t i;
    for(i = 0; i < indexes_size; ++i)
    {
        size_t index = indexes[i];
        bitmap_bit_raise2(bitmap, index);
    }
}

TEST_CASE(
        "bitmaps macro test",
        "[bitmap][macro]"
)
{
    size_t n3 = BITMAP_BITS_TO_BLOCKS_ALIGNED(BITMAP_SIZE3);
    CHECK( n3 == 1 );
    size_t n67 = BITMAP_BITS_TO_BLOCKS_ALIGNED(BITMAP_SIZE67);
    CHECK( n67 == 2 );
}

TEST_CASE(
        "bitmaps bitmap_bitwise_raise test",
        "[bitmap][bitmap_bitwise_raise]"
)
{
    static BITMAP_VAR(bitmap_67, BITMAP_SIZE67);

    {
        memset(bitmap_67, 0, sizeof(bitmap_67));
        bitmap_bitwise_raise1(bitmap_67, BITMAP_SIZE67);
        static const uint8_t pattern[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        CHECK( memcmp(bitmap_67, pattern, sizeof(pattern)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_clear test",
        "[bitmap][bitmap_bitwise_clear]"
)
{
    static BITMAP_VAR(bitmap_67, BITMAP_SIZE67);

    {
        memset(bitmap_67, -1, sizeof(bitmap_67));
        bitmap_bitwise_clear2(bitmap_67, BITMAP_SIZE67);
        static const uint8_t pattern[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        CHECK( memcmp(bitmap_67, pattern, sizeof(pattern)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_copy test",
        "[bitmap][bitmap_bitwise_copy]"
)
{
    static BITMAP_VAR(bitmap_src67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_dest67, BITMAP_SIZE67);

    {
        bitmap_bitwise_clear2(bitmap_src67, BITMAP_SIZE67);
        bitmap_bitwise_raise1(bitmap_dest67, BITMAP_SIZE67);
        bitmap_bitwise_copy3(bitmap_dest67, bitmap_src67, BITMAP_SIZE67);

        static const uint8_t pattern[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        CHECK( memcmp(bitmap_dest67, pattern, sizeof(pattern)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bit_raise test",
        "[bitmap][bitmap_bit_raise]"
)
{
    int res;
    static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

    bitmap_bitwise_clear2(bitmap67, BITMAP_SIZE67);

    {
        static uint64_t pattern1 = 0;
        static uint64_t pattern2 = 0;

        uint64_t bit;
        unsigned i;

        bit = 1;
        for(i = 0; i < BITMAP_SIZE64; ++i)
        {
            bitmap_bit_raise2(bitmap67, i);
            pattern1 |= bit;

            res = memcmp(bitmap67, &pattern1, sizeof(pattern1));
            CHECK( res == 0 );

            bit <<= 1;
        }

        bit = 1;
        for(; i < BITMAP_SIZE67; ++i)
        {
            bitmap_bit_raise2(bitmap67, i);
            pattern2 |= bit;

            res = memcmp((uint8_t*)bitmap67 + sizeof(pattern1), &pattern2, sizeof(pattern1));
            CHECK( res == 0 );

            bit <<= 1;
        }
    }
}

TEST_CASE(
        "bitmaps bitmap_bit_clear test",
        "[bitmap][bitmap_bit_clear]"
)
{
    int res;
    static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

    bitmap_bitwise_raise1(bitmap67, BITMAP_SIZE67);

    {
        static uint64_t pattern1 = 0xffffffffffffffff;
        static uint64_t pattern2 = 0xffffffffffffffff;

        uint64_t bit;
        unsigned i;

        bit = 1;
        for(i = 0; i < BITMAP_SIZE64; ++i)
        {
            bitmap_bit_clear2(bitmap67, i);
            pattern1 &= ~bit;

            res = memcmp(bitmap67, &pattern1, sizeof(pattern1));
            CHECK( res == 0 );

            bit <<= 1;
        }

        bit = 1;
        for(; i < BITMAP_SIZE67; ++i)
        {
            bitmap_bit_clear2(bitmap67, i);
            pattern2 &= ~bit;

            res = memcmp((uint8_t*)bitmap67 + sizeof(pattern1), &pattern2, BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67 - BITMAP_SIZE64));
            CHECK( res == 0 );

            bit <<= 1;
        }

    }

}

TEST_CASE(
        "bitmaps bitmap_bit_get test",
        "[bitmap][bitmap_bit_get]"
)
{
    bool bitvalue;

    static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

    {
        bitmap_bitwise_clear2(bitmap67, BITMAP_SIZE67);
        bitmap_bit_raise2(bitmap67, 0);
        bitmap_bit_raise2(bitmap67, 66);

        bitvalue = bitmap_bit_get2(bitmap67, 0);
        CHECK( bitvalue == true );
        bitvalue = bitmap_bit_get2(bitmap67, 1);
        CHECK( bitvalue == false );
        bitvalue = bitmap_bit_get2(bitmap67, 65);
        CHECK( bitvalue == false );
        bitvalue = bitmap_bit_get2(bitmap67, 66);
        CHECK( bitvalue == true );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_not2 test",
        "[bitmap][bitmap_bitwise_not2]"
)
{
    static BITMAP_VAR(bitmap_dest67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_src67, BITMAP_SIZE67);

    {
        bitmap_bitwise_clear2(bitmap_src67, BITMAP_SIZE67);
        bitmap_bitwise_raise1(bitmap_dest67, BITMAP_SIZE67);

        static const uint8_t pattern1[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
                0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        memcpy(bitmap_src67, pattern1, sizeof(pattern1));

        bitmap_bitwise_not3(bitmap_dest67, bitmap_src67, BITMAP_SIZE67);

        static const uint8_t pattern2[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
                0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        CHECK( memcmp(bitmap_dest67, pattern2, sizeof(pattern2)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_or2 test",
        "[bitmap][bitmap_bitwise_or2]"
)
{
    static BITMAP_VAR(bitmap_dest67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_src67, BITMAP_SIZE67);

    P_prepare_fill_0_trashed(bitmap_dest67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_src67, BITMAP_SIZE67);

    {
        /* prepare */
        bitmap_bit_raise2(bitmap_dest67, 0);
        bitmap_bit_raise2(bitmap_dest67, 2);
        bitmap_bit_raise2(bitmap_dest67, 4);
        bitmap_bit_raise2(bitmap_src67, 0);
        bitmap_bit_raise2(bitmap_src67, 2);

        /* operation */
        bitmap_bitwise_or3(bitmap_dest67, bitmap_src67, BITMAP_SIZE67);

        /* check */
        /* 1010 1000 */
        static const uint8_t pattern67[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        CHECK( memcmp(bitmap_dest67, pattern67, sizeof(pattern67)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_or3 test",
        "[bitmap][bitmap_bitwise_or3]"
)
{
    static BITMAP_VAR(bitmap_dest67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_a67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_b67, BITMAP_SIZE67);

    P_prepare_fill_0_trashed(bitmap_dest67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_a67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

    {
        /* prepare */
        bitmap_bit_raise2(bitmap_a67, 0);
        bitmap_bit_raise2(bitmap_a67, 2);
        bitmap_bit_raise2(bitmap_a67, 4);
        bitmap_bit_raise2(bitmap_b67, 0);
        bitmap_bit_raise2(bitmap_b67, 2);

        /* operation */
        bitmap_bitwise_or4(bitmap_dest67, bitmap_a67, bitmap_b67, BITMAP_SIZE67);

        /* check */
        /* 1010 1000 */
        static const uint8_t pattern67[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        CHECK( memcmp(bitmap_dest67, pattern67, sizeof(pattern67)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_and2 test",
        "[bitmap][bitmap_bitwise_and2]"
)
{
    static BITMAP_VAR(bitmap_dest67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_src67, BITMAP_SIZE67);

    P_prepare_fill_0_trashed(bitmap_dest67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_src67, BITMAP_SIZE67);

    {
        /* prepare */
        bitmap_bit_raise2(bitmap_dest67, 0);
        bitmap_bit_raise2(bitmap_dest67, 2);
        bitmap_bit_raise2(bitmap_dest67, 4);

        bitmap_bit_raise2(bitmap_src67, 0);
        bitmap_bit_raise2(bitmap_src67, 2);

        /* operation */
        bitmap_bitwise_and3(bitmap_dest67, bitmap_src67, BITMAP_SIZE67);

        /* check */
        /* 1010 0000 */
        static const uint8_t pattern67[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        CHECK( memcmp(bitmap_dest67, pattern67, sizeof(pattern67)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_and3 test",
        "[bitmap][bitmap_bitwise_and3]"
)
{
    static BITMAP_VAR(bitmap_dest67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_a67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_b67, BITMAP_SIZE67);

    P_prepare_fill_0_trashed(bitmap_dest67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_a67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

    {
        /* prepare */
        bitmap_bit_raise2(bitmap_a67, 0);

        bitmap_bit_raise2(bitmap_a67, 2);

        bitmap_bit_raise2(bitmap_a67, 4);

        bitmap_bit_raise2(bitmap_b67, 0);

        bitmap_bit_raise2(bitmap_b67, 2);

        /* operation */
        bitmap_bitwise_and4(bitmap_dest67, bitmap_a67, bitmap_b67, BITMAP_SIZE67);

        /* check */
        /* 1010 1000 */
        static const uint8_t pattern67[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        CHECK( memcmp(bitmap_dest67, pattern67, sizeof(pattern67)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_clear2 test",
        "[bitmap][bitmap_bitwise_clear2]"
)
{
    static BITMAP_VAR(bitmap_dest67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_src67, BITMAP_SIZE67);

    P_prepare_fill_0_trashed(bitmap_dest67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_src67, BITMAP_SIZE67);

    {
        /* prepare */
        bitmap_bitwise_raise1(bitmap_dest67, BITMAP_SIZE67);

        bitmap_bitwise_clear2(bitmap_src67, BITMAP_SIZE67);

        bitmap_bit_raise2(bitmap_src67, 0);
        bitmap_bit_raise2(bitmap_src67, 2);
        bitmap_bit_raise2(bitmap_src67, 4);
        bitmap_bit_raise2(bitmap_src67, 6);

        /* operation */
        bitmap_bitwise_clear3(bitmap_dest67, bitmap_src67, BITMAP_SIZE67);

        /* check */
        /* 0101 0101 */
        static const uint8_t pattern67[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        CHECK( memcmp(bitmap_dest67, pattern67, sizeof(pattern67)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_clear3 test",
        "[bitmap][bitmap_bitwise_clear3]"
)
{
    static BITMAP_VAR(bitmap_dest67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_a67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_b67, BITMAP_SIZE67);

    P_prepare_fill_0_trashed(bitmap_dest67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_a67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

    {
        /* prepare */
        bitmap_bitwise_raise1(bitmap_dest67, BITMAP_SIZE67);

        bitmap_bitwise_raise1(bitmap_a67, BITMAP_SIZE67);

        bitmap_bitwise_clear2(bitmap_b67, BITMAP_SIZE67);

        bitmap_bit_raise2(bitmap_b67, 0);
        bitmap_bit_raise2(bitmap_b67, 2);
        bitmap_bit_raise2(bitmap_b67, 4);
        bitmap_bit_raise2(bitmap_b67, 6);

        /* operation */
        bitmap_bitwise_clear4(bitmap_dest67, bitmap_a67, bitmap_b67, BITMAP_SIZE67);

        /* check */
        /* 0101 0101 1111 1111 */
        static const uint8_t pattern67[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        CHECK( memcmp(bitmap_dest67, pattern67, sizeof(pattern67)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_zero test",
        "[bitmap][bitmap_bitwise_check_zero]"
)
{
    static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

    P_prepare_fill_0_trashed(bitmap67, BITMAP_SIZE67);

    {
        /* prepare */
        bitmap_bitwise_clear2(bitmap67, BITMAP_SIZE67);

        /* operation */
        bool zero;
        zero = bitmap_bitwise_check_zero2(bitmap67, BITMAP_SIZE67);
        CHECK( zero == true );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_equal test",
        "[bitmap][bitmap_bitwise_check_equal]"
)
{
    {
        static BITMAP_VAR(bitmap_a67, BITMAP_SIZE67);
        static BITMAP_VAR(bitmap_b67, BITMAP_SIZE67);

        /* prepare */
        P_prepare_fill_0(bitmap_a67, BITMAP_SIZE67);
        P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

        bitmap_bitwise_clear2(bitmap_b67, BITMAP_SIZE67);

        /* operation */
        bool equal;
        equal = bitmap_bitwise_check_equal3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( equal == true );
    }

    {
        static BITMAP_VAR(bitmap_a128, BITMAP_SIZE128);
        static BITMAP_VAR(bitmap_b128, BITMAP_SIZE128);

        P_prepare_fill_0_trashed(bitmap_a128, BITMAP_SIZE67);
        P_prepare_fill_0_trashed(bitmap_b128, BITMAP_SIZE67);

        /* prepare */
        bitmap_bitwise_clear2(bitmap_a128, BITMAP_SIZE128);

        bitmap_bitwise_clear2(bitmap_b128, BITMAP_SIZE128);

        bitmap_bit_raise2(bitmap_a128, 15);

        bitmap_bit_raise2(bitmap_b128, 14);

        /* operation */
        bool equal;
        equal = bitmap_bitwise_check_equal3(bitmap_a128, bitmap_b128, BITMAP_SIZE128);
        CHECK( equal == false );
    }

}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_inclusion test",
        "[bitmap][bitmap_bitwise_check_inclusion]"
)
{
    static BITMAP_VAR(bitmap_a67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_b67, BITMAP_SIZE67);

    {
        P_prepare_fill_0(bitmap_a67, BITMAP_SIZE67);
        P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

        /* prepare */
        bitmap_bitwise_clear2(bitmap_a67, BITMAP_SIZE67);

        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 0100 0000 0001 1111 */

        bitmap_bit_raise2(bitmap_a67, 1);
        bitmap_bit_raise2(bitmap_a67, 2);

        bitmap_bit_raise2(bitmap_b67, 1);

        bool inclusion;
        /* operation */
        inclusion = bitmap_bitwise_check_inclusion3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( inclusion == true );

        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1100 0000 0001 1111 */

        bitmap_bit_raise2(bitmap_b67, 0);

        /* operation */
        inclusion = bitmap_bitwise_check_inclusion3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( inclusion == false );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_intersection test",
        "[bitmap][bitmap_bitwise_check_intersection]"
)
{
    static BITMAP_VAR(bitmap_a67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_b67, BITMAP_SIZE67);

    P_prepare_fill_0(bitmap_a67, BITMAP_SIZE67);
    P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

    {
        /* prepare */
        bitmap_bitwise_clear2(bitmap_a67, BITMAP_SIZE67);

        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1000 0000 0001 1111 */

        bitmap_bit_raise2(bitmap_a67, 1);

        bitmap_bit_raise2(bitmap_a67, 2);

        bitmap_bit_raise2(bitmap_b67, 0);

        bool intersection;
        /* operation */
        intersection = bitmap_bitwise_check_intersection3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( intersection == false );

        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1100 0000 0001 1111 */

        bitmap_bit_raise2(bitmap_b67, 1);

        /* operation */
        intersection = bitmap_bitwise_check_intersection3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( intersection == true );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_relation test",
        "[bitmap][bitmap_bitwise_check_relation]"
)
{
    static BITMAP_VAR(bitmap_a67, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap_b67, BITMAP_SIZE67);

    /* prepare */
    P_prepare_fill_0(bitmap_a67, BITMAP_SIZE67);

    bitmap_bit_raise2(bitmap_a67, 1);

    bitmap_bit_raise2(bitmap_a67, 2);

    {
        /* BITMAP_RELATION__EQUAL */
        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 0110 0000 0001 1111 */
        /* b = 1110 0000 0001 1111 */

        /* prepare */
        P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

        bitmap_bit_raise2(bitmap_b67, 1);
        bitmap_bit_raise2(bitmap_b67, 2);

        bitmap_relation_t relation;
        /* operation */
        relation = bitmap_bitwise_check_relation3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( relation == BITMAP_RELATION__EQUAL );

        bitmap_bit_raise2(bitmap_b67, 0);

        /* operation */
        relation = bitmap_bitwise_check_relation3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( relation == BITMAP_RELATION__INTERSECTION );
    }

    {
        /* BITMAP_RELATION__INCLUSION */
        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 0010 0000 0001 1111 */
        /* b = 1010 0000 0001 1111 */

        /* prepare */
        P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

        bitmap_bit_raise2(bitmap_b67, 2);

        bitmap_relation_t relation;
        /* operation */
        relation = bitmap_bitwise_check_relation3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( relation == BITMAP_RELATION__INCLUSION );

        bitmap_bit_raise2(bitmap_b67, 0);

        /* operation */
        relation = bitmap_bitwise_check_relation3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( relation == BITMAP_RELATION__INTERSECTION );
    }

    {
        /* BITMAP_RELATION__INTERSECTION */
        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1010 0000 0001 1111 */
        /* b = 0010 0000 0001 1111 */

        /* prepare */
        P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

        bitmap_bit_raise2(bitmap_b67, 0);
        bitmap_bit_raise2(bitmap_b67, 2);

        bitmap_relation_t relation;
        /* operation */
        relation = bitmap_bitwise_check_relation3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( relation == BITMAP_RELATION__INTERSECTION );

        bitmap_bit_clear2(bitmap_b67, 0);

        /* operation */
        relation = bitmap_bitwise_check_relation3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( relation == BITMAP_RELATION__INCLUSION );
    }

    {
        /* BITMAP_RELATION__DIFFERENT */
        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1001 0000 0001 1111 */

        /* prepare */
        P_prepare_fill_0_trashed(bitmap_b67, BITMAP_SIZE67);

        bitmap_bit_raise2(bitmap_b67, 0);
        bitmap_bit_raise2(bitmap_b67, 3);

        bitmap_relation_t relation;
        /* operation */
        relation = bitmap_bitwise_check_relation3(bitmap_a67, bitmap_b67, BITMAP_SIZE67);
        CHECK( relation == BITMAP_RELATION__DIFFERENT );
    }
}

TEST_CASE(
        "bitmaps bitmap_bit_nearest_forward_set_get test",
        "[bitmap][bitmap_bit_nearest_forward_set_get]"
)
{
    {
        static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

        /* prepare */
        static const size_t indexes[] = { 0, 2, 4, 6, 8, 10, 66 };
        P_prepare_bitmap(indexes, ARRAY_SIZE(indexes), bitmap67, BITMAP_SIZE67);

        /* operation */
        bitmap_foreach_bit_context_t ctx;

        size_t ibit;
        size_t i = 0;
        BITMAP_FOREACH_BIT_IN_BITMAP(&ibit, bitmap67, BITMAP_SIZE67, &ctx)
        {
            CHECK( ibit == indexes[i] );
            ++i;
        }
        CHECK( i == ARRAY_SIZE(indexes) );
    }

    {
        static BITMAP_VAR(bitmap133, BITMAP_SIZE133);

        /* prepare */
        static const size_t indexes[] = {
                0, 2, 14, 15,
                64, 66, 127,
                128, 130, 132,
        };
        P_prepare_bitmap(indexes, ARRAY_SIZE(indexes), bitmap133, BITMAP_SIZE133);

        /* operation */
        bitmap_foreach_bit_context_t ctx;

        size_t ibit;
        size_t i = 0;
        BITMAP_FOREACH_BIT_IN_BITMAP(&ibit, bitmap133, BITMAP_SIZE133, &ctx)
        {
            CHECK( ibit == indexes[i] );
            ++i;
        }
        CHECK( i == ARRAY_SIZE(indexes) );
    }

}

TEST_CASE(
        "bitmaps bitmap_snprintf_ranged test",
        "[bitmap][bitmap_snprintf_ranged]"
)
{
    int status;
#define STR_SIZE 256
    char str[STR_SIZE];
    static const char * enum_marker = ", ";
    static const char * range_marker = " - ";

    {
        static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

        /*          xxxx xxxx xxx. .... */
        /* bitmap = 0000 0000 0000 0000 */
        static const char * str_pattern = "";

        P_prepare_fill_0_trashed(bitmap67, BITMAP_SIZE67);

        status = bitmap_snprintf_ranged6(str, STR_SIZE, bitmap67, BITMAP_SIZE67, enum_marker, range_marker);
        REQUIRE( status == 0 );

        CHECK( strncmp(str, str_pattern, STR_SIZE) == 0 );
    }
    {
        static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

        /*          xxxx xxxx xxx. .... */
        /* bitmap = 1010 1010 1010 0000 */
        static const size_t indexes[] = { 0, 2, 4, 6, 8, 10, 66, 67 };
        static const char * str_pattern = "0, 2, 4, 6, 8, 10, 66";

        P_prepare_bitmap(indexes, ARRAY_SIZE(indexes), bitmap67, BITMAP_SIZE67);

        status = bitmap_snprintf_ranged6(str, STR_SIZE, bitmap67, BITMAP_SIZE67, enum_marker, range_marker);
        REQUIRE( status == 0 );

        CHECK( strncmp(str, str_pattern, STR_SIZE) == 0 );
    }
    {
        static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

        /*          xxxx xxxx xxx. .... */
        /* bitmap = 0101 1011 1110 0000 */
        static const size_t indexes[] = { 1, 3, 4, 6, 7, 8, 9, 10, 64, 65, 66, 67 };
        static const char * str_pattern = "1, 3, 4, 6 - 10, 64 - 66";

        P_prepare_bitmap(indexes, ARRAY_SIZE(indexes), bitmap67, BITMAP_SIZE67);

        status = bitmap_snprintf_ranged6(str, STR_SIZE, bitmap67, BITMAP_SIZE67, enum_marker, range_marker);
        REQUIRE( status == 0 );

        CHECK( strncmp(str, str_pattern, STR_SIZE) == 0 );
    }
    {
        static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

        /*          xxxx xxxx xxx. .... */
        /* bitmap = 1111 1010 1000 0000 */
        static const size_t indexes[] = { 0, 1, 2, 3, 4, 6, 8 };
        static const char * str_pattern = "0 - 4, 6, 8";

        P_prepare_bitmap(indexes, ARRAY_SIZE(indexes), bitmap67, BITMAP_SIZE67);

        status = bitmap_snprintf_ranged6(str, STR_SIZE, bitmap67, BITMAP_SIZE67, enum_marker, range_marker);
        REQUIRE( status == 0 );

        CHECK( strncmp(str, str_pattern, STR_SIZE) == 0 );
    }

#undef STR_SIZE
}

TEST_CASE(
        "bitmaps bitmap_bitwise_power test",
        "[bitmap][bitmap_bitwise_power]"
)
{
    static BITMAP_VAR(bitmap67, BITMAP_SIZE67);

    static const size_t indexes[] = { 0, 2, 4, 6, 8, 10, 66 };
    P_prepare_bitmap(indexes, ARRAY_SIZE(indexes), bitmap67, BITMAP_SIZE67);

    size_t size = bitmap_bitwise_power2(
            bitmap67,
            BITMAP_SIZE67
    );

    CHECK(size == ARRAY_SIZE(indexes));
}

TEST_CASE(
        "bitmaps bitmap_bitwise_power6 test",
        "[bitmap][bitmap_bitwise_power6]"
)
{
    static BITMAP_VAR(bitmap67_1, BITMAP_SIZE67);
    static BITMAP_VAR(bitmap133_2, BITMAP_SIZE133);

    static const size_t indexes_a[] = { 0, 2, 4, 6, 8, 10, 66 };
    P_prepare_bitmap(indexes_a, ARRAY_SIZE(indexes_a), bitmap67_1, BITMAP_SIZE67);

    static const size_t indexes_b[] = {    2,    6,    10,     67, 120, 127, 128, 132 };
    P_prepare_bitmap(indexes_b, ARRAY_SIZE(indexes_b), bitmap133_2, BITMAP_SIZE133);

    size_t power_intersection;
    size_t power_union;

    {

        bitmap_bitwise_power6(
                bitmap67_1,
                BITMAP_SIZE67,
                bitmap133_2,
                BITMAP_SIZE133,
                &power_intersection,
                &power_union
        );

        CHECK( power_intersection == 3);
        CHECK( power_union == 12);
    }

    {
        bitmap_bitwise_power6(
                bitmap133_2,
                BITMAP_SIZE133,
                bitmap67_1,
                BITMAP_SIZE67,
                &power_intersection,
                &power_union
        );

        CHECK( power_intersection == 3);
        CHECK( power_union == 12);
    }

}

TEST_CASE(
        "bitmaps bitmap_sscanf_append_ranged test",
        "[bitmap][bitmap_sscanf_append_ranged]"
)
{
    static BITMAP_VAR(bitmap67, BITMAP_SIZE67);
    int res;

    {
        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "x123"
        );
        CHECK( res == -1 );
    }
    {
        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "1x"
        );
        CHECK( res == -1 );
    }
    {
        static uint8_t pattern[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x00 | (1 << 1), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xf8           , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        P_prepare_fill_0_trashed(
                bitmap67,
                BITMAP_SIZE67
        );

        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "1"
        );
        CHECK( res == 0 );
        CHECK( memcmp(bitmap67, pattern, sizeof(pattern) ) == 0 );

    }
    {
        static uint8_t pattern[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x00 | (1 << 1) | (1 << 2) | (1 << 3),
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xf8 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        P_prepare_fill_0_trashed(
                bitmap67,
                BITMAP_SIZE67
        );

        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "1 - 3"
        );
        CHECK( res == 0 );
        CHECK( memcmp(bitmap67, pattern, sizeof(pattern) ) == 0 );

    }

    {
        static uint8_t pattern[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x00 | (1 << 1) | (1 << 5) | (1 << 6) | (1 << 7),
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        P_prepare_fill_0_trashed(
                bitmap67,
                BITMAP_SIZE67
        );

        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "1, 5 - 7"
        );
        CHECK( res == 0 );
        CHECK( memcmp(bitmap67, pattern, sizeof(pattern) ) == 0 );

    }

    {
        static uint8_t pattern[BITMAP_BITS_TO_BYTES_ALIGNED(BITMAP_SIZE67)] =
        {
                0x00 | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 5),
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        P_prepare_fill_0_trashed(
                bitmap67,
                BITMAP_SIZE67
        );

        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "1 - 3, 5"
        );
        CHECK( res == 0 );
        CHECK( memcmp(bitmap67, pattern, sizeof(pattern) ) == 0 );
    }

    {
        static BITMAP_VAR(pattern67, BITMAP_SIZE67);
        P_prepare_fill_55(
                pattern67,
                BITMAP_SIZE67
        );

        bitmap_bitwise_clear2(bitmap67, BITMAP_SIZE67);
        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "0,2,4,6,8,"
                "10,12,14,16,18,"
                "20,22,24,26,28,"
                "30,32,34,36,38,"
                "40,42,44,46,48,"
                "50,52,54,56,58,"
                "60,62,64,66"
        );

        CHECK( res == 0 );
        CHECK( memcmp(bitmap67, pattern67, sizeof(pattern67) ) == 0 );
    }

    {
        static BITMAP_VAR(pattern67, BITMAP_SIZE67);
        P_prepare_fill_AA(
                pattern67,
                BITMAP_SIZE67
        );

        bitmap_bitwise_clear2(bitmap67, BITMAP_SIZE67);
        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "1,3,5,7,9,"
                "11,13,15,17,19,"
                "21,23,25,27,29,"
                "31,33,35,37,39,"
                "41,43,45,47,49,"
                "51,53,55,57,59,"
                "61,63,65"
        );

        CHECK( res == 0 );
        CHECK( memcmp(bitmap67, pattern67, sizeof(pattern67) ) == 0 );
    }

    {
        static BITMAP_VAR(pattern67, BITMAP_SIZE67);
        P_prepare_fill_111000(
                pattern67,
                BITMAP_SIZE67
        );

        bitmap_bitwise_clear2(bitmap67, BITMAP_SIZE67);
        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "0-2,"
                "6,7,8,"
                "12-14,"
                "18-20,"
                "24,25,26,"
                "30,31,32,"
                "36-38,"
                "42-44,"
                "48-50,"
                "54,55,56,"
                "60,61,62,"
                "66"
        );

        CHECK( res == 0 );
        CHECK( memcmp(bitmap67, pattern67, sizeof(pattern67) ) == 0 );
    }

    {
        static BITMAP_VAR(pattern67, BITMAP_SIZE67);
        P_prepare_fill_000111(
                pattern67,
                BITMAP_SIZE67
        );

        bitmap_bitwise_clear2(bitmap67, BITMAP_SIZE67);
        res = bitmap_sscanf_append_ranged5(
                bitmap67,
                BITMAP_SIZE67,
                ',',
                '-',
                "3,4,5,"
                "9-11,"
                "15,16,17,"
                "21,22,23,"
                "27-29,"
                "33-35,"
                "39,40,41,"
                "45,46,47,"
                "51,52,53,"
                "57-59,"
                "63-65"
        );

        CHECK( res == 0 );
        CHECK( memcmp(bitmap67, pattern67, sizeof(pattern67) ) == 0 );
    }
}
