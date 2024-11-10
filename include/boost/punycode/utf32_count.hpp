//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#ifndef BOOST_PUNYCODE_UTF32_COUNT_HPP
#define BOOST_PUNYCODE_UTF32_COUNT_HPP

#include <boost/punycode/detail/config.hpp>
#include <cstddef>
#include <iterator>

namespace boost {
namespace punycode {

// utf32 output iterator which
// counts utf32 code points
class utf32_count
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

    utf32_count&
    operator=(char32_t) noexcept
    {
        ++n_;
        return *this;
    }

    utf32_count&
    operator*() noexcept
    {
        return *this;
    }

    utf32_count&
    operator++() noexcept
    {
        return *this;
    }

    utf32_count&
    operator++(int) noexcept
    {
        return *this;
    }
};

} // punycode
} // boost

#endif
