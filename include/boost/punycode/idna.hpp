//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#ifndef BOOST_PUNYCODE_IDNA_HPP
#define BOOST_PUNYCODE_IDNA_HPP

#include <boost/punycode/detail/config.hpp>
#include <boost/core/detail/string_view.hpp>
#include <boost/system/result.hpp>
#include <string>

namespace boost {
namespace punycode {

/** Return an IDNA for the given utf8-encoded domain.
*/
BOOST_PUNYCODE_DECL
system::result<std::string>
utf8_to_idna(
    core::string_view domain,
    std::string&& storage = std::string());

} // punycode
} // boost

#endif
