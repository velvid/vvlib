#pragma once

#include "relocate.hpp"

#include <new>
#include <utility>

namespace vv
{

struct none_t
{
    struct _tag
    {
    };

    constexpr none_t(_tag) {};
};

inline constexpr none_t none{none_t::_tag()};

template <class T>
class option
{
public:
    constexpr option() = delete;

    constexpr ~option()
    {
        if (_engaged) _value.~T();
    }

    constexpr option(const option& other) : _engaged(other._engaged)
    {
        if (other._engaged) ::new (&_value) T(other._value);
    }

    constexpr option& operator=(const option& other)
    {
        if (this == &other) return *this;

        // Invalidate current state.
        if (_engaged) _value.~T();

        // Copy state from other option.
        _engaged = other._engaged;
        if (other._engaged) ::new (&_value) T(other._value);

        return *this;
    }

    constexpr option(option&& other) : _engaged(other._engaged)
    {
        if (other._engaged) ::new (&_value) T(std::move(other._value));
    }

    constexpr option& operator=(option&& other)
    {
        if (this == &other) return *this;

        // Invalidate current state.
        if (_engaged) _value.~T();

        // Move state from option, and invalidate other value.
        _engaged = other._engaged;
        if (other._engaged) ::new (&_value) T(std::move(other._value));

        return *this;
    }

    constexpr option(none_t) : _engaged(false) {}

    constexpr option& operator=(none_t)
    {
        if (_engaged) _value.~T();
        _engaged = false;
        return *this;
    }

    constexpr void swap(option& other)
    {
        if (this == &other) return;

        if (_engaged && other._engaged)
        {
            std::swap(_value, other._value);
        }
        else if (_engaged) // && other._engaged
        {
            _engaged = false;
            other._engaged = true;

            vv::relocate_at(&other._value, &_value);
        }
        else if (other._engaged) // && _engaged
        {
            other._engaged = false;
            _engaged = true;

            vv::relocate_at(&_value, &other._value);
        }
        else // !engaged && !other._engaged
        {
            // Nothing to swap.
        }
    }

    constexpr explicit operator bool() const { return _engaged; }
    constexpr bool is_some() const { return _engaged; }
    constexpr bool is_none() const { return !_engaged; }

    constexpr auto operator->() -> T* { return &_value; }
    constexpr auto operator->() const-> const T* { return &_value; }

    constexpr auto operator*() & -> T& { return _value; }
    constexpr auto operator*() const& -> const T& { return _value; }
    constexpr auto operator*() && -> T&& { return std::move(_value); }
    constexpr auto operator*() const&& -> const T&& { return std::move(_value); }

    constexpr auto value() & -> T& { return _value; }
    constexpr auto value() const& -> const T& { return _value; }
    constexpr auto value() && -> T&& { return std::move(_value); }
    constexpr auto value() const&& -> const T&& { return std::move(_value); }

private:
    constexpr option(T&& value) : _value(std::move(value)), _engaged(true) {}

    template <class... Args>
    constexpr option(std::in_place_t, Args&&... args)
        : _value(std::forward<Args>(args)...), _engaged(true)
    {
    }

    template <class _T>
    friend constexpr auto some(_T&& value) -> option<std::decay_t<_T>>;

    template <class _T, class... Args>
    friend constexpr auto some(std::in_place_t, Args&&... args) -> option<std::decay_t<_T>>;

private:
    union {
        char _raw{};
        T _value;
    };
    bool _engaged{false};
};

template <class T>
option(T) -> option<T>;

template <class T>
constexpr auto some(T&& value) -> option<std::decay_t<T>>
{
    return option(std::move(value));
}

template <class T, class... Args>
constexpr auto some(std::in_place_t, Args&&... args) -> option<std::decay_t<T>>
{
    return option<std::decay_t<T>>(std::in_place, std::forward<Args>(args)...);
}

}; // namespace vv
