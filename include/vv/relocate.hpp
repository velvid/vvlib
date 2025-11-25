#pragma once

#include "defer.hpp"

#include <new>
#include <type_traits>

namespace vv
{

template <class T>
struct is_trivially_reloctable
{
    // This is by no means perfect.
    static constexpr bool value =
        std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>;
};

template <class T>
inline constexpr bool is_trivially_reloctable_v = is_trivially_reloctable<T>::value;

template <class To, class From>
    requires std::is_convertible_v<From, To>
constexpr To relocate(From& src)
{
    if constexpr (is_trivially_reloctable_v<From> && is_trivially_reloctable_v<To>)
    {
        return src;
    }
    else
    {
        VV_DEFER(src.~From());
        alignas(alignof(To)) unsigned char dest[sizeof(To)];
        return std::move(*(::new (&dest) To(std::move(src))));
    }
}

template <class To, class From>
    requires std::is_convertible_v<From, To>
constexpr To* relocate_at(To* dest, From* src)
{
    if constexpr (is_trivially_reloctable_v<From> && is_trivially_reloctable_v<To>)
    {
        *dest = *src;
        return dest;
    }
    else
    {
        VV_DEFER(src->~From());
        return ::new (dest) To(std::move(*src));
    }
}

}; // namespace vv
