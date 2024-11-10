//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

// Test that header file is self-contained.
#include <boost/punycode/encode.hpp>

#include "test_suite.hpp"

namespace boost {
namespace punycode {

struct encode_test
{
    void
    run()
    {
    }
};

TEST_SUITE(
    encode_test,
    "boost.punycode.encode");

} // punycode
} // boost
