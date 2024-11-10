//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#ifndef BOOST_PUNYCODE_DETAIL_CONFIG_HPP
#define BOOST_PUNYCODE_DETAIL_CONFIG_HPP

#include <boost/config.hpp>

namespace boost {
namespace punycode {

//------------------------------------------------

#if defined(BOOST_PUNYCODE_DOCS)
# define BOOST_PUNYCODE_DECL
#else
# if (defined(BOOST_PUNYCODE_DYN_LINK) || defined(BOOST_ALL_DYN_LINK)) && !defined(BOOST_PUNYCODE_STATIC_LINK)
#  if defined(BOOST_PUNYCODE_SOURCE)
#   define BOOST_PUNYCODE_DECL        BOOST_SYMBOL_EXPORT
#   define BOOST_PUNYCODE_BUILD_DLL
#  else
#   define BOOST_PUNYCODE_DECL        BOOST_SYMBOL_IMPORT
#  endif
# endif // shared lib

# ifndef  BOOST_PUNYCODE_DECL
#  define BOOST_PUNYCODE_DECL
# endif

# if !defined(BOOST_PUNYCODE_SOURCE) && !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_PUNYCODE_NO_LIB)
#  define BOOST_LIB_NAME boost_punycode
#  if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_PUNYCODE_DYN_LINK)
#   define BOOST_DYN_LINK
#  endif
#  include <boost/config/auto_link.hpp>
# endif
#endif

//------------------------------------------------

// source location
#ifdef BOOST_PUNYCODE_NO_SOURCE_LOCATION
# define BOOST_PUNYCODE_POS ::boost::source_location()
#else
# define BOOST_PUNYCODE_POS (BOOST_CURRENT_LOCATION)
#endif

} // punycode
} // boost

#endif
