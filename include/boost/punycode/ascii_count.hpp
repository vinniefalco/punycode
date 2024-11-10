//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#ifndef BOOST_PUNYCODE_ASCII_COUNT_HPP
#define BOOST_PUNYCODE_ASCII_COUNT_HPP

#include <boost/punycode/detail/config.hpp>
#include <cstddef>
#include <iterator>

namespace boost {
namespace punycode {

/** Output iterator which counts characters written.
*/
struct ascii_count
{
    std::size_t* n_;

public:
    using value_type        = char;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using iterator_category =
        std::output_iterator_tag;

    ascii_count(ascii_count const&) = default;
    ascii_count& operator=(ascii_count const&) = default;

    explicit
    ascii_count(
        std::size_t& n) noexcept
        : n_(&n)
    {
        *n_ = 0;
    }

    ascii_count&
    operator=(char) noexcept
    {
        ++*n_;
        return *this;
    }

    ascii_count&
    operator*() noexcept
    {
        return *this;
    }

    ascii_count&
    operator++() noexcept
    {
        return *this;
    }

    ascii_count&
    operator++(int) noexcept
    {
        return *this;
    }
};

} // punycode
} // boost

#endif
