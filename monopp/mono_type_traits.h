#pragma once
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>

namespace mono
{

template <typename T>
using is_mono_valuetype = std::is_pod<T>;


template <typename Functor>
struct function_traits : public function_traits<decltype(&Functor::operator())>
{
};

template <typename R, typename... Args>
struct function_traits<R(Args...)>
{
	using return_type = R;
	using function_type = R(Args...);
	constexpr static const std::size_t arity = sizeof...(Args);
};

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)>
{
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...)> : public function_traits<R(Args...)>
{
	using owner_type = C&;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const> : public function_traits<R(Args...)>
{
	using owner_type = const C&;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) volatile> : public function_traits<R(Args...)>
{
	using owner_type = volatile C&;
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const volatile> : public function_traits<R(Args...)>
{
	using owner_type = const volatile C&;
};

template <typename Functor>
struct function_traits<std::function<Functor>> : public function_traits<Functor>
{
};

template <typename T>
struct function_traits<T&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<const T&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<volatile T&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<const volatile T&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<T&&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<const T&&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<volatile T&&> : public function_traits<T>
{
};

template <typename T>
struct function_traits<const volatile T&&> : public function_traits<T>
{
};

}
