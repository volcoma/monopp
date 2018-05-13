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
		using mono_type_name = MonoObject*;                                                                  \
		static_assert(std::is_pod<mono_type>::value, "basic_mono_converter is only for pod types");          \
		static auto to_mono(mono_assembly&, cpp_type val) -> mono_type                                       \
		{                                                                                                    \
			return managed_interface::converter::convert<cpp_type, mono_type>(val);                          \
		}                                                                                                    \
                                                                                                             \
		static auto from_mono(mono_type_name val) -> cpp_type                                                \
		{                                                                                                    \
			return managed_interface::converter::convert<mono_type, cpp_type>(                               \
				*reinterpret_cast<mono_type*>(mono_object_unbox(val)));                                      \
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
