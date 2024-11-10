//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#ifndef BOOST_PUNYCODE_UTF8_COUNT_HPP
#define BOOST_PUNYCODE_UTF8_COUNT_HPP

#include <boost/punycode/detail/config.hpp>
#include <cstddef>
#include <iterator>

namespace boost {
namespace punycode {

// utf32 output iterator which
// counts utf8 code points
class utf8_count
{
    std::size_t n_ = 0;

public:
    using value_type        = char32_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using iterator_category =
        std::output_iterator_tag;

    std::size_t
    count() const noexcept
    {
        return n_;
    }

    utf8_count&
    operator=(char32_t cp) noexcept
    {
        if(cp < 0x80)
            n_ += 1;
        else if(cp < 0x800)
            n_ += 2;
        else if(cp < 0x10000)
            n_ += 3;
        else
            n_ += 4;
        return *this;
    }

    utf8_count&
    operator*() noexcept
    {
        return *this;
    }

    utf8_count&
    operator++() noexcept
    {
        return *this;
    }

    utf8_count&
    operator++(int) noexcept
    {
        return *this;
    }
};

} // punycode
} // boost

#endif
