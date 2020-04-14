/**
 * @file bitmap_io.c
 */

#include <bitmap/bitmap.h>

#include "bitmap_common.h"

#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

typedef struct
{
    ssize_t begin;
    ssize_t end;
} bitmap_srange_t;

/**
 * @brief Добавление форматной строки с аргументами к строке с проверкой размера
 * @param[i/o] str_ptr      Указатель на строку, к которой происходит добавление. Модифицируется! (char *)
 * @param[i/o] str_rest     Сколько свобоных символов осталось в str_ptr. Модифицируется! (ssize_t)
 * @return len              >= 0 - Записаная длина; < 0 - согласно спецификации snprintf (сколько не хватило для записи?) (ssize_t)
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
        const bitmap_srange_t * BITMAP_RESTRICT range,
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

int bitmap_snprintf_ranged6(
        char * BITMAP_RESTRICT dest,
        size_t size,
        const bitmap_block_t * BITMAP_RESTRICT bitmap,
        size_t bits_num,
        const char * BITMAP_RESTRICT enum_marker,
        const char * BITMAP_RESTRICT range_marker
)
{
    int res = 0;

    char * bits_str_ptr = dest;
    ssize_t rest = size;

    /* protect */
    if(size == 0)
    {
        return 0;
    }
    bits_str_ptr[0] = '\0';

    bitmap_srange_t range = {-1, -1};
    bool first_print = true;

    size_t iteration = 0;
    size_t ibit;
    bitmap_foreach_bit_context_t ctx;
    BITMAP_FOREACH_BIT_IN_BITMAP(&ibit, bitmap, bits_num, &ctx)
    {

        if(iteration == 0)
        {
            range.begin = ibit;
        }
        else
        {
            bool range_interrupted = (range.end + 1 != (ssize_t)ibit);
            if(range_interrupted)
            {
                res = P_snprintf_range(first_print, &bits_str_ptr, &rest, &range, enum_marker, range_marker);
                if(res) goto end;

                first_print = false;

                range.begin = ibit;
            }
        }

        range.end = ibit;

        ++iteration;
    }

    if(range.begin >= 0 && range.end >= 0)
    {
        res = P_snprintf_range(first_print, &bits_str_ptr, &rest, &range, enum_marker, range_marker);
        if(res) goto end;
    }

    end:

    if(size > 0)
    {
        dest[size - 1] = '\0';
    }

    return res;
}

int bitmap_sscanf_append_ranged5(
        bitmap_block_t * BITMAP_RESTRICT bitmap,
        size_t bits_num,
        char enum_marker,
        char range_marker,
        const char * BITMAP_RESTRICT src
)
{
#define CONVERT_CHAR_TO_DIGIT(xch)  ((xch) - '0')
#define INIT(xvalue, ch) \
        ((*xvalue) = CONVERT_CHAR_TO_DIGIT(ch))
#define APPEND(xvalue, ch) \
        ((*xvalue) = (*xvalue) * 10 + CONVERT_CHAR_TO_DIGIT(ch))

#define CHECK(xvalue, xbits_num) \
        do { \
            if((*xvalue) >= (xbits_num)) \
            { \
                return -1; \
            } \
        } while(0)

#define INIT_AND_CHECK(xvalue, ch, xbits_num) \
        do { \
            INIT(xvalue, ch); \
            CHECK(xvalue, xbits_num); \
        } while(0)

#define APPEND_AND_CHECK(xvalue, ch, xbits_num) \
        do { \
            APPEND(xvalue, ch); \
            CHECK(xvalue, xbits_num); \
        } while(0)

    enum state
    {
        ST_DIGIT_FIRST,
        ST_DIGIT_NEXT,
        ST_DIGIT_AFTER_MARKER_ENUM,
        ST_DIGIT_AFTER_MARKER_RANGE,
        ST_AWAIT_MARKER,
    };

    bitmap_range_t range;

    enum state state = ST_DIGIT_FIRST;

    size_t * value = &range.begin;
    while(1)
    {
        char ch = *src;
        switch(state)
        {
            case ST_DIGIT_FIRST:
            {
                if(ch == enum_marker)
                {
                    return -1;
                }
                else if(ch == range_marker)
                {
                    return -1;
                }
                else if(isdigit(ch))
                {
                    INIT_AND_CHECK(value, ch, bits_num);
                    state = ST_DIGIT_NEXT;
                }
                else if(isspace(ch))
                {
                    /* skip */
                }
                else if(ch == '\0')
                {
                    return 0;
                }
                else
                {
                    return -1;
                }
                break;
            }
            case ST_DIGIT_NEXT:
            {
                if(ch == enum_marker)
                {
                    if(value == &range.begin)
                    {
                        range.end = range.begin;
                    }
                    bitmap_bitwise_range_raise2(bitmap, &range);
                    state = ST_DIGIT_AFTER_MARKER_ENUM;
                }
                else if(ch == range_marker)
                {
                    state = ST_DIGIT_AFTER_MARKER_RANGE;
                }
                else if(isdigit(ch))
                {
                    APPEND_AND_CHECK(value, ch, bits_num);
                }
                else if(isspace(ch))
                {
                    state = ST_AWAIT_MARKER;
                }
                else if(ch == '\0')
                {
                    if(value == &range.begin)
                    {
                        range.end = range.begin;
                    }
                    bitmap_bitwise_range_raise2(bitmap, &range);
                    return 0;
                }
                else
                {
                    return -1;
                }
                break;
            }
            case ST_AWAIT_MARKER:
            {
                if(ch == enum_marker)
                {
                    state = ST_DIGIT_AFTER_MARKER_ENUM;
                }
                else if(ch == range_marker)
                {
                    state = ST_DIGIT_AFTER_MARKER_RANGE;
                }
                else if(isdigit(ch))
                {
                    return -1;
                }
                else if(isspace(ch))
                {
                    /* skip */
                }
                else if(ch == '\0')
                {
                    if(value == &range.begin)
                    {
                        range.end = range.begin;
                    }
                    bitmap_bitwise_range_raise2(bitmap, &range);
                    return 0;
                }
                else
                {
                    return -1;
                }
                break;
            }
            case ST_DIGIT_AFTER_MARKER_ENUM:
            {
                if(ch == enum_marker)
                {
                    return -1;
                }
                else if(ch == range_marker)
                {
                    return -1;
                }
                else if(isdigit(ch))
                {
                    value = &range.begin;
                    INIT_AND_CHECK(value, ch, bits_num);
                    state = ST_DIGIT_NEXT;
                }
                else if(isspace(ch))
                {
                    /* skip */
                }
                else if(ch == '\0')
                {
                    return 0;
                }
                else
                {
                    return -1;
                }
                break;
            }
            case ST_DIGIT_AFTER_MARKER_RANGE:
            {
                if(ch == enum_marker)
                {
                    return -1;
                }
                else if(ch == range_marker)
                {
                    return -1;
                }
                else if(isdigit(ch))
                {
                    value = &range.end;
                    INIT_AND_CHECK(value, ch, bits_num);
                    state = ST_DIGIT_NEXT;
                }
                else if(isspace(ch))
                {
                    /* skip */
                }
                else if(ch == '\0')
                {
                    return 0;
                }
                else
                {
                    return -1;
                }
                break;
            }
        }
        ++src;
    }

    return 0;
}
