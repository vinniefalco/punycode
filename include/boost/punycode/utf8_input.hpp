//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#ifndef BOOST_PUNYCODE_UTF8_INPUT_HPP
#define BOOST_PUNYCODE_UTF8_INPUT_HPP

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

// utf32 forward iterator which
// reads utf8 code points
class utf8_input
{
    static char32_t const invalid =
        0xffffffff;

    char const* s_;
    char const* end_;
    char32_t cp_;

public:
    using value_type        = char32_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using iterator_category =
        std::forward_iterator_tag;

    utf8_input(
        char const* s,
        char const* end)
        : s_(s)
        , end_(end)
    {
        next();
    }

    explicit
    utf8_input(
        char const* s) noexcept
        : s_(s)
        , end_(s)
        , cp_(invalid)
    {
    }

    bool
    operator==(
        utf8_input const& other) const noexcept
    {
        return
            s_ == other.s_ &&
            end_ == other.end_ &&
            cp_ == other.cp_;
    }

    bool
    operator!=(
        utf8_input const& other) const noexcept
    {
        return !(*this == other);
    }

    char32_t
    operator*() const noexcept
    {
        BOOST_ASSERT(cp_ != invalid);
        return cp_;
    }

    utf8_input&
    operator++()
    {
        BOOST_ASSERT(cp_ != invalid);
        next();
        return *this;
    }

    utf8_input
    operator++(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

private:
    static
    constexpr
    unsigned char
    mask(char c)
    {
        return static_cast<
            unsigned char>(
                0xff & c);
    }

    static
    char32_t
    parse_utf8(
        char const*& in0,
        char const* end)
    {
        if(in0 >= end)
            punycode::detail::throw_invalid_argument(
                BOOST_PUNYCODE_POS);
        char32_t cp;
        char const* in = in0;

        auto const u = mask(*in);
        if(u < 0x80)
        {
            ++in;
            cp = u;
        }
        else if((u >> 5) == 0x06)
        {
            if(end - in < 2)
                punycode::detail::throw_invalid_argument(
                    BOOST_PUNYCODE_POS);
            cp =
                ((u << 6) & 0x7ff) +
                (mask(in[1]) & 0x3f);
            in += 2;
                 
        }
        else if((u >> 4) == 0x0e)
        {
            if(end - in < 3)
                punycode::detail::throw_invalid_argument(
                    BOOST_PUNYCODE_POS);
            cp =
                ((u << 12) & 0xffff) +
                ((mask(in[1]) << 6) & 0xfff) +
                (mask(in[2]) & 0x3f);
            in += 3;
        }
        else if((u >> 3) == 0x1e)
        {
            if(end - in < 4)
                punycode::detail::throw_invalid_argument(
                    BOOST_PUNYCODE_POS);
            cp =
                ((u << 18) & 0x1fffff) +
                ((mask(in[1]) << 12) & 0x3ffff) +
                ((mask(in[2]) << 6) & 0xfff) +
                (mask(in[3]) & 0x3f);
            in += 4;
        }
        else
        {
            punycode::detail::throw_invalid_argument(
                BOOST_PUNYCODE_POS);
        }
        in0 = in;
        return cp;
    }

    void
    next()
    {
        if(s_ == end_)
        {
            cp_ = invalid;
            return;
        }
        cp_ = parse_utf8(s_, end_);
    }
};

} // punycode
} // boost

#endif
