//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#include "src/stringprep_b2.hpp"
#include <boost/punycode/idna.hpp>
#include <boost/punycode/punycode.hpp>
#include <boost/punycode/utf8_count.hpp>
#include <boost/punycode/utf8_input.hpp>
#include <boost/punycode/utf8_output.hpp>
#include <algorithm>

namespace boost {
namespace punycode {

//------------------------------------------------
//
// NAMEPREP
//
// https://datatracker.ietf.org/doc/html/rfc3491
//

class nameprep_iterator_base
{
protected:
    enum : char32_t
    {
        invalid = 0xffffffff
    };

    char32_t cp_[4];
    char i_ = 0;

    static constexpr bool
    map_to_nothing(char32_t cp) noexcept;

    struct writer
    {
        nameprep_iterator_base& self;

        void operator()(
            char32_t cp) noexcept
        {
            self.cp_[0] = cp;
            self.i_ = 0;
        }

        void operator()(
            char32_t cp0,
            char32_t cp1) noexcept
        {
            self.cp_[1] = cp0;
            self.cp_[0] = cp1;
            self.i_ = 1;
        }

        void operator()(
            char32_t cp0,
            char32_t cp1,
            char32_t cp2) noexcept
        {
            self.cp_[2] = cp0;
            self.cp_[1] = cp1;
            self.cp_[0] = cp2;
            self.i_ = 2;
        }

        void operator()(
            char32_t cp0,
            char32_t cp1,
            char32_t cp2,
            char32_t cp3) noexcept
        {
            self.cp_[3] = cp0;
            self.cp_[2] = cp1;
            self.cp_[1] = cp2;
            self.cp_[0] = cp3;
            self.i_ = 3;
        }
    };
};

//------------------------------------------------

// https://datatracker.ietf.org/doc/html/rfc3454#appendix-B.1
constexpr bool
nameprep_iterator_base::
map_to_nothing(char32_t cp) noexcept
{
    switch(cp)
    {
    case 0x00AD:
    case 0x034F:
    case 0x1806:
    case 0x180B:
    case 0x180C:
    case 0x180D:
    case 0x200B:
    case 0x200C:
    case 0x200D:
    case 0x2060:
    case 0xFE00:
    case 0xFE01:
    case 0xFE02:
    case 0xFE03:
    case 0xFE04:
    case 0xFE05:
    case 0xFE06:
    case 0xFE07:
    case 0xFE08:
    case 0xFE09:
    case 0xFE0A:
    case 0xFE0B:
    case 0xFE0C:
    case 0xFE0D:
    case 0xFE0E:
    case 0xFE0F:
    case 0xFEFF:
        return true;
    default:
        break;
    }
    return false;
}

//------------------------------------------------

/** Iterate over a utf32 sequence, applying nameprep.
*/
template<class InputIt>
class nameprep_iterator
    : private nameprep_iterator_base
{
    // VFALCO typecheck on InputIt

    InputIt it_;
    InputIt end_; // VFALCO only needed for debug?

public:
    using value_type        = char32_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using iterator_category =
        std::forward_iterator_tag;

    nameprep_iterator(nameprep_iterator const&) = default;
    nameprep_iterator& operator=(nameprep_iterator const&) = default;

    nameprep_iterator() = default;

    nameprep_iterator(
        InputIt it,
        InputIt end)
        : it_(it)
        , end_(end)
    {
        get();
    }

    explicit
    nameprep_iterator(
        InputIt end)
        : it_(end)
        , end_(end)
    {
        cp_[0] = invalid;
        i_ = 0;
    }

    bool
    operator==(
        nameprep_iterator const& other) const noexcept
    {
        BOOST_ASSERT(end_ == other.end_);
        return
            it_ == other.it_ &&
            i_ == other.i_;
    }

    bool
    operator!=(
        nameprep_iterator const& other) const noexcept
    {
        return ! (*this == other);
    }

    char32_t
    operator*() const noexcept
    {
        BOOST_ASSERT(i_ > 0 || cp_[i_] != invalid);
        return cp_[i_];
    }

    nameprep_iterator&
    operator++()
    {
        BOOST_ASSERT(i_ > 0 || cp_[i_] != invalid);
        if(i_ > 0)
        {
            --i_;
            return *this;
        }
        ++it_;
        get();
        return *this;
    }

    nameprep_iterator
    operator++(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

private:
    void
    get()
    {
        while(it_ != end_)
        {
            auto cp = *it_;
            if(map_to_nothing(cp))
            {
                ++it_;
                continue;
            }
            return stringprep_b2(cp, writer{*this});
        }
        cp_[0] = invalid;
        i_ = 0;
    }
};

//------------------------------------------------

static
bool
is_ascii(char32_t cp)
{
    return cp < 128;
}

/** Write a utf32 IDNA from a utf32 IRI
*/
template<
    class OutputIt,
    class InputIt>
OutputIt
encode_idna(
    OutputIt out,
    InputIt first0,
    InputIt last0)
{
    if(first0 == last0)
        return out;
    nameprep_iterator<InputIt> first(first0, last0);
    nameprep_iterator<InputIt> last(last0);
    auto it = first;
    for(;;)
    {
        auto cp = *it;
        if(cp == '.')
        {
            // copy ascii
            out = std::copy(first, it, out);
            *out++ = '.';
            ++it;
            if(it == last)
                return out;
            first = it;
            continue;
        }
        if(is_ascii(cp))
        {
            ++it;
            if(it == last)
            {
                // copy ascii
                out = std::copy(first, it, out);
                return out;
            }
            continue;
        }
        for(;;)
        {
            ++it;
            if(it == last)
            {
                // convert puny
                *out++ = 'x';
                *out++ = 'n';
                *out++ = '-';
                *out++ = '-';
                out = encode(out, first, it);
                return out;
            }
            cp = *it;
            if(cp == '.')
            {
                // convert puny
                *out++ = 'x';
                *out++ = 'n';
                *out++ = '-';
                *out++ = '-';
                out = encode(out, first, it);
                *out++ = '.';
                ++it;
                if(it == last)
                    return out;
                first = it;
                break;
            }
        }
    }
}

system::result<std::string>
utf8_to_idna(
    core::string_view s,
    std::string&& storage)
{
    utf8_input const first(
        s.data(), s.data() + s.size());
    utf8_input const last(
        s.data() + s.size());
    storage.resize(encode_idna(
        utf8_count(), first, last).count());
    encode_idna(
        utf8_output(&storage[0]), first, last);
    return std::move(storage);
}

} // url
} // boost

