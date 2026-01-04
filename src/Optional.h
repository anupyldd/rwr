#pragma once

#include <type_traits>
#include <cassert>
#include <new>
#include <initializer_list>
#include <string>

struct NullOpt {};
inline constexpr NullOpt nopt;

template<typename T>
class Opt
{
public:
    constexpr Opt() {};
    constexpr Opt(NullOpt);

    constexpr Opt(const Opt&);
    constexpr Opt(Opt&&) noexcept;

    constexpr Opt(const T&);
    constexpr Opt(T&&);

    constexpr explicit operator bool () const;

    constexpr auto operator * () & -> T&;
    constexpr auto operator * () const & -> const T&;
    constexpr auto operator * () && -> T&&;
    constexpr auto operator * () const && -> const T&&;

    template<typename U>
    requires std::is_constructible_v<T, U> && std::is_assignable_v<T, U>
    constexpr auto operator = (U&&) -> Opt&;

    constexpr auto operator = (const Opt&) -> Opt&;
    constexpr auto operator = (Opt&&) -> Opt&;

    constexpr auto operator = (NullOpt) -> Opt&;

    constexpr auto operator = (std::initializer_list<std::nullptr_t>) -> Opt&;

    constexpr auto operator -> () -> T*;

    constexpr ~Opt();

private:
    auto Destroy() -> void;

private:
    union { T value; };
    bool hasValue = false;
};

/*********************************************************/

template<typename T>
constexpr Opt<T>::~Opt()
{
    Destroy();
}

template<typename T>
auto Opt<T>::Destroy() -> void
{
    if (hasValue)
    {
        value.~T();
        hasValue = false;
    }
}

template<typename T>
constexpr Opt<T>::Opt(NullOpt)
{
}

template<typename T>
constexpr Opt<T>::Opt(const Opt& other)
    : hasValue(other.hasValue)
{
    if (hasValue) new(&value) T(other.value);
}

template<typename T>
constexpr Opt<T>::Opt(Opt&& other) noexcept
    : hasValue(other.hasValue)
{
    if (hasValue)
    {
        new(&value) T(std::move(other.value));
        other.Destroy();
    }
}

template<typename T>
constexpr Opt<T>::Opt(const T& val)
    : hasValue(true)
{
    new(&value) T(val);
}

template<typename T>
constexpr Opt<T>::Opt(T&& val)
    : hasValue(true)
{
    new(&value) T(std::move(val));
}

template<typename T>
constexpr Opt<T>::operator bool() const
{
    return hasValue;
}

template<typename T>
constexpr auto Opt<T>::operator * () & -> T&
{
    assert(hasValue);
    return value;
}

template<typename T>
constexpr auto Opt<T>::operator * () const & -> const T&
{
    assert(hasValue);
    return value;
}

template<typename T>
constexpr auto Opt<T>::operator * () && -> T&&
{
    assert(hasValue);
    return std::move(value);
}

template<typename T>
constexpr auto Opt<T>::operator * () const && -> const T&&
{
    assert(hasValue);
    return std::move(value);
}

template<typename T>
template<typename U>
requires std::is_constructible_v<T, U> && std::is_assignable_v<T, U>
constexpr auto Opt<T>::operator = (U&& val) -> Opt&
{
    if (hasValue)
    {
        value = std::forward<U>(val);
    }
    else
    {
        new(&value) T(std::forward<U>(val));
        hasValue = true;
    }
    return *this;
}

template<typename T>
constexpr auto Opt<T>::operator = (const Opt& other) -> Opt&
{
    if (other)
    {
        if (hasValue)
        {
            value = other.value;
        }
        else
        {
            new(&value) T(other.value);
            hasValue = true;
        }
    }
    else
    {
        Destroy();
    }
    return *this;
}

template<typename T>
constexpr auto Opt<T>::operator = (Opt&& other) -> Opt&
{
    if (other)
    {
        if (hasValue)
        {
            value = std::move(other.value);
        }
        else
        {
            new(&value) T(std::move(other.value));
            hasValue = true;
        }
        other.Destroy();
    }
    else
    {
        Destroy();
    }
    return *this;
}

template<typename T>
constexpr auto Opt<T>::operator = (NullOpt) -> Opt&
{
    Destroy();
    return *this;
}

template<typename T>
constexpr auto Opt<T>::operator = (std::initializer_list<std::nullptr_t>) -> Opt&
{
    Destroy();
    return *this;
}

template<typename T>
constexpr auto Opt<T>::operator -> () -> T*
{
    return &value;
}

/*********************************************************/

class NoDefault {
public:
    NoDefault(int x) : value(x) {}
    ~NoDefault() {}
    int value;
};

auto OptionalTest()
{
    Opt<int> o1;
    o1 = 10;
    o1 = {};
    o1 = nopt;

    Opt<int> o2{ 1 };
    o2 = 2;

    Opt<NoDefault> o3{2};
    o3 = 10;

    auto fn = [](bool b) -> Opt<int> { if (b) return 10; else return {}; };
    auto o4 = fn(true);
    auto o5 = fn(false);

    auto o6 = Opt<std::string>{};
    o6 = "hello";
    auto o7 = Opt<std::string>{"bye"};
    o6 = o7;
    o6 = {};
    o7 = {};

    Opt<std::initializer_list<int>> o8;
    o8 = {1,2,3};
    o8 = {};
}