#pragma once

#include "mono_config.h"

#include "mono_domain.h"
#include "mono_exception.h"
#include "mono_string.h"

#include <string>

namespace mono
{


template<typename T>
inline auto to_mono_arg(T& t)
{
    static_assert(std::is_pod<T>::value, "Should not pass here for non-pod types");
    return std::addressof(t);
}
inline auto to_mono_arg(MonoObject* t)
{
    return t;
}

template <typename T>
struct convert_mono_type
{
	using mono_type_name = T;

    static_assert(std::is_fundamental<T>::value, "Specialize for non fundamental types");

	static auto to_mono(mono_assembly&, T&& t) -> T
	{
		return std::forward<T>(t);
	}

	static auto from_mono(T&& t) -> T
	{
		return std::forward<T>(t);
	}
	
	static auto from_mono(MonoObject* obj) -> T
	{
		return *reinterpret_cast<T*>(mono_object_unbox(obj));
	}
};

template <>
struct convert_mono_type<MonoObject*>
{
	using mono_type_name = MonoObject*;

	static auto to_mono(mono_assembly&, MonoObject* t) -> mono_type_name
	{
		return t;
	}

	static auto from_mono(mono_type_name obj) -> MonoObject*
	{
		return obj;
	}
};

template <>
struct convert_mono_type<std::string>
{
	using mono_type_name = MonoObject*;

	static auto to_mono(mono_assembly& assembly, const std::string& str) -> mono_type_name
	{
		return assembly.new_string(str).get_mono_object();
	}

    static auto from_mono(MonoObject* mono_str) -> std::string
	{
		return mono_string(mono_str).str();
	}
};

} // namespace mono
