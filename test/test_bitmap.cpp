/**
 * @file test_bitmap.cpp
 *
 */

#include <bitmap/bitmap4096.h>

#include <catch/catch.hpp>

#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define BITMAP_SIZE3 (3)
#define BITMAP_SIZE11 (8 + 3)
#define BITMAP_SIZE16 (8 + 8)
#define BITMAP_SIZE22 (8 + 8 + 6)

static void P_prepare_fill0(
        bitmap_block_type_t *bitmap,
        size_t bits_num
)
{
    /* Сделаем так, чтобы хвостовые биты були установлены в 1, но они не должны учитываться при сравнении */
    bitmap_bitwise_set(bitmap, true, bits_num);

    size_t i;
    for(i = 0; i < bits_num; ++i)
    {
        bitmap_bit_set(bitmap, bits_num, i, false);
    }
}

static void P_prepare_bitmap(
        const size_t * indexes,
        size_t indexes_size,
        bitmap_block_type_t * bitmap,
        size_t bits_num
)
{
    /* prepare */
    P_prepare_fill0(bitmap, bits_num);

    size_t i;
    for(i = 0; i < indexes_size; ++i)
    {
        size_t index = indexes[i];
        bitmap_bit_set(bitmap, bits_num, index, true);
    }
}

TEST_CASE(
        "bitmaps macro test",
        "[bitmap][macro]"
)
{
    size_t n3 = BITMAP_BLOCKS_NUM(BITMAP_SIZE3);
    CHECK( n3 == 1 );
}

TEST_CASE(
        "bitmaps bitmap_bitwise_set test",
        "[bitmap][bitmap_bitwise_set]"
)
{

    static BITMAP_VAR(bitmap_a3, BITMAP_SIZE3);
    static BITMAP_VAR(bitmap_a11, BITMAP_SIZE11);

    {
        bitmap_bitwise_set(bitmap_a3, false, BITMAP_SIZE3);
        static const uint8_t pattern3[1] = {0x00};
        CHECK( memcmp(bitmap_a3, pattern3, sizeof(pattern3)) == 0 );
    }
    {
        bitmap_bitwise_set(bitmap_a11, true, BITMAP_SIZE11);
        static const uint8_t pattern11[2] = {0xFF, 0xFF};
        CHECK( memcmp(bitmap_a11, pattern11, sizeof(pattern11)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_copy test",
        "[bitmap][bitmap_bitwise_copy]"
)
{
    static BITMAP_VAR(bitmap_src11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_dest11, BITMAP_SIZE11);

    {
        bitmap_bitwise_set(bitmap_src11, false, BITMAP_SIZE11);
        bitmap_bitwise_set(bitmap_dest11, true, BITMAP_SIZE11);
        bitmap_bitwise_copy(bitmap_dest11, bitmap_src11, BITMAP_SIZE11);

        static const uint8_t pattern11[2] = {0x00, 0x00};
        CHECK( memcmp(bitmap_dest11, pattern11, sizeof(pattern11)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bit_set test",
        "[bitmap][bitmap_bit_set]"
)
{
    static BITMAP_VAR(bitmap11, BITMAP_SIZE11);

    bitmap_bitwise_set(bitmap11, false, BITMAP_SIZE11);

    {
        bitmap_bit_set(bitmap11, BITMAP_SIZE11, 0, true);

        static const uint8_t pattern[2] = {0x01, 0x00};
        int res = memcmp(bitmap11, pattern, sizeof(pattern));
        CHECK( res == 0 );
    }

    {
        bitmap_bit_set(bitmap11, BITMAP_SIZE11, 2, true);

        static const uint8_t pattern[2] = {0x05, 0x00};
        int res = memcmp(bitmap11, pattern, sizeof(pattern));
        CHECK( res == 0 );
    }

    {
        bitmap_bit_set(bitmap11, BITMAP_SIZE11, 2, false);

        static const uint8_t pattern[2] = {0x01, 0x00};
        int res = memcmp(bitmap11, pattern, sizeof(pattern));
        CHECK( res == 0 );
    }

    {
        bitmap_bit_set(bitmap11, BITMAP_SIZE11, 10, true);

        static const uint8_t pattern[2] = {0x01, 0x04};
        int res = memcmp(bitmap11, pattern, sizeof(pattern));
        CHECK( res == 0 );
    }

    {
        bitmap_bit_set(bitmap11, BITMAP_SIZE11, 11, true);

        static const uint8_t pattern[2] = {0x01, 0x04};
        int res = memcmp(bitmap11, pattern, sizeof(pattern));
        CHECK( res == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bit_get test",
        "[bitmap][bitmap_bit_get]"
)
{
    bool bitvalue;

    static BITMAP_VAR(bitmap11, BITMAP_SIZE11);

    {
        bitmap_bitwise_set(bitmap11, false, BITMAP_SIZE11);
        bitmap_bit_set(bitmap11, BITMAP_SIZE11, 0, true);

        bitvalue = bitmap_bit_get(bitmap11, BITMAP_SIZE11, 0);
        CHECK( bitvalue == true );
        bitvalue = bitmap_bit_get(bitmap11, BITMAP_SIZE11, 1);
        CHECK( bitvalue == false );
        /* check over last bit of bitmap */
        bitvalue = bitmap_bit_get(bitmap11, BITMAP_SIZE11, 10 + 1);
        CHECK( bitvalue == false );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_not2 test",
        "[bitmap][bitmap_bitwise_not2]"
)
{
    static BITMAP_VAR(bitmap_dest11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_src11, BITMAP_SIZE11);

    {
        bitmap_bitwise_set(bitmap_src11, false, BITMAP_SIZE11);
        bitmap_bitwise_set(bitmap_dest11, true, BITMAP_SIZE11);

        static const uint8_t bitmap[2] = {0xF0, 0xF0};
        memcpy(bitmap_src11, bitmap, sizeof(bitmap));

        bitmap_bitwise_not2(bitmap_dest11, bitmap_src11, BITMAP_SIZE11);

        static const uint8_t pattern11[2] = {0x0F, 0x0F};
        CHECK( memcmp(bitmap_dest11, pattern11, sizeof(pattern11)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_or2 test",
        "[bitmap][bitmap_bitwise_or2]"
)
{
    static BITMAP_VAR(bitmap_dest11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_src11, BITMAP_SIZE11);

    P_prepare_fill0(bitmap_dest11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_src11, BITMAP_SIZE11);

    {
        /* prepare */
        bitmap_bit_set(bitmap_dest11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_dest11, BITMAP_SIZE11, 2, true);
        bitmap_bit_set(bitmap_dest11, BITMAP_SIZE11, 4, true);
        bitmap_bit_set(bitmap_src11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_src11, BITMAP_SIZE11, 2, true);

        /* operation */
        bitmap_bitwise_or2(bitmap_dest11, bitmap_src11, BITMAP_SIZE11);

        /* check */
        static const uint8_t pattern11[2] = {0x15, 0xf8}; /* 1010 1000 0001 1111 */
        CHECK( memcmp(bitmap_dest11, pattern11, sizeof(pattern11)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_or3 test",
        "[bitmap][bitmap_bitwise_or3]"
)
{
    static BITMAP_VAR(bitmap_dest11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_a11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_b11, BITMAP_SIZE11);

    P_prepare_fill0(bitmap_dest11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_a11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

    {
        /* prepare */
        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 2, true);
        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 4, true);
        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 2, true);

        /* operation */
        bitmap_bitwise_or3(bitmap_dest11, bitmap_a11, bitmap_b11, BITMAP_SIZE11);

        /* check */
        static const uint8_t pattern11[2] = {0x15, 0xf8}; /* 1010 1000 0001 1111 */
        CHECK( memcmp(bitmap_dest11, pattern11, sizeof(pattern11)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_and2 test",
        "[bitmap][bitmap_bitwise_and2]"
)
{
    static BITMAP_VAR(bitmap_dest11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_src11, BITMAP_SIZE11);

    P_prepare_fill0(bitmap_dest11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_src11, BITMAP_SIZE11);

    {
        /* prepare */
        bitmap_bit_set(bitmap_dest11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_dest11, BITMAP_SIZE11, 2, true);
        bitmap_bit_set(bitmap_dest11, BITMAP_SIZE11, 4, true);

        bitmap_bit_set(bitmap_src11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_src11, BITMAP_SIZE11, 2, true);

        /* operation */
        bitmap_bitwise_and2(bitmap_dest11, bitmap_src11, BITMAP_SIZE11);

        /* check */
        static const uint8_t pattern11[2] = {0x05, 0xf8}; /* 1010 0000 0001 1111 */
        CHECK( memcmp(bitmap_dest11, pattern11, sizeof(pattern11)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_and3 test",
        "[bitmap][bitmap_bitwise_and3]"
)
{
    static BITMAP_VAR(bitmap_dest11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_a11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_b11, BITMAP_SIZE11);

    P_prepare_fill0(bitmap_dest11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_a11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

    {
        /* prepare */
        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 0, true);

        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 2, true);

        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 4, true);

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, true);

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 2, true);

        /* operation */
        bitmap_bitwise_and3(bitmap_dest11, bitmap_a11, bitmap_b11, BITMAP_SIZE11);

        /* check */
        static const uint8_t pattern11[2] = {0x05, 0xf8}; /* 1010 1000 0001 1111 */
        CHECK( memcmp(bitmap_dest11, pattern11, sizeof(pattern11)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_clear2 test",
        "[bitmap][bitmap_bitwise_clear2]"
)
{
    static BITMAP_VAR(bitmap_dest11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_src11, BITMAP_SIZE11);

    P_prepare_fill0(bitmap_dest11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_src11, BITMAP_SIZE11);

    {
        /* prepare */
        bitmap_bitwise_set(bitmap_dest11, true, BITMAP_SIZE11);

        bitmap_bitwise_set(bitmap_src11, false, BITMAP_SIZE11);

        bitmap_bit_set(bitmap_src11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_src11, BITMAP_SIZE11, 2, true);
        bitmap_bit_set(bitmap_src11, BITMAP_SIZE11, 4, true);
        bitmap_bit_set(bitmap_src11, BITMAP_SIZE11, 6, true);

        /* operation */
        bitmap_bitwise_clear2(bitmap_dest11, bitmap_src11, BITMAP_SIZE11);

        /* check */
        static const uint8_t pattern11[2] = {0xAA, 0xFF}; /* 0101 0101 1111 1111 */
        CHECK( memcmp(bitmap_dest11, pattern11, sizeof(pattern11)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_clear3 test",
        "[bitmap][bitmap_bitwise_clear3]"
)
{
    static BITMAP_VAR(bitmap_dest11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_a11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_b11, BITMAP_SIZE11);

    P_prepare_fill0(bitmap_dest11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_a11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

    {
        /* prepare */
        bitmap_bitwise_set(bitmap_dest11, true, BITMAP_SIZE11);

        bitmap_bitwise_set(bitmap_a11, true, BITMAP_SIZE11);

        bitmap_bitwise_set(bitmap_b11, false, BITMAP_SIZE11);

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 2, true);
        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 4, true);
        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 6, true);

        /* operation */
        bitmap_bitwise_clear3(bitmap_dest11, bitmap_a11, bitmap_b11, BITMAP_SIZE11);

        /* check */
        static const uint8_t pattern11[2] = {0xAA, 0xFF}; /* 0101 0101 1111 1111 */
        CHECK( memcmp(bitmap_dest11, pattern11, sizeof(pattern11)) == 0 );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_zero test",
        "[bitmap][bitmap_bitwise_check_zero]"
)
{
    static BITMAP_VAR(bitmap11, BITMAP_SIZE11);

    P_prepare_fill0(bitmap11, BITMAP_SIZE11);

    {
        /* prepare */
        bitmap_bitwise_set(bitmap11, false, BITMAP_SIZE11);

        /* operation */
        bool zero;
        zero = bitmap_bitwise_check_zero(bitmap11, BITMAP_SIZE11);
        CHECK( zero == true );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_equal test",
        "[bitmap][bitmap_bitwise_check_equal]"
)
{
    {
        static BITMAP_VAR(bitmap_a11, BITMAP_SIZE11);
        static BITMAP_VAR(bitmap_b11, BITMAP_SIZE11);

        /* prepare */
        P_prepare_fill0(bitmap_a11, BITMAP_SIZE11);
        P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

        bitmap_bitwise_set(bitmap_b11, false, BITMAP_SIZE11);

        /* operation */
        bool equal;
        equal = bitmap_bitwise_check_equal(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( equal == true );
    }

    {
        static BITMAP_VAR(bitmap_a16, BITMAP_SIZE16);
        static BITMAP_VAR(bitmap_b16, BITMAP_SIZE16);

        P_prepare_fill0(bitmap_a16, BITMAP_SIZE11);
        P_prepare_fill0(bitmap_b16, BITMAP_SIZE11);

        /* prepare */
        bitmap_bitwise_set(bitmap_a16, false, BITMAP_SIZE16);

        bitmap_bitwise_set(bitmap_b16, false, BITMAP_SIZE16);

        bitmap_bit_set(bitmap_a16, BITMAP_SIZE16, 15, true);

        bitmap_bit_set(bitmap_b16, BITMAP_SIZE16, 14, true);

        /* operation */
        bool equal;
        equal = bitmap_bitwise_check_equal(bitmap_a16, bitmap_b16, BITMAP_SIZE16);
        CHECK( equal == false );
    }

}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_inclusion test",
        "[bitmap][bitmap_bitwise_check_inclusion]"
)
{
    static BITMAP_VAR(bitmap_a11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_b11, BITMAP_SIZE11);

    {
        P_prepare_fill0(bitmap_a11, BITMAP_SIZE11);
        P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

        /* prepare */
        bitmap_bitwise_set(bitmap_a11, false, BITMAP_SIZE11);

        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 0100 0000 0001 1111 */

        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 1, true);

        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 2, true);

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 1, true);

        bool inclusion;
        /* operation */
        inclusion = bitmap_bitwise_check_inclusion(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( inclusion == true );

        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1100 0000 0001 1111 */

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, true);

        /* operation */
        inclusion = bitmap_bitwise_check_inclusion(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( inclusion == false );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_intersection test",
        "[bitmap][bitmap_bitwise_check_intersection]"
)
{
    static BITMAP_VAR(bitmap_a11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_b11, BITMAP_SIZE11);

    P_prepare_fill0(bitmap_a11, BITMAP_SIZE11);
    P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

    {
        /* prepare */
        bitmap_bitwise_set(bitmap_a11, false, BITMAP_SIZE11);

        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1000 0000 0001 1111 */

        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 1, true);

        bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 2, true);

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, true);

        bool intersection;
        /* operation */
        intersection = bitmap_bitwise_check_intersection(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( intersection == false );

        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1100 0000 0001 1111 */

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 1, true);

        /* operation */
        intersection = bitmap_bitwise_check_intersection(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( intersection == true );
    }
}

TEST_CASE(
        "bitmaps bitmap_bitwise_check_relation test",
        "[bitmap][bitmap_bitwise_check_relation]"
)
{
    static BITMAP_VAR(bitmap_a11, BITMAP_SIZE11);
    static BITMAP_VAR(bitmap_b11, BITMAP_SIZE11);

    /* prepare */
    bitmap_bitwise_set(bitmap_a11, false, BITMAP_SIZE11);

    bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 1, true);

    bitmap_bit_set(bitmap_a11, BITMAP_SIZE11, 2, true);

    {
        /* BITMAP_RELATION__EQUAL */
        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 0110 0000 0001 1111 */
        /* b = 1110 0000 0001 1111 */

        /* prepare */
        P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 1, true);
        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 2, true);

        bitmap_relation_t relation;
        /* operation */
        relation = bitmap_bitwise_check_relation(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( relation == BITMAP_RELATION__EQUAL );

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, true);

        /* operation */
        relation = bitmap_bitwise_check_relation(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( relation == BITMAP_RELATION__INTERSECTION );
    }

    {
        /* BITMAP_RELATION__INCLUSION */
        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 0010 0000 0001 1111 */
        /* b = 1010 0000 0001 1111 */

        /* prepare */
        P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 2, true);

        bitmap_relation_t relation;
        /* operation */
        relation = bitmap_bitwise_check_relation(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( relation == BITMAP_RELATION__INCLUSION );

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, true);

        /* operation */
        relation = bitmap_bitwise_check_relation(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( relation == BITMAP_RELATION__INTERSECTION );
    }

    {
        /* BITMAP_RELATION__INTERSECTION */
        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1010 0000 0001 1111 */
        /* b = 0010 0000 0001 1111 */

        /* prepare */
        P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 2, true);

        bitmap_relation_t relation;
        /* operation */
        relation = bitmap_bitwise_check_relation(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( relation == BITMAP_RELATION__INTERSECTION );

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, false);

        /* operation */
        relation = bitmap_bitwise_check_relation(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( relation == BITMAP_RELATION__INCLUSION );
    }

    {
        /* BITMAP_RELATION__DIFFERENT */
        /*     xxxx xxxx xxx. .... */
        /* a = 0110 0000 0000 0000 */
        /* b = 1001 0000 0001 1111 */

        /* prepare */
        P_prepare_fill0(bitmap_b11, BITMAP_SIZE11);

        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 0, true);
        bitmap_bit_set(bitmap_b11, BITMAP_SIZE11, 3, true);

        bitmap_relation_t relation;
        /* operation */
        relation = bitmap_bitwise_check_relation(bitmap_a11, bitmap_b11, BITMAP_SIZE11);
        CHECK( relation == BITMAP_RELATION__DIFFERENT );
    }
}

TEST_CASE(
        "bitmaps bitmap_bit_nearest_forward_set_get test",
        "[bitmap][bitmap_bit_nearest_forward_set_get]"
)
{
    {
        static BITMAP_VAR(bitmap11, BITMAP_SIZE11);

        /* prepare */
        P_prepare_fill0(bitmap11, BITMAP_SIZE11);

        /*          xxxx xxxx xxx. .... */
        /* bitmap = 1010 1010 1011 1111 */

        size_t indexes[] = { 0, 2, 4, 6, 8, 10 };
        size_t indexes2[] = { 0, 2, 4, 6, 8, 10, 0, 0, 0, 0, 0 };

        size_t len = ARRAY_SIZE(indexes);

        size_t i;
        for(i = 0; i < len; ++i)
        {
            size_t index = indexes[i];
            bitmap_bit_set(bitmap11, BITMAP_SIZE11, index, true);
        }

        static const uint8_t pattern11[2] = { 0x55, 0xFD };
        CHECK( memcmp(bitmap11, pattern11, sizeof(pattern11)) == 0 );

        /* operation */
        bitmap_foreach_bit_context_t ctx;

        size_t ibit;
        i = 0;
        BITMAP_FOREACH_BIT_IN_BITMAP(&ibit, bitmap11, BITMAP_SIZE11, &ctx)
        {
            CHECK( ibit == indexes2[i] );
            ++i;
        }
        CHECK( i == ARRAY_SIZE(indexes) );
    }

    {
        static BITMAP_VAR(bitmap16, BITMAP_SIZE16);

        /* prepare */
        bitmap_bitwise_set(bitmap16, false, BITMAP_SIZE16);

        /*          xxxx xxxx xxxx xxxx */
        /* bitmap = 1010 1010 1010 1111 */

        size_t indexes[] = { 0, 2, 4, 6, 8, 10 , 12, 13, 14, 15};
        size_t indexes2[] = { 0, 2, 4, 6, 8, 10, 12, 13, 14, 15};

        size_t len = ARRAY_SIZE(indexes);

        size_t i;
        for(i = 0; i < len; ++i)
        {
            size_t index = indexes[i];
            bitmap_bit_set(bitmap16, BITMAP_SIZE16, index, true);
        }

        static const uint8_t pattern16[2] = { 0x55, 0xF5 };
        CHECK( memcmp(bitmap16, pattern16, sizeof(pattern16)) == 0 );

        /* operation */
        bitmap_foreach_bit_context_t ctx;

        size_t ibit;
        i = 0;
        BITMAP_FOREACH_BIT_IN_BITMAP(&ibit, bitmap16, BITMAP_SIZE16, &ctx)
        {
            CHECK( ibit == indexes2[i] );
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
        static BITMAP_VAR(bitmap11, BITMAP_SIZE11);

        /*          xxxx xxxx xxx. .... */
        /* bitmap = 0000 0000 0000 0000 */
        static const char * str_pattern = "";

        P_prepare_fill0(bitmap11, BITMAP_SIZE11);

        status = bitmap_snprintf_ranged(str, STR_SIZE, bitmap11, BITMAP_SIZE11, enum_marker, range_marker);
        REQUIRE( status == 0 );

        CHECK( strncmp(str, str_pattern, STR_SIZE) == 0 );
    }
    {
        static BITMAP_VAR(bitmap11, BITMAP_SIZE11);

        /*          xxxx xxxx xxx. .... */
        /* bitmap = 1010 1010 1010 0000 */
        size_t indexes[] = { 0, 2, 4, 6, 8, 10 };
        static const char * str_pattern = "0, 2, 4, 6, 8, 10";

        P_prepare_bitmap(indexes, ARRAY_SIZE(indexes), bitmap11, BITMAP_SIZE11);

        status = bitmap_snprintf_ranged(str, STR_SIZE, bitmap11, BITMAP_SIZE11, enum_marker, range_marker);
        REQUIRE( status == 0 );

        CHECK( strncmp(str, str_pattern, STR_SIZE) == 0 );
    }
    {
        static BITMAP_VAR(bitmap11, BITMAP_SIZE11);

        /*          xxxx xxxx xxx. .... */
        /* bitmap = 0101 1011 1110 0000 */
        size_t indexes[] = { 1, 3, 4, 6, 7, 8, 9, 10 };
        static const char * str_pattern = "1, 3, 4, 6 - 10";

        P_prepare_bitmap(indexes, ARRAY_SIZE(indexes), bitmap11, BITMAP_SIZE11);

        status = bitmap_snprintf_ranged(str, STR_SIZE, bitmap11, BITMAP_SIZE11, enum_marker, range_marker);
        REQUIRE( status == 0 );

        CHECK( strncmp(str, str_pattern, STR_SIZE) == 0 );
    }
    {
        static BITMAP_VAR(bitmap11, BITMAP_SIZE11);

        /*          xxxx xxxx xxx. .... */
        /* bitmap = 1111 1010 1000 0000 */
        size_t indexes[] = { 0, 1, 2, 3, 4, 6, 8 };
        static const char * str_pattern = "0 - 4, 6, 8";

        P_prepare_bitmap(indexes, ARRAY_SIZE(indexes), bitmap11, BITMAP_SIZE11);

        status = bitmap_snprintf_ranged(str, STR_SIZE, bitmap11, BITMAP_SIZE11, enum_marker, range_marker);
        REQUIRE( status == 0 );

        CHECK( strncmp(str, str_pattern, STR_SIZE) == 0 );
    }

#undef STR_SIZE
}
