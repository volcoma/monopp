#pragma once

#include "mono_config.h"

#include "mono_domain.h"
#include "mono_exception.h"
#include "mono_string.h"
#include "mono_type_traits.h"

namespace mono
{

template <typename T>
inline auto to_mono_arg(T& t)
{
	static_assert(is_mono_valuetype<T>::value, "Should not pass here for non-pod types");
	return std::addressof(t);
}
inline auto to_mono_arg(MonoObject* t)
{
	return t;
}

template <typename T>
struct convert_mono_type
{
	using cpp_type_name = T;
	using mono_type_name = T;

	static_assert(is_mono_valuetype<T>::value, "Specialize for non-pod types");

	static auto to_mono(mono_assembly&, cpp_type_name&& t) -> mono_type_name
	{
		return std::forward<mono_type_name>(t);
	}

	static auto to_mono(mono_assembly&, const cpp_type_name& t) -> mono_type_name
	{
		return t;
	}

	static auto from_mono(mono_type_name&& t) -> cpp_type_name
	{
		return std::forward<mono_type_name>(t);
	}

	static auto from_mono(const mono_type_name& t) -> cpp_type_name
	{
		return t;
	}

	static auto from_mono(MonoObject* obj) -> cpp_type_name
	{
		return *reinterpret_cast<cpp_type_name*>(mono_object_unbox(obj));
	}
};

template <>
struct convert_mono_type<MonoObject*>
{
	using cpp_type_name = MonoObject*;
	using mono_type_name = MonoObject*;

	static auto to_mono(mono_assembly&, cpp_type_name t) -> mono_type_name
	{
		return t;
	}

	static auto from_mono(mono_type_name obj) -> cpp_type_name
	{
		return obj;
	}
};

template <>
struct convert_mono_type<std::string>
{
	using cpp_type_name = std::string;
	using mono_type_name = MonoObject*;

	static auto to_mono(mono_assembly& assembly, const cpp_type_name& str) -> mono_type_name
	{
		return assembly.new_string(str).get_mono_object();
	}

	static auto from_mono(mono_type_name mono_str) -> cpp_type_name
	{
		return mono_string(mono_str).str();
	}
};

} // namespace mono
