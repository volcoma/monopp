#pragma once

#include "mono_config.h"

#include "mono_domain.h"
#include "mono_exception.h"
#include "mono_string.h"
#include "mono_type_traits.h"
#include <cstring>

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

	static auto to_mono(const mono_assembly&, const cpp_type_name& t) -> mono_type_name
	{
		return t;
	}

	static auto from_mono(const mono_type_name& t) -> cpp_type_name
	{
		return t;
	}

	static auto from_mono(MonoObject* obj) -> cpp_type_name
	{
		auto mono_cls = mono_object_get_class(obj);
		bool valuetype = !!mono_class_is_valuetype(mono_cls);
		(void)valuetype;
		assert(valuetype && "Mono type should be a valuetype in this specialization");
		std::uint32_t mono_align = 0;
		const auto mono_sz = std::uint32_t(mono_class_value_size(mono_cls, &mono_align));
		constexpr auto cpp_sz = sizeof(cpp_type_name);
		constexpr auto cpp_align = alignof(cpp_type_name);
		(void)mono_sz;
		(void)cpp_sz;
		(void)cpp_align;
		assert(mono_sz <= cpp_sz && mono_align <= cpp_align && "Different type layouts");
		void* ptr = mono_object_unbox(obj);
		return *reinterpret_cast<cpp_type_name*>(ptr);
	}
};

template <>
struct convert_mono_type<MonoObject*>
{
	using cpp_type_name = MonoObject*;
	using mono_type_name = MonoObject*;

	static auto to_mono(const mono_assembly&, cpp_type_name t) -> mono_type_name
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

	static auto to_mono(const mono_assembly& assembly, const cpp_type_name& str) -> mono_type_name
	{
		return assembly.new_string(str).get_mono_object();
	}

	static auto from_mono(mono_type_name mono_str) -> cpp_type_name
	{
		return mono_string(mono_str).str();
	}
};

} // namespace mono
