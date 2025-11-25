#pragma once

#include "macros.hpp"

namespace vv
{

template <class Fn>
class defer_t
{
public:
    constexpr defer_t(Fn& fn) noexcept : _fn(fn) {};
    constexpr ~defer_t() noexcept { _fn(); }

private:
    Fn& _fn;
};

} // namespace vv

#define _VV_DEFER_INSTANCE(fn, guard, ...)                                                         \
    auto fn = [&]() { __VA_ARGS__; };                                                              \
    vv::defer_t<decltype(fn)> guard(fn);

#define _VV_DEFER_COUNTED_INSTANCE(count, ...)                                                     \
    _VV_DEFER_INSTANCE(                                                                            \
        VV_CONCAT(vv_defer_fn_, count), VV_CONCAT(vv_defer_guard_, count), __VA_ARGS__             \
    )

#define VV_DEFER(...) _VV_DEFER_COUNTED_INSTANCE(__COUNTER__, __VA_ARGS__)
