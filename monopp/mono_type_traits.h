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
	using arg_types_decayed = std::tuple<std::decay_t<Args>...>;
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

template <typename... Args>
inline void ignore(Args&&...)
{
}

template <typename F, typename Tuple, std::size_t... I>
decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
	ignore(t);
	return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}
template <typename F, typename Tuple>
decltype(auto) apply(F&& f, Tuple&& t)
{
	using indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
	return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), indices());
}

template <typename Tuple, typename F, std::size_t... Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>)
{
	ignore(tuple, f);
	using swallow = int[];
	(void)swallow{1, (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...};
}

template <typename Tuple, typename F>
void for_each(Tuple&& tuple, F&& f)
{
	for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
				  std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
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

struct type_names_t
{
	std::string name;
	std::string fullname;
};

inline const std::map<index_t, type_names_t>& get_types()
{
	// valid shortcuts are
	// char, bool, byte, sbyte, uint16,
	// int16, uint, int, ulong, long, uintptr,
	// intptr, single, double, string and object.

	// clang-format off
	static const std::map<index_t, type_names_t> types =
    {
        {id<std::int8_t>(), {"sbyte", "System.SByte"}},
        {id<std::uint8_t>(), {"byte", "System.Byte"}}, 
        {id<std::int16_t>(), {"short", "System.Int16"}},
        {id<std::uint16_t>(), {"ushort", "System.UInt16"}},
        {id<std::int32_t>(), {"int", "System.Int32"}},
        {id<std::uint32_t>(), {"uint", "System.UInt32"}},
        {id<std::int64_t>(), {"long", "System.Int64"}},
        {id<std::uint64_t>(), {"ulong", "System.UInt64"}},
        {id<bool>(), {"bool", "System.Boolean"}},
        {id<float>(), {"single", "System.Single"}},
        {id<double>(), {"double", "System.Double"}},
        {id<char16_t>(), {"char", "System.Char"}},
        {id<std::string>(), {"string", "System.String"}},
        {id<void>(), {"void", "System.Void"}}
    };
	// clang-format on

	return types;
}

template <typename T>
inline type_names_t get_name(bool& found)
{
	const auto& types = get_types();
	auto it = types.find(id<std::decay_t<T>>());
	if(it == types.end())
	{
		found |= false;
		return {"unknown", "Unknown"};
	}
	found |= true;
	return it->second;
}

template <typename... Args>
inline std::pair<std::string, bool> get_args_signature(const std::tuple<Args...>& tup)
{
	bool all_types_known = false;
	auto inv = [&all_types_known](auto... args) {
		std::vector<std::string> argsv = {types::get_name<decltype(args)>(all_types_known).name...};
		ignore(args...);
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

template <typename T>
bool is_compatible_type(const std::string& expected_name)
{
	bool found = false;

	auto name = types::get_name<T>(found).fullname;

	if(found)
	{
		return name == expected_name;
	}

	return true;
}
} // namespace types
} // namespace mono
