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
auto check_pod_type(MonoObject* obj) -> bool
{
	mono_object object(obj);
	const auto& type = object.get_type();
	const auto mono_sz = type.get_sizeof();
	const auto mono_align = type.get_alignof();
	constexpr auto cpp_sz = sizeof(T);
	constexpr auto cpp_align = alignof(T);

	return mono_sz <= cpp_sz && mono_align <= cpp_align;
}

template <typename T>
struct convert_mono_type
{
	using cpp_type = T;
	using mono_unboxed_type = T;
	using mono_boxed_type = MonoObject*;

	static_assert(std::is_trivially_copyable<mono_unboxed_type>::value,
				  "Specialize convertor for non-pod types");

	static auto to_mono(const cpp_type& obj) -> mono_unboxed_type
	{
		return obj;
	}

	static auto from_mono_unboxed(const mono_unboxed_type& obj) -> cpp_type
	{
		return obj;
	}

	static auto from_mono_boxed(const mono_boxed_type& obj) -> cpp_type
	{
		assert(check_pod_type<mono_unboxed_type>(obj) && "Different type layouts");
		void* ptr = mono_object_unbox(obj);
		return *reinterpret_cast<cpp_type*>(ptr);
	}
};

template <>
struct convert_mono_type<mono_object>
{
	using cpp_type = mono_object;
	using mono_unboxed_type = MonoObject*;
	using mono_boxed_type = MonoObject*;

	static auto to_mono(const cpp_type& obj) -> mono_unboxed_type
	{
		return obj.get_internal_ptr();
	}

	static auto from_mono_unboxed(const mono_unboxed_type& obj) -> cpp_type
	{
		return cpp_type(obj);
	}

	static auto from_mono_boxed(const mono_unboxed_type& obj) -> cpp_type
	{
		return cpp_type(obj);
	}
};

template <>
struct convert_mono_type<mono_type>
{
	using cpp_type = mono_type;
	using mono_unboxed_type = MonoReflectionType*;
	using mono_boxed_type = MonoReflectionType*;

	static auto to_mono(const cpp_type& obj) -> mono_unboxed_type
	{
		// Get the current Mono domain
		MonoDomain* domain = mono_domain_get();

		// Obtain the MonoType* from your cpp_type (assuming a getter function)
		MonoType* monoType = mono_class_get_type(obj.get_internal_ptr());

		// Convert MonoType* to MonoReflectionType*
		MonoReflectionType* reflectionType = mono_type_get_object(domain, monoType);

		return reflectionType;
	}

	static auto from_mono_unboxed(const mono_unboxed_type& obj) -> cpp_type
	{
		MonoType* monoType = mono_reflection_type_get_type(obj);
		return cpp_type(monoType);
	}

	static auto from_mono_boxed(const mono_unboxed_type& obj) -> cpp_type
	{
		MonoType* monoType = mono_reflection_type_get_type(obj);
		return cpp_type(monoType);
	}
};

template <>
struct convert_mono_type<std::string>
{
	using cpp_type = std::string;
	using mono_unboxed_type = MonoObject*;
	using mono_boxed_type = MonoObject*;

	static auto to_mono(const cpp_type& obj) -> mono_unboxed_type
	{
		const auto& domain = mono_domain::get_current_domain();
		return mono_string(domain, obj).get_internal_ptr();
	}

	static auto from_mono_unboxed(const mono_unboxed_type& obj) -> cpp_type
	{
		if(!obj)
		{
			return {};
		}
		return mono_string(mono_object(obj)).as_utf8();
	}

	static auto from_mono_boxed(const mono_boxed_type& obj) -> cpp_type
	{
		if(!obj)
		{
			return {};
		}
		return mono_string(mono_object(obj)).as_utf8();
	}
};

} // namespace mono
