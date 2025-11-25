#include "vv/option.hpp"
#include <winerror.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <utility>

struct ctor_counters
{
    size_t default_ctor = 0;
    size_t copy_ctor = 0;
    size_t copy_assign = 0;
    size_t move_ctor = 0;
    size_t move_assign = 0;
    size_t dtor = 0;
};

struct object_t
{
    object_t(bool& o) : observer(&o) { *observer = true; }

    object_t(const object_t&) = delete;

    object_t(object_t&& other) : observer(std::exchange(other.observer, nullptr)) {}

    ~object_t()
    {
        if (observer)
        {
            *observer = false;
        }
    }

    bool* observer;
};

TEST_CASE("option")
{
    SUBCASE("is_some")
    {
        vv::option<int> opt = vv::some(1);
        REQUIRE(opt.is_some());
        CHECK_EQ(*opt, 1);
    }

    SUBCASE("is_none")
    {
        vv::option<int> opt = vv::none;
        CHECK(opt.is_none());
    }

    SUBCASE("rebinding some to none, etc.")
    {
        vv::option<int> opt = vv::some(1);
        REQUIRE(opt.is_some());
        CHECK_EQ(*opt, 1);

        opt = vv::none;
        CHECK(opt.is_none());

        opt = vv::some(2);
        REQUIRE(opt.is_some());
        CHECK_EQ(*opt, 2);

        opt = vv::none;
        CHECK(opt.is_none());
    }

    SUBCASE("lifetime observation")
    {
        SUBCASE("scope exit")
        {
            bool observer = false;

            {
                vv::option opt = vv::some<object_t>(std::in_place, observer);
                CHECK(observer == true);
            }

            CHECK(observer == false);
        }

        SUBCASE("rebind")
        {
            bool observer1 = false;
            bool observer2 = false;

            vv::option opt = vv::some<object_t>(std::in_place, observer1);
            CHECK(observer1 == true);
            CHECK(observer2 == false);

            opt = vv::some<object_t>(std::in_place, observer2);
            CHECK(observer1 == false);
            CHECK(observer2 == true);

            opt = vv::some<object_t>(std::in_place, observer1);
            CHECK(observer1 == true);
            CHECK(observer2 == false);

            opt = vv::none;
            CHECK(observer1 == false);
            CHECK(observer2 == false);
        }
    }

    SUBCASE("bool conversion")
    {
        if (vv::option opt = vv::some(1))
        {
            // pass
        }
        else
        {
            FAIL("incorrect evaluation");
        }
    }

    SUBCASE("access")
    {
        struct S
        {
            int a;
            int b;

            bool operator==(const S&) const = default;
        };

        vv::option opt = vv::some(S{0xA, 0xB});
        REQUIRE(opt.is_some());
        CHECK_EQ(*opt, S{0xA, 0xB});
        CHECK_EQ(opt.value(), S{0xA, 0xB});
        CHECK_EQ(opt->a, 0xA);
        CHECK_EQ(opt->b, 0xB);
    }
}
