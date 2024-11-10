//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#ifndef BOOST_PUNYCODE_UTF8_OUTPUT_HPP
#define BOOST_PUNYCODE_UTF8_OUTPUT_HPP

#include <boost/punycode/detail/config.hpp>
#include <boost/punycode/detail/except.hpp>
#include <cstddef>
#include <iterator>

namespace boost {
namespace punycode {

// utf32 output iterator which
// emits utf8 code points
class utf8_output
{
    char* dest_;

    // VFALCO should track end_ for assertions

public:
    using value_type        = char32_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using iterator_category =
        std::output_iterator_tag;

    explicit utf8_output(
        char* dest) noexcept
        : dest_(dest)
    {
    }

    utf8_output&
    operator=(char32_t cp) noexcept;

    utf8_output&
    operator*() noexcept
    {
        return *this;
    }

    utf8_output&
    operator++() noexcept
    {
        return *this;
    }

    utf8_output&
    operator++(int) noexcept
    {
        return *this;
    }
};

inline
utf8_output&
utf8_output::
operator=(char32_t cp) noexcept
{               
    if(cp < 0x80)
    {
        *dest_++ = static_cast<char>(cp);
    }
    else if(cp < 0x800)
    {
        *dest_++ = static_cast<char>( (cp >> 6)          | 0xc0);
        *dest_++ = static_cast<char>( (cp & 0x3f)        | 0x80);
    }
    else if(cp < 0x10000)
    {
        *dest_++ = static_cast<char>( (cp >> 12)         | 0xe0);
        *dest_++ = static_cast<char>(((cp >> 6) & 0x3f)  | 0x80);
        *dest_++ = static_cast<char>( (cp       & 0x3f)  | 0x80);
    }
    else
    {
        *dest_++ = static_cast<char>( (cp >> 18)         | 0xf0);
        *dest_++ = static_cast<char>(((cp >> 12) & 0x3f) | 0x80);
        *dest_++ = static_cast<char>(((cp >> 6)  & 0x3f) | 0x80);
        *dest_++ = static_cast<char>( (cp        & 0x3f) | 0x80);
    }
    return *this;
}

} // punycode
} // boost

#endif
