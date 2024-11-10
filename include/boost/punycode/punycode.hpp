//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//
/*  This is a derivative work.
    Original is copryght by Adam M. Costello:
    http://www.nicemice.net/idn/punycode-spec.gz
*/

#ifndef BOOST_PUNYCODE_PUNYCODE_HPP
#define BOOST_PUNYCODE_PUNYCODE_HPP

#include <boost/punycode/detail/config.hpp>
#include <boost/punycode/detail/except.hpp>
#include <boost/punycode/ascii_count.hpp>
#include <boost/assert.hpp>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits.h>

namespace boost {
namespace punycode {

namespace detail {

enum : std::size_t
{
    base = 36,
    tmin = 1,
    tmax = 26,
    skew = 38,
    damp = 700,
    initial_n = 128,
    initial_bias = 72
};

inline
std::size_t
adapt(
    std::size_t delta,
    std::size_t n_points,
    bool is_first) noexcept
{
    // scale back then increase delta
    delta /= is_first ? damp : 2;
    delta += delta / n_points;

    std::uint32_t k = 0;
    auto const lim =
        ((base - tmin) * tmax) / 2;
    while(delta > lim)
    {
        k += base;
        delta /= (base - tmin);
    }

    return k + (
        ((base - tmin + 1) * delta) /
        (delta + skew));
}

inline
char
encode_digit(std::size_t c)
{
    BOOST_ASSERT(c <= 35);
    // digit
    if(c > 25)
        return static_cast<
            char>(c + 22);
    // letter
    return static_cast<
        char>(c + 'a');
}

// write a variable length integer
template<class OutputIt>
OutputIt
encode_varint(
    OutputIt& dest, // in-out
    std::size_t bias,
    std::size_t delta) noexcept
{
    std::size_t k = base;
    std::size_t q = delta;
    for(;;)
    {
        std::size_t t;
        if(k <= bias)
            t = tmin;
        else if(k >= bias + tmax)
            t = tmax;
        else
            t = k - bias;
        if(q < t)
            break;
        *dest++ = encode_digit(
            t + (q - t) % (base - t));
        q = (q - t) / (base - t);
        k += base;
    }
    *dest++ = encode_digit(q);
    return dest;
}

// ascii version sans locale
constexpr inline bool
is_lower(
    char32_t cp) noexcept
{
    return static_cast<
        std::uint32_t>(cp) - 'a' < 26;
}

// ascii version sans locale
constexpr inline bool
is_upper(
    char32_t cp) noexcept
{
    return static_cast<
        std::uint32_t>(cp) - 'A' < 26;
}

inline
std::size_t
decode_digit(
    char32_t cp) noexcept
{
    auto const ucp = static_cast<
        std::uint32_t>(cp);
    if(ucp - 0x30 < 0x0a)
        return cp - 22;
    if(is_lower(cp))
        return cp - 'a';
    if(is_upper(cp))
        return cp - 'A';
    return SIZE_MAX;
}

//----------------------------------------------------------

} // detail

/*
inline
std::size_t
encode(
    void (*writef)(void*, char32_t),
    char32_t (*readf)(void*))
{
    return 0;
}
*/  

template<
    class OutputIt,
    class InputIt
>
OutputIt
encode(
    OutputIt dest,
    InputIt first,
    InputIt last)
{
    std::size_t b;
    std::size_t h;
    std::size_t delta;
    std::size_t bias;
    std::size_t n;

    std::size_t di = 0;
    std::size_t srclen = 0;

    // copy the low-ascii chars
    auto src = first;
    while(src != last)
    {
        ++srclen;
        auto const cp = *src++;
        if(cp < 128)
        {
            ++di;
            *dest++ =
                static_cast<
                    char>(cp);
        }
    }

    // VFALCO WHY?
    if(di >= srclen)
        return dest;

    b = di;
    h = di;

    // output delimiter if needed
    if(di > 0)
        *dest++ = '-';

    n = detail::initial_n;
    bias = detail::initial_bias;
    delta = 0;

    for(; h < srclen; n++, delta++)
    {
        // Find next smallest non-basic code point.
        std::size_t m;
        for(m = SIZE_MAX, src = first;
            src != last; ++src)
        {
            auto const cp = *src;
            if(cp >= n && cp < m)
                m = static_cast<std::size_t>(cp);
        }

        if((m - n) > (SIZE_MAX - delta) / (h + 1))
        {
            BOOST_ASSERT(0 && "OVERFLOW");
            goto fail;
        }

        delta += (m - n) * (h + 1);
        n = m;

        for(src = first; src != last; ++src)
        {
            auto const cp = *src;
            if(cp < n)
            {
                if(++delta == 0)
                {
                    BOOST_ASSERT(0 && "OVERFLOW");
                    goto fail;
                }
            }
            else if(cp == n)
            {
                dest = detail::encode_varint(
                    dest, bias, delta);
                bias = detail::adapt(delta, h + 1, h == b);
                delta = 0;
                h++;
            }
        }
    }

fail:
    return dest;
}

template<class InputIt>
std::size_t
encoded_size(
    InputIt first,
    InputIt last)
{
    std::size_t n = 0;
    encode(ascii_count{n}, first, last);
    return n;
}

inline
void
decode(
    char const* src,
    //char const* const last,
    const size_t srclen,
    char32_t* dest,
    size_t* const dstlen)
{
    char const* const begin = src;
    char const* const end = begin + srclen;

    // validate the input and also find the last '-'
    std::size_t delim_pos =
        [begin, end]()
        {
            std::size_t i = 0;
            std::size_t n = 0;
            auto src = begin;
            while(src != end)
            {
                if(*src & 0x80)
                {
                    // invalid high-ascii
                    punycode::detail::throw_invalid_argument(
                        BOOST_PUNYCODE_POS);
                }
                if(*src == '-')
                    n = i;
                ++i;
                ++src;
            }
            return n;
        }();

    /* Copy basic code points to output. */
    auto di = delim_pos;

    for(auto i = 0; i < di; i++)
        dest[i] = src[i];

    std::size_t i = 0;
    char32_t n = detail::initial_n;
    std::size_t bias = detail::initial_bias;

    // encoded digits
    auto csrc = src + delim_pos +
        ((delim_pos > 0) ? 1 : 0);

    for(;csrc < end && di < *dstlen; di++)
    {
        auto const i0 = i;
        for(std::size_t w = 1,
                k = detail::base;
            di < *dstlen;
            k += detail::base)
        {
            auto const digit =
                detail::decode_digit(*csrc++);
            if(digit == SIZE_MAX)
                goto fail;
            if(digit > (SIZE_MAX - i) / w)
            {
                BOOST_ASSERT(0 && "OVERFLOW");
                goto fail;
            }
            i += digit * w;
            std::size_t t;
            if(k <= bias)
                t = detail::tmin;
            else if(k >= bias + detail::tmax)
                t = detail::tmax;
            else
                t = k - bias;
            if(digit < t)
                break;
            if(w > SIZE_MAX / (detail::base - t))
            {
                BOOST_ASSERT(0 && "OVERFLOW");
                goto fail;
            }
            w *= detail::base - t;
        }

        bias = detail::adapt(
            i - i0,
            di + 1,
            i0 == 0);

        if(i / (di + 1) > SIZE_MAX - n)
        {
            BOOST_ASSERT(0 && "OVERFLOW");
            goto fail;
        }

        BOOST_ASSERT(
            i / (di + 1) < ULONG_MAX);
        n += static_cast<std::uint32_t>(
            i / (di + 1));
        i %= (di + 1);

        std::memmove(
            dest + i + 1,
            dest + i,
            (di - i) * sizeof(char32_t));
        dest[i++] = n;
    }

fail:
    *dstlen = di;
}

} // punycode
} // boost

#endif
