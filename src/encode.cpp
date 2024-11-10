//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#include <boost/punycode/encode.hpp>
#include <boost/punycode/punycode.hpp>
#include <boost/punycode/utf8_count.hpp>
#include <boost/punycode/utf8_input.hpp>
#include <boost/punycode/utf8_output.hpp>
#include <algorithm>

namespace boost {
namespace punycode {

static
bool
is_ascii(char32_t cp)
{
    return cp < 128;
}

// convert the UTF32 domain name into a UTF32 IDNA
template<
    class OutIterator,
    class FwdIterator>
OutIterator
encode_idna(
    OutIterator out,
    FwdIterator first,
    FwdIterator last)
{
    if(first == last)
        return out;
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
    std::size_t n;
    utf8_input const first(
        s.data(), s.data() + s.size());
    utf8_input const last(
        s.data() + s.size());
    encode_idna(utf8_count(n), first, last );
    storage.resize(n);
    encode_idna(
        utf8_output(&storage[0]), first, last);
    return std::move(storage);
}

} // url
} // boost

