#include "vv/result.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("result")
{
    SUBCASE("is_ok")
    {
        vv::result<int, int> opt = vv::ok(1);
        CHECK(opt.is_ok());
    }
}