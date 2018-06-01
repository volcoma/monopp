#pragma once

#include <monopp/mono_type_conversion.h>

namespace mono
{
namespace managed_interface
{

#define register_basic_mono_converter_for_pod(cpp_type, mono_type)                                           \
	template <>                                                                                              \
	struct convert_mono_type<cpp_type>                                                                       \
	{                                                                                                        \
		using mono_type_name = mono_type;                                                                    \
		static_assert(std::is_pod<mono_type>::value, "basic_mono_converter is only for pod types");          \
		static auto to_mono(cpp_type val) -> mono_type_name                                                  \
		{                                                                                                    \
			return managed_interface::converter::convert<cpp_type, mono_type_name>(val);                     \
		}                                                                                                    \
		static auto from_mono_unboxed(mono_type_name val) -> cpp_type                                        \
		{                                                                                                    \
			return managed_interface::converter::convert<mono_type_name, cpp_type>(val);                     \
		}                                                                                                    \
		static auto from_mono_boxed(MonoObject* obj) -> cpp_type                                             \
		{                                                                                                    \
			void* ptr = mono_object_unbox(obj);                                                              \
			return managed_interface::converter::convert<mono_type_name, cpp_type>(                          \
				*reinterpret_cast<mono_type_name*>(ptr));                                                    \
		}                                                                                                    \
	}

struct converter
{
	template <typename src_t, typename dst_t>
	static auto convert(const src_t&) -> dst_t
	{
		static_assert(std::is_same<src_t, dst_t>::value, "Please specialize");
	}
};

} // namespace managed_interface

} // namespace mono
