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
	using cpp_type = T;
	using mono_unboxed_type = T;
	using mono_boxed_type = MonoObject*;

	static_assert(std::is_scalar<cpp_type>::value, "Specialize for non-scalar types");

	static auto to_mono(const cpp_type& t) -> mono_unboxed_type
	{
		return t;
	}

	static auto from_mono_unboxed(const mono_unboxed_type& t) -> cpp_type
	{
		return t;
	}

	static auto from_mono_boxed(const mono_boxed_type& t) -> cpp_type
	{
		mono_object obj(t);
		const auto& type = obj.get_type();
		const auto mono_sz = type.get_sizeof();
		const auto mono_align = type.get_alignof();
		constexpr auto cpp_sz = sizeof(cpp_type);
		constexpr auto cpp_align = alignof(cpp_type);
		ignore(mono_align, mono_sz, cpp_sz, cpp_align);
		assert(mono_sz <= cpp_sz && mono_align <= cpp_align && "Different type layouts");
		void* ptr = mono_object_unbox(obj.get_internal_ptr());
		return *reinterpret_cast<cpp_type*>(ptr);
	}
};

template <>
struct convert_mono_type<MonoObject*>
{
    using cpp_type = MonoObject*;
	using mono_unboxed_type = MonoObject*;
	using mono_boxed_type = MonoObject*;
    
    static auto to_mono(const cpp_type& t) -> mono_unboxed_type
	{
		return t;
	}

	static auto from_mono_unboxed(const mono_unboxed_type& t) -> cpp_type
	{
		return t;
	}

	static auto from_mono_boxed(const mono_unboxed_type& t) -> cpp_type
	{
        return t;
    }
};

template <>
struct convert_mono_type<std::string>
{
    using cpp_type = std::string;
	using mono_unboxed_type = MonoObject*;
	using mono_boxed_type = MonoObject*;

	static auto to_mono(const cpp_type& str) -> mono_unboxed_type
	{
		const auto& domain = mono_domain::get_current_domain();
		return mono_string(domain, str).get_internal_ptr();
	}

	static auto from_mono_unboxed(const mono_unboxed_type& mono_str) -> cpp_type
	{
		return mono_string(mono_str).str();
	}

	static auto from_mono_boxed(const mono_unboxed_type& mono_str) -> cpp_type
	{
		return mono_string(mono_str).str();
	}
};

} // namespace mono
