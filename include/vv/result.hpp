#pragma once

#include "relocate.hpp"

#include <new>
#include <utility>

namespace vv
{

template <class T, class E>
class result;

template <class T>
class ok_t;

template <class E>
class err_t;

template <class T>
class ok_t
{
private:
    template <class _T, class _E>
    friend class result;

    template <class _T>
    friend constexpr auto ok(_T&& value) -> ok_t<std::decay_t<_T>>;

    template <class _T>
    constexpr ok_t(_T&& value) : _value(std::forward<_T>(value)){};

    constexpr T& operator*() & { return _value; }
    constexpr const T& operator*() const& { return _value; }
    constexpr T&& operator*() && { return std::move(_value); }
    constexpr const T& operator*() const&& { return std::move(_value); }

private:
    T _value;
};

template <class E>
class err_t
{
private:
    template <class _T, class _E>
    friend class result;

    template <class _E>
    friend constexpr auto err(_E&& error) -> err_t<std::decay_t<_E>>;

    template <class _E>
    constexpr err_t(_E&& value) : _value(std::forward<_E>(value)){};

    constexpr E& operator*() & { return _value; }
    constexpr const E& operator*() const& { return _value; }
    constexpr E&& operator*() && { return std::move(_value); }
    constexpr const E& operator*() const&& { return std::move(_value); }

private:
    E _value;
};

template <class T>
ok_t(T) -> ok_t<T>;

template <class E>
err_t(E) -> err_t<E>;

template <class T, class E>
class result
{
public:
    constexpr result() = delete;

    constexpr ~result() { _is_error ? _error.~E() : _value.~T(); }

    constexpr result(const result& other) : _is_error(other._is_error)
    {
        if (_is_error)
            ::new (&_error) E(other._error);
        else
            ::new (&_value) T(other._value);
    }

    constexpr result& operator=(const result& other)
    {
        if (this == &other) return *this;

        _is_error ? _error.~E() : _value.~T();

        _is_error = other._is_error;
        if (_is_error)
            ::new (&_error) E(other._error);
        else
            ::new (&_value) T(other._value);

        return *this;
    }

    constexpr result(result&& other) : _is_error(other._is_error)
    {
        if (_is_error)
            ::new (&_error) E(std::move(other._error));
        else
            ::new (&_value) T(std::move(other._value));
    }

    constexpr result& operator=(result&& other)
    {
        if (this == &other) return *this;

        _is_error ? _error.~E() : _value.~T();

        _is_error = other._is_error;
        if (_is_error)
            ::new (&_error) E(std::move(other._error));
        else
            ::new (&_value) T(std::move(other._value));

        return *this;
    }

    template <class _T>
    constexpr result(ok_t<_T>&& other) : _value(*other), _is_error(false)
    {
    }

    template <class _T>
    constexpr result& operator=(ok_t<_T>&& other)
    {
        _is_error ? _error.~E() : _value.~T();
        _is_error = false;
        _value = std::move(*other);
        return *this;
    }

    template <class _E>
    constexpr result(err_t<_E>&& other) : _error(*other), _is_error(true)
    {
    }

    template <class _E>
    constexpr result& operator=(err_t<_E>&& other)
    {
        _is_error ? _error.~E() : _value.~T();
        _is_error = true;
        _error = std::move(*other);
        return *this;
    }

    constexpr void swap(result& other)
    {
        if (this == &other) return;

        if (_is_error && other._is_error)
        {
            std::swap(_error, other._error);
        }
        else if (_is_error) // && !other._is_error
        {
            _is_error = false;
            other._is_error = true;

            E err = vv::relocate(_error);
            T val = vv::relocate(other._value);

            vv::relocate_at(&_value, &val);
            vv::relocate_at(&other._error, &err);
        }
        else if (other._is_error) // && !_is_error
        {
            other._is_error = false;
            _is_error = true;

            E err = vv::relocate(other._error);
            T val = vv::relocate(_value);

            vv::relocate_at(&_error, &err);
            vv::relocate_at(&other._value, &val);
        }
        else // !_is_error && !other._is_error
        {
            std::swap(_value, other._value);
        }
    }

    constexpr explicit operator bool() const { return !_is_error; }
    constexpr bool is_ok() const { return !_is_error; }
    constexpr bool is_error() const { return _is_error; }

    constexpr auto operator*() & -> T& { return _value; }
    constexpr auto operator*() && -> T&& { return std::move(_value); }
    constexpr auto operator*() const& -> const T& { return _value; }
    constexpr auto operator*() const&& -> const T&& { return std::move(_value); }

    constexpr auto value() & -> T& { return _value; }
    constexpr auto value() && -> T&& { return std::move(_value); }
    constexpr auto value() const& -> const T& { return _value; }
    constexpr auto value() const&& -> const T&& { return std::move(_value); }

    constexpr auto error() & -> E& { return _error; }
    constexpr auto error() && -> E&& { return std::move(_error); }
    constexpr auto error() const& -> const E& { return _error; }
    constexpr auto error() const&& -> const E&& { return std::move(_error); }

private:
    union {
        T _value;
        E _error;
    };
    bool _is_error;
};

template <class T>
constexpr auto ok(T&& value) -> ok_t<std::decay_t<T>>
{
    return ok_t(std::move(value));
}

template <class E>
constexpr auto err(E&& error) -> err_t<std::decay_t<E>>
{
    return err_t(std::move(error));
}

template <class T, class E>
constexpr auto ok_if(bool cond, T&& value, E&& error) -> result<std::decay_t<T>, std::decay_t<E>>
{
    if (cond) return ok(value);
    return err(error);
}

}; // namespace vv
