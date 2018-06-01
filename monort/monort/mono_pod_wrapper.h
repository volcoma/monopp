#pragma once

#include <monopp/mono_type_conversion.h>

namespace mono
{
namespace managed_interface
{

#define register_basic_mono_converter_for_pod(cpp_type_raw, mono_data_type_aligned)                          \
	template <>                                                                                              \
	struct convert_mono_type<cpp_type_raw>                                                                   \
	{                                                                                                        \
		using cpp_type = cpp_type_raw;                                                                       \
		using mono_unboxed_type = mono_data_type_aligned;                                                    \
		using mono_boxed_type = MonoObject*;                                                                 \
                                                                                                             \
		static_assert(std::is_pod<mono_unboxed_type>::value, "basic_mono_converter is only for pod types");  \
                                                                                                             \
		static auto to_mono(const cpp_type& obj) -> mono_unboxed_type                                        \
		{                                                                                                    \
			return managed_interface::converter::convert<cpp_type, mono_unboxed_type>(obj);                  \
		}                                                                                                    \
		static auto from_mono_unboxed(const mono_unboxed_type& obj) -> cpp_type                              \
		{                                                                                                    \
			return managed_interface::converter::convert<mono_unboxed_type, cpp_type>(obj);                  \
		}                                                                                                    \
		static auto from_mono_boxed(const mono_boxed_type& obj) -> cpp_type                                  \
		{                                                                                                    \
			void* ptr = mono_object_unbox(obj);                                                              \
			return managed_interface::converter::convert<mono_unboxed_type, cpp_type>(                       \
				*reinterpret_cast<mono_unboxed_type*>(ptr));                                                 \
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
