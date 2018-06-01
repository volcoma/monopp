#pragma once

#include "mono_config.h"

#include "mono_domain.h"
#include "mono_string.h"
#include "mono_type.h"
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

	static_assert(std::is_scalar<cpp_type_name>::value, "Specialize for non-scalar types");

	static auto to_mono(const cpp_type_name& t) -> mono_type_name
	{
		return t;
	}

	static auto from_mono_unboxed(const mono_type_name& t) -> cpp_type_name
	{
		return t;
	}

	static auto from_mono_boxed(MonoObject* t) -> cpp_type_name
	{
		mono_object obj(t);
		const auto& type = obj.get_type();
		const auto mono_sz = type.get_sizeof();
		const auto mono_align = type.get_alignof();
		constexpr auto cpp_sz = sizeof(cpp_type_name);
		constexpr auto cpp_align = alignof(cpp_type_name);
		ignore(mono_align, mono_sz, cpp_sz, cpp_align);
		assert(mono_sz <= cpp_sz && mono_align <= cpp_align && "Different type layouts");
		void* ptr = mono_object_unbox(obj.get_internal_ptr());
		return *reinterpret_cast<cpp_type_name*>(ptr);
	}
};

template <>
struct convert_mono_type<MonoObject*>
{
	using cpp_type_name = MonoObject*;
	using mono_type_name = MonoObject*;

	static auto to_mono(cpp_type_name t) -> mono_type_name
	{
		return t;
	}
	static auto from_mono_unboxed(mono_type_name obj) -> cpp_type_name
	{
		return obj;
	}

	static auto from_mono_boxed(MonoObject* obj) -> cpp_type_name
	{
		return obj;
	}
};

template <>
struct convert_mono_type<std::string>
{
	using cpp_type_name = std::string;
	using mono_type_name = MonoObject*;

	static auto to_mono(const cpp_type_name& str) -> mono_type_name
	{
		const auto& domain = mono_domain::get_current_domain();
		return mono_string(domain, str).get_internal_ptr();
	}

	static auto from_mono_unboxed(mono_type_name mono_str) -> cpp_type_name
	{
		return mono_string(mono_str).str();
	}

	static auto from_mono_boxed(MonoObject* mono_str) -> cpp_type_name
	{
		return mono_string(mono_str).str();
	}
};

} // namespace mono
