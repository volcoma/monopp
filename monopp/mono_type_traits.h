#pragma once
#include <cstddef>
#include <functional>
#include <map>
#include <tuple>
#include <type_traits>
#include <vector>

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
	using arg_types = std::tuple<Args...>;
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

/////////////////////////////////////////////////////////////////////

template <typename F, typename Tuple, std::size_t... I>
decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
	return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}
template <typename F, typename Tuple>
decltype(auto) apply(F&& f, Tuple&& t)
{
	using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
	return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices());
}

namespace types
{
using index_t = size_t;

inline index_t& get_counter()
{
	static index_t value = 0;
	return value;
}

template <typename T>
inline index_t id()
{
	static index_t sid = get_counter()++;
	return sid;
}

inline const std::map<index_t, std::string>& get_types()
{
	// valid shortcuts are
	// char, bool, byte, sbyte, uint16,
	// int16, uint, int, ulong, long, uintptr,
	// intptr, single, double, string and object.

	// clang-format off
	static const std::map<index_t, std::string> types_ =
    {
		{id<std::int8_t>(), "sbyte"},
        {id<std::uint8_t>(), "byte"}, 
        {id<std::int16_t>(), "short"},
		{id<std::uint16_t>(), "ushort"},
        {id<std::int32_t>(), "int"},
        {id<std::uint32_t>(), "uint"},
		{id<std::int64_t>(), "long"},
        {id<std::uint64_t>(), "ulong"},
        {id<bool>(), "bool"},
		{id<float>(), "single"},
        {id<double>(), "double"},
        {id<char16_t>(), "char"},
		{id<std::string>(), "string"}
    };
	// clang-format on

	return types_;
}

template <typename T>
inline std::string get_name(bool& found)
{
	const auto& types = get_types();
	auto it = types.find(id<std::decay_t<T>>());
	if(it == types.end())
	{
		found |= false;
		return "unknown";
	}
	found |= true;
	return it->second;
}

template <typename... Args>
inline std::pair<std::string, bool> get_args_signature(const std::tuple<Args...>& tup)
{
	bool all_types_known = false;
	auto inv = [&all_types_known](auto... args) {
		std::vector<std::string> argsv = {types::get_name<decltype(args)>(all_types_known)...};

		std::string result;
		size_t i = 0;
		for(const auto& tp : argsv)
		{
			if(i++ > 0)
			{
				result += ',';
			}

			result += tp;
		}

		return result;
	};

	auto args = apply(inv, tup);
	return std::make_pair(args, all_types_known);
}
}
}
