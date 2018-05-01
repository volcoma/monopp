#pragma once

#include "mono_config.h"

#include "mono_assembly.h"
#include "mono_exception.h"
#include "mono_string.h"
#include <string>

namespace mono
{

template <typename T>
struct convert_mono_type
{
	using mono_type_name = T;

	static auto to_mono(mono_assembly&, T&& t) -> T
	{
		return std::forward<T>(t);
	}

	static auto from_mono(T&& t) -> T
	{
		return std::forward<T>(t);
	}
};

template <>
struct convert_mono_type<std::string>
{
	using mono_type_name = MonoString*;

	static auto to_mono(mono_assembly& assembly, const std::string& str) -> MonoString*
	{
		return assembly.new_string(str).get_mono_string();
	}

	static auto from_mono(MonoString* mono_str) -> std::string
	{
		return mono_string(mono_str).str();
	}
};

} // namespace mono
